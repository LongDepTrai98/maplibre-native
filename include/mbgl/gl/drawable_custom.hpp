#pragma once
#include <mbgl/gfx/drawable.hpp>
#include <mbgl/gfx/draw_mode.hpp>
#include <mbgl/gfx/drawable_tweaker.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/vertex_attribute_gl.hpp>
#include <mbgl/gl/uniform_buffer_gl.hpp>
#include <memory>

namespace mbgl {

class SegmentBase;
class PaintParameters;
namespace style {
class CustomDrawableTweaker : public gfx::DrawableTweaker {
public:
    CustomDrawableTweaker() = default; 
    ~CustomDrawableTweaker() override = default;
    void init(gfx::Drawable&) override; 
    void execute(gfx::Drawable& drawable, PaintParameters& parameters) override; 
    public: 
        mbgl::mat4 tile_matrix; 
};
}

namespace gfx {

class IndexBuffer;
class UploadPass;
class VertexBufferResource;

using UniqueVertexBufferResource = std::unique_ptr<VertexBufferResource>;

} // namespace gfx
namespace gl {
class DrawableCustom : public gfx::Drawable {
public:
    DrawableCustom(std::string name);
    ~DrawableCustom() override;

    void draw(PaintParameters&) const override;

    struct DrawSegmentGL;
    void setIndexData(gfx::IndexVectorBasePtr, std::vector<UniqueDrawSegment> segments) override;

    void setVertices(std::vector<uint8_t>&&, std::size_t, gfx::AttributeDataType) override;

    const gfx::UniformBufferArray& getUniformBuffers() const override;
    gfx::UniformBufferArray& mutableUniformBuffers() override;

    void setVertexAttrId(const size_t id);

    void upload(gfx::UploadPass&);

    void updateVertexAttributes(gfx::VertexAttributeArrayPtr,
                                std::size_t vertexCount,
                                gfx::DrawMode,
                                gfx::IndexVectorBasePtr,
                                const SegmentBase* segments,
                                std::size_t segmentCount) override;
    UniformBufferArrayGL tmp_array_gl; 
protected:
    class Impl;
    std::unique_ptr<Impl> impl;
private:
};
}
} // namespace mbgl