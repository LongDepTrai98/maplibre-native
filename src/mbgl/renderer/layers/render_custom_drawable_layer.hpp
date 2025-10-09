#pragma once

#include <mbgl/style/layers/custom_drawable_layer_impl.hpp>
#include <mbgl/renderer/render_layer.hpp>
namespace threepp
{
class Object3D; 
}
namespace mbgl {

class RenderCustomDrawableLayer final : public RenderLayer {
public:
    explicit RenderCustomDrawableLayer(Immutable<style::CustomDrawableLayer::Impl>);
    ~RenderCustomDrawableLayer() override;

    /// Generate any changes needed by the layer
    void update(gfx::ShaderRegistry&,
                gfx::Context&,
                const TransformState&,
                const std::shared_ptr<UpdateParameters>&,
                const RenderTree&,
                UniqueChangeRequestVec&) override;

private:
    void transition(const TransitionParameters&) override {}
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void prepare(const LayerPrepareParameters&) override;
    bool is3D() const override;
    std::shared_ptr<style::CustomDrawableLayerHost> host;
};

class RenderCustomDrawableTerrainLayer final : public RenderLayer {
public:
    explicit RenderCustomDrawableTerrainLayer(Immutable<style::TerrainDrawableLayer::Impl>);
    ~RenderCustomDrawableTerrainLayer() override;

    /// Generate any changes needed by the layer
    void update(gfx::ShaderRegistry&,
                gfx::Context&,
                const TransformState&,
                const std::shared_ptr<UpdateParameters>&,
                const RenderTree&,
                UniqueChangeRequestVec&) override;

private:
    bool hasRenderTileDem(const OverscaledTileID& tileID) const; 
    void updateRenderTileDemIDs(); 
    void transition(const TransitionParameters&) override {}
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void prepare(const LayerPrepareParameters&) override;
    bool is3D() const override;
    std::shared_ptr<style::CustomDrawableLayerHost> host;
    RenderTiles renderTilesDem;
    RenderTileIDMap renderTileDemIDs;
    RenderTileIDMap newRenderTileDemIDs;
    std::shared_ptr<std::vector<std::reference_wrapper<const RenderTile>>> tilesToRenderThisFrame{nullptr}; 
    std::unordered_map<OverscaledTileID, threepp::Object3D*> u_drawable{};
    std::set<std::string> meshID{};
};


} // namespace mbgl
