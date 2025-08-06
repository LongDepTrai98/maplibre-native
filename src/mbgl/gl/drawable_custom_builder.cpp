#include <mbgl/gl/drawable_custom_builder.hpp>
#include <mbgl/gfx/drawable_builder_impl.hpp>
#include <mbgl/gfx/drawable_impl.hpp>
#include <mbgl/gl/drawable_custom.hpp>
#include <mbgl/util/convert.hpp>
#include <cstring>
namespace mbgl
{
	namespace gl 
	{
    std::unique_ptr<gfx::Drawable::DrawSegment> DrawableCustomBuilder::createSegment(gfx::DrawMode drawMode,
                                                                                     SegmentBase&& seg) {
            return nullptr;
    }
    void DrawableCustomBuilder::customFlush(gfx::Context& context) 
    {
        drawables.emplace_back(std::move(currentDrawable)); 
    }
    gfx::UniqueDrawable DrawableCustomBuilder::createDrawable() const {
        return std::make_unique<DrawableCustom>(drawableName.empty() ? name : drawableName);
    }
    void DrawableCustomBuilder::init() {}

    } // namespace gl
}