#include <mbgl/geometry/dem_data.hpp>
#include <mbgl/math/clamp.hpp>
#include <threepp/threepp.hpp>
#include <mutex>
namespace mbgl {
static std::mutex mutex; 
static std::shared_ptr<threepp::BufferGeometry> terrain_geo{nullptr};

DEMData::DEMData(const PremultipliedImage& _image, Tileset::DEMEncoding _encoding)
    : dim(_image.size.height),
      // extra two pixels per row for border backfilling on either edge
      stride(dim + 2),
      encoding(_encoding) {
    image = std::make_shared<PremultipliedImage>(Size(static_cast<uint32_t>(stride), static_cast<uint32_t>(stride)));
    if (_image.size.height != _image.size.width) {
        throw std::runtime_error("raster-dem tiles must be square.");
    }
    ++count; 
    auto* dest = reinterpret_cast<uint32_t*>(image->data.get()) + stride + 1;
    auto* source = reinterpret_cast<uint32_t*>(_image.data.get());
    for (int32_t y = 0; y < dim; y++) {
        memcpy(dest, source, dim * 4);
        dest += stride;
        source += dim;
    }

    // in order to avoid flashing seams between tiles, here we are initially
    // populating a 1px border of pixels around the image with the data of the
    // nearest pixel from the image. this data is eventually replaced when the
    // tile's neighboring tiles are loaded and the accurate data can be
    // backfilled using DEMData#backfillBorder

    auto* data = reinterpret_cast<uint32_t*>(image->data.get());
    for (int32_t x = 0; x < dim; x++) {
        auto rowOffset = stride * (x + 1);
        // left vertical border
        data[rowOffset] = data[rowOffset + 1];

        // right vertical border
        data[rowOffset + dim + 1] = data[rowOffset + dim];
    }

    // top horizontal border with corners
    memcpy(data, data + stride, stride * 4);
    // bottom horizontal border with corners
    memcpy(data + (dim + 1) * stride, data + dim * stride, stride * 4);

    // create elevation data
    // height = -10000 + ((R * 256 * 256 + G * 256 + B) * 0.1)
    auto size_ = image->size;
    int segmentX = 65; 
    int segmentY = 65; 
    mutex.lock(); 
    if (!terrain_geo) {
        terrain_geo = threepp::BufferGeometry::create();
        terrain_geo = threepp::PlaneGeometry::create(8192, 8192, segmentX, segmentY);
        terrain_geo->applyMatrix4(threepp::Matrix4().makeRotationY(-threepp::math::PI));
        terrain_geo->scale(-1, 1, 1);
        terrain_geo->translate(4096, 4096, 0);
    }
    mutex.unlock();
    int c = image->channels; 
    int elevationIdx{0}; 
    std::vector<float> elevation;
    elevation.reserve(size_.width * size_.height); 
    for (int y = 0; y < size_.height; ++y) {
        for (int x = 0; x < size_.width; ++x) 
        {
            int idx = (y * size_.width + x) * c;
            int R = image->data[idx + 0];
            int G = image->data[idx + 1];
            int B = image->data[idx + 2];
            float height = -10000.f + ((R * 1.0f * 65536.0f + G * 256.0f + B * 1.0f) * 0.1f);
            elevation.emplace_back(height); 
            elevationIdx = y* size_.width + x; 
        }
    }
    auto new_terraingeo = terrain_geo->clone(); 
    auto pos = new_terraingeo->getAttribute<float>("position");

    for (int i = 0; i < pos->count(); ++i) {
        float worldX = pos->getX(i); // [0..8192]
        float worldY = pos->getY(i);

        int px = (int)((worldX / 8192.0f) * (size_.width - 1));
        int py = (int)((worldY / 8192.0f) * (size_.height - 1));

        if (px < 0) px = 0;
        if (px >= size_.width) px = size_.width - 1;
        if (py < 0) py = 0;
        if (py >= size_.height) py = size_.height - 1;

        int idx = py * size_.width + px;
        pos->setZ(i, elevation[idx]);
    }
    pos->needsUpdate();
    new_terraingeo->computeVertexNormals();
    new_terraingeo->computeBoundingSphere();
    new_terraingeo->computeBoundingBox(); 
    auto terrain_mat = threepp::MeshBasicMaterial::create();
    terrain_mat->side = threepp::Side::Double;
    terrain_mesh = threepp::Mesh::create(new_terraingeo, terrain_mat); 
    terrain_mesh->visible = false; 
    terrain_mesh->matrixAutoUpdate = false; 
}


// This function takes the DEMData from a neighboring tile and backfills the
// edge/corner data in order to create a one pixel "buffer" of image data around
// the tile. This is necessary because the hillshade formula calculates the
// dx/dz, dy/dz derivatives at each pixel of the tile by querying the 8
// surrounding pixels, and if we don't have the pixel buffer we get seams at
// tile boundaries.
void DEMData::backfillBorder(const DEMData& borderTileData, int8_t dx, int8_t dy) {
    auto& o = borderTileData;

    // Tiles from the same source should always be of the same dimensions.
    assert(dim == o.dim);

    // We determine the pixel range to backfill based which corner/edge
    // `borderTileData` represents. For example, dx = -1, dy = -1 represents the
    // upper left corner of the base tile, so we only need to backfill one pixel
    // at coordinates (-1, -1) of the tile image.
    int32_t xMin = dx * dim;
    int32_t xMax = dx * dim + dim;
    int32_t yMin = dy * dim;
    int32_t yMax = dy * dim + dim;

    if (dx == -1)
        xMin = xMax - 1;
    else if (dx == 1)
        xMax = xMin + 1;

    if (dy == -1)
        yMin = yMax - 1;
    else if (dy == 1)
        yMax = yMin + 1;

    int32_t ox = -dx * dim;
    int32_t oy = -dy * dim;

    auto* dest = reinterpret_cast<uint32_t*>(image->data.get());
    auto* source = reinterpret_cast<uint32_t*>(o.image->data.get());

    for (int32_t y = yMin; y < yMax; y++) {
        for (int32_t x = xMin; x < xMax; x++) {
            dest[idx(x, y)] = source[idx(x + ox, y + oy)];
        }
    }
}

void DEMData::releaseGeo() {
    isRelaseGeo = true; 
 }

int32_t DEMData::get(const int32_t x, const int32_t y) const {
    const auto& unpack = getUnpackVector();
    const uint8_t* value = image->data.get() + idx(x, y) * 4;
    return static_cast<int32_t>(value[0] * unpack[0] + value[1] * unpack[1] + value[2] * unpack[2] - unpack[3]);
}

const std::array<float, 4>& DEMData::getUnpackVector() const {
    // https://www.mapbox.com/help/access-elevation-data/#mapbox-terrain-rgb
    static const std::array<float, 4> unpackMapbox = {{6553.6f, 25.6f, 0.1f, 10000.0f}};
    // https://aws.amazon.com/public-datasets/terrain/
    static const std::array<float, 4> unpackTerrarium = {{256.0f, 1.0f, 1.0f / 256.0f, 32768.0f}};

    return encoding == Tileset::DEMEncoding::Terrarium ? unpackTerrarium : unpackMapbox;
}

} // namespace mbgl
