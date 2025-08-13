#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <mbgl/style/layers/custom_drawable_layer_impl.hpp>

#include <mbgl/renderer/layers/render_custom_drawable_layer.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/change_request.hpp>
#include <mbgl/renderer/layer_group.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/shaders/shader_program_base.hpp>
#include <mbgl/renderer/layer_tweaker.hpp>
#include <mbgl/gfx/drawable.hpp>
#include <mbgl/gfx/drawable_tweaker.hpp>
#include <mbgl/shaders/line_layer_ubo.hpp>
#include <mbgl/util/convert.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/shaders/fill_layer_ubo.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/containers.hpp>
#include <mbgl/gfx/fill_generator.hpp>
#include <mbgl/shaders/custom_drawable_layer_ubo.hpp>
#include <mbgl/shaders/shader_source.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/shaders/widevector_ubo.hpp>
#include <mbgl/shaders/custom_geometry_ubo.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/gfx/uniform_buffer.hpp>

#include <cmath>

namespace mbgl {

namespace style {

using namespace shaders;

namespace {
const LayerTypeInfo typeInfoCustomDrawable{"custom-drawable",
                                           LayerTypeInfo::Source::NotRequired,
                                           LayerTypeInfo::Pass3D::Required,
                                           LayerTypeInfo::Layout::NotRequired,
                                           LayerTypeInfo::FadingTiles::NotRequired,
                                           LayerTypeInfo::CrossTileIndex::NotRequired,
                                           LayerTypeInfo::TileKind::NotRequired};
} // namespace



CustomDrawableLayer::CustomDrawableLayer(const std::string& layerID, std::unique_ptr<CustomDrawableLayerHost> host)
    : Layer(makeMutable<Impl>(layerID, std::move(host))) {}

CustomDrawableLayer::~CustomDrawableLayer() = default;

const CustomDrawableLayer::Impl& CustomDrawableLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<CustomDrawableLayer::Impl> CustomDrawableLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> CustomDrawableLayer::cloneRef(const std::string&) const {
    assert(false);
    return nullptr;
}

using namespace conversion;

std::optional<Error> CustomDrawableLayer::setPropertyInternal(const std::string&, const Convertible&) {
    return Error{"layer doesn't support this property"};
}

StyleProperty CustomDrawableLayer::getProperty(const std::string&) const {
    return {};
}

Mutable<Layer::Impl> CustomDrawableLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

const LayerTypeInfo* CustomDrawableLayer::Impl::staticTypeInfo() noexcept {
    return &typeInfoCustomDrawable;
}

CustomDrawableLayerHost::Interface::Interface(RenderLayer& layer_,
                                              LayerGroupBasePtr& layerGroup_,
                                              gfx::ShaderRegistry& shaders_,
                                              gfx::Context& context_,
                                              const TransformState& state_,
                                              const std::shared_ptr<UpdateParameters>& updateParameters_,
                                              const RenderTree& renderTree_,
                                              UniqueChangeRequestVec& changes_)
    : layer(layer_),
      layerGroup(layerGroup_),
      context(context_),
      state(state_),
      updateParameters(updateParameters_),
      renderTree(renderTree_),
      changes(changes_) {
    // ensure we have a default layer group set up
    if (!layerGroup) {
        if (auto aLayerGroup = context.createTileLayerGroup(
                /*layerIndex*/ layer.getLayerIndex(), /*initialCapacity=*/64, layer.getID())) {
            changes.emplace_back(std::make_unique<AddLayerGroupRequest>(aLayerGroup));
            layerGroup = std::move(aLayerGroup);
        }
    }
}

std::size_t CustomDrawableLayerHost::Interface::getDrawableCount() const {
    return layerGroup->getDrawableCount();
}

void CustomDrawableLayerHost::Interface::setTileID(OverscaledTileID tileID_) {
    tileID = tileID_;
}

void CustomDrawableLayerHost::Interface::addCustomDrawable() {
    setTileID({16, 52186, 30788});
    custom_builder = createCustomBuilder("threepp");
    auto id = custom_builder->getCurrentDrawable(true)->getID(); 
    custom_builder->customFlush(context); 
    for (auto& drawable : custom_builder->clearDrawables()) {
        TileLayerGroup* tileLayerGroup = static_cast<TileLayerGroup*>(layerGroup.get());
        drawable->setTileID(tileID.value());
        drawable->setIs3D(true); 
        tileLayerGroup->addDrawable(RenderPass::Translucent, tileID.value(), std::move(drawable));
    }
}

void CustomDrawableLayerHost::Interface::addCustomDrawableWithTile(mbgl::OverscaledTileID tileID_) 
{
    setTileID(tileID_); 
    custom_builder.reset(); 
    custom_builder = createCustomBuilder(m_BuilderName);
    auto id = custom_builder->getCurrentDrawable(true)->getID();
    custom_builder->customFlush(context);
    for (auto& drawable : custom_builder->clearDrawables()) {
        TileLayerGroup* tileLayerGroup = static_cast<TileLayerGroup*>(layerGroup.get());
        drawable->setTileID(tileID.value());
        drawable->setIs3D(true);
        tileLayerGroup->addDrawable(RenderPass::Translucent, tileID.value(), std::move(drawable));
    }
    TileLayerGroup* tileLayerGroup = static_cast<TileLayerGroup*>(layerGroup.get());
}

void CustomDrawableLayerHost::Interface::removeDrawable(const util::SimpleIdentity& id) {
    TileLayerGroup* tileLayerGroup = static_cast<TileLayerGroup*>(layerGroup.get());
    tileLayerGroup->removeDrawablesIf([&](gfx::Drawable& drawable) { return drawable.getID() == id; });
}

LayerGroupBasePtr& CustomDrawableLayerHost::Interface::getLayerGroupBase() {
    return layerGroup; 
}



std::unique_ptr<gfx::DrawableBuilder> CustomDrawableLayerHost::Interface::createCustomBuilder(
    const std::string& name) const {
    std::unique_ptr<gfx::DrawableBuilder> builder_ = context.createCustomDrawableBuilder(name);
    builder_->setEnableDepth(true);
    builder_->setRenderPass(RenderPass::Translucent);
    return builder_; 
}

} // namespace style
} // namespace mbgl
