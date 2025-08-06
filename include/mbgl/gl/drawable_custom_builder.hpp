#pragma once

#include <mbgl/gfx/drawable_builder.hpp>
#include <mbgl/gl/vertex_attribute_gl.hpp>

namespace mbgl {
namespace gl {

/**
    Base class for OpenGL-specific drawable builders.
 */
class DrawableCustomBuilder final : public gfx::DrawableBuilder {
public:
    DrawableCustomBuilder(std::string name_)
        : gfx::DrawableBuilder(std::move(name_)) {}
    ~DrawableCustomBuilder() override = default;

    using DrawSegment = gfx::Drawable::DrawSegment;
    std::unique_ptr<DrawSegment> createSegment(gfx::DrawMode, SegmentBase&&) override;
    void customFlush(gfx::Context& context) override; 
protected:
    gfx::UniqueDrawable createDrawable() const override;

    /// Setup the SDK-specific aspects after all the values are present
    void init() override;
};

} // namespace gl
} // namespace mbgl
