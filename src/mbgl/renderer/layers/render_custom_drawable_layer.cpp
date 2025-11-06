#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <mbgl/style/layers/custom_drawable_layer_impl.hpp>
#include <mbgl/renderer/layers/render_custom_drawable_layer.hpp>
#include <mbgl/gfx/drawable.hpp>
#include <mbgl/gl/drawable_custom.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/renderer/buckets/hillshade_bucket.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/math/angles.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/renderer/layer_group.hpp>
#include <mbgl/gfx/drawable_builder.hpp>
#include <threepp/threepp.hpp>
#include <iostream>
#include <format>

namespace mbgl {

using namespace style;

namespace {

inline const CustomDrawableLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == CustomDrawableLayer::Impl::staticTypeInfo());
    return static_cast<const CustomDrawableLayer::Impl&>(*impl);
}

inline const TerrainDrawableLayer::Impl& terrain_impl(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == TerrainDrawableLayer::Impl::staticTypeInfo());
    return static_cast<const TerrainDrawableLayer::Impl&>(*impl);
}

} // namespace

RenderCustomDrawableLayer::RenderCustomDrawableLayer(Immutable<style::CustomDrawableLayer::Impl> _impl)
    : RenderLayer(makeMutable<CustomDrawableLayerProperties>(std::move(_impl))),
      host(impl(baseImpl).host) {
    assert(gfx::BackendScope::exists());
    host->initialize();
}

RenderCustomDrawableLayer::~RenderCustomDrawableLayer() {
    assert(gfx::BackendScope::exists());
    host->deinitialize();
}

void RenderCustomDrawableLayer::evaluate(const PropertyEvaluationParameters&) {
    passes = RenderPass::Pass3D;
    // It is fine to not update `evaluatedProperties`, as `baseImpl` should never be updated for this layer.
}

bool RenderCustomDrawableLayer::hasTransition() const {
    return true;
}
bool RenderCustomDrawableLayer::hasCrossfade() const {
    return true;
}

void RenderCustomDrawableLayer::prepare(const LayerPrepareParameters& params) {
}

bool RenderCustomDrawableLayer::is3D() const {
    return true;
}

void RenderCustomDrawableLayer::update(gfx::ShaderRegistry& shaders,
                                       gfx::Context& context,
                                       const TransformState& state,
                                       const std::shared_ptr<UpdateParameters>& updateParameters,
                                       const RenderTree& renderTree,
                                       UniqueChangeRequestVec& changes) {
    // check if host changed and update
    bool hostChanged = (host != impl(baseImpl).host);
    if (hostChanged) {
        // deinitialize the previous one before initializing the new one.
        if (host) {
            host->deinitialize();
        }
        host = impl(baseImpl).host;
        host->initialize();
    }
    // delegate the call to the custom layer
    if (host) {
        CustomDrawableLayerHost::Interface interface(
            *this, layerGroup, shaders, context, state, updateParameters, renderTree, changes,nullptr,nullptr);
        host->update(interface);
    }
}

RenderCustomDrawableTerrainLayer::RenderCustomDrawableTerrainLayer(Immutable<style::TerrainDrawableLayer::Impl> _impl)
    : RenderLayer(makeMutable<CustomTerrainLayerProperties>(std::move(_impl))),
      host(terrain_impl(baseImpl).host) {
    assert(gfx::BackendScope::exists());
    host->initialize();
}

RenderCustomDrawableTerrainLayer::~RenderCustomDrawableTerrainLayer() {
    assert(gfx::BackendScope::exists());
    host->deinitialize();
}

void RenderCustomDrawableTerrainLayer::update(gfx::ShaderRegistry& shaders,
                                              gfx::Context& context,
                                              const TransformState& state,
                                              const std::shared_ptr<UpdateParameters>& updateParameters,
                                              const RenderTree& renderTree,
                                              UniqueChangeRequestVec& changes) {

    bool hostChanged = (host != terrain_impl(baseImpl).host);
    if (hostChanged) {
        // deinitialize the previous one before initializing the new one.
        if (host) {
            host->deinitialize();
        }
        host = impl(baseImpl).host;
        host->initialize();
    }
    
    if (!renderTiles || renderTiles->empty()) {
        removeAllDrawables();
        return;
    }


    if (!layerGroup) {
        if (auto layerGroup_ = context.createTileLayerGroup(layerIndex, /*initialCapacity=*/64, getID())) {
            setLayerGroup(std::move(layerGroup_), changes);
        } else {
            return;
        }
    }

    auto* tileLayerGroup = static_cast<TileLayerGroup*>(layerGroup.get());
    auto renderPass = RenderPass::Translucent;

    //
    if (!tilesToRenderThisFrame)
    {
        tilesToRenderThisFrame = std::make_shared<std::vector<std::reference_wrapper<const RenderTile>>>(); 
    }
    tilesToRenderThisFrame->clear(); 
    if (host) {
        CustomDrawableLayerHost::Interface interface(*this,
                                                     layerGroup,
                                                     shaders,
                                                     context,
                                                     state,
                                                     updateParameters,
                                                     renderTree,
                                                     changes,
                                                     tilesToRenderThisFrame,
                                                     (mbgl::style::Layer::Impl*)(baseImpl.get()));
         tileLayerGroup->removeDrawablesIf([&](gfx::Drawable& drawable) { 
		     mbgl::gl::DrawableCustom* ptrDrawable_ = static_cast<mbgl::gl::DrawableCustom*>(&drawable);
             bool isRemove = drawable.getTileID() && !hasRenderTile(*drawable.getTileID()); 
             if (isRemove)
             {
                 if (u_drawable.count(drawable.getTileID().value()) != 0) {
                     u_drawable.erase(drawable.getTileID().value());
                 }
             }
             return isRemove;
         });


        for (const RenderTile& tile : *renderTiles) {
            tilesToRenderThisFrame->emplace_back(tile);
            const auto& tileID = tile.getOverscaledTileID();
            if (tileID.canonical.z < baseImpl->minZoom)
            {
                continue; 
            }
            if (!tile.getNeedsRendering())
            {
                continue; 
            }
            auto* bucket_ = tile.getBucket(*baseImpl);
            if (!bucket_ || !bucket_->hasData()) {
                removeTile(renderPass, tileID);
                continue;
            };
            auto& bucket = static_cast<HillshadeBucket&>(*bucket_);
            setRenderTileBucketID(tileID, bucket.getID());
            if (bucket.hasData()) {
                auto& demData = bucket.getDEMData(); 
                auto& image = demData.getImagePtr(); 
                mbgl::gl::DrawableCustom* ptrDrawable;
                bool createNew{true}; 
                createNew = u_drawable.count(tileID) == 0; 
                 if (createNew)
                 {
                     interface.addCustomDrawableWithTile(tileID, ptrDrawable); 
                     if (ptrDrawable)
                     {
                         std::string mesh_uuid = demData.getTerrainMesh()->uuid; 
                         std::shared_ptr<threepp::Object3D> terrain_mesh{nullptr}; 
                         if (meshID.count(mesh_uuid) == 0)
                         {
                             terrain_mesh = demData.getTerrainMesh(); 
                             ptrDrawable->getImpl()->scene->add(terrain_mesh); 
                             meshID.insert(mesh_uuid); 
                         }
                         else
                         {
                             terrain_mesh = demData.getTerrainMesh()->clone(); 
                             ptrDrawable->getImpl()->scene->add(terrain_mesh); 
                         }
                         u_drawable.insert({tileID, terrain_mesh.get()}); 
                     }
                 }
            }
        }   
        host->update(interface);
    }
}

bool RenderCustomDrawableTerrainLayer::hasRenderTileDem(const OverscaledTileID& tileID) const {
    return renderTileDemIDs.find(tileID).has_value();
}

void RenderCustomDrawableTerrainLayer::updateRenderTileDemIDs() {

    if (!renderTilesDem || renderTilesDem->empty()) {
        renderTileDemIDs.clear();
        return;
    }

    newRenderTileDemIDs.assign(renderTilesDem->begin(), renderTilesDem->end(), [&](const auto& tile) {
        const auto& tileID = tile.get().getOverscaledTileID();
        return std::make_pair(tileID, getRenderTileBucketID(tileID));
    });

    renderTileDemIDs.swap(newRenderTileDemIDs);
    newRenderTileDemIDs.clear();
}


void RenderCustomDrawableTerrainLayer::evaluate(const PropertyEvaluationParameters&) {
    passes = RenderPass::Pass3D;
}

bool RenderCustomDrawableTerrainLayer::hasTransition() const {
    return true;
}

bool RenderCustomDrawableTerrainLayer::hasCrossfade() const {
    return true;
}

void RenderCustomDrawableTerrainLayer::prepare(const LayerPrepareParameters& params) {
    renderTiles = params.source->getRenderTiles();
    auto imageData = params.source->getImageRenderData();
    // It is possible image data is not available until the source loads it.
    assert(renderTiles || imageData || !params.source->isEnabled());
    updateRenderTileIDs();
    updateRenderTileDemIDs(); 
}

bool RenderCustomDrawableTerrainLayer::is3D() const {
    return true;
}

} // namespace mbgl
