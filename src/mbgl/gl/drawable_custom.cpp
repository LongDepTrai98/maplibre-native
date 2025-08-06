#include <mbgl/gl/drawable_custom.hpp>
#include <mbgl/gl/texture2d.hpp>
#include <mbgl/gl/upload_pass.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/vertex_attribute_gl.hpp>
#include <mbgl/gl/vertex_buffer_resource.hpp>
#include <mbgl/shaders/segment.hpp>
#include <mbgl/shaders/gl/shader_program_gl.hpp>
#include <mbgl/util/instrumentation.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/gfx/drawable_impl.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
namespace mbgl {
namespace gl {
DrawableCustom::DrawableCustom(std::string name_) : Drawable(std::move(name_)) { draw_type = DrawableType::DrawableCustom; }
DrawableCustom::~DrawableCustom() {}
void DrawableCustom::draw(PaintParameters& parameters) const {
    auto& context = static_cast<gl::Context&>(parameters.context);


}
void DrawableCustom::setIndexData(gfx::IndexVectorBasePtr, std::vector<UniqueDrawSegment> segments) {}
void DrawableCustom::setVertices(std::vector<uint8_t>&&, std::size_t, gfx::AttributeDataType) {}
const gfx::UniformBufferArray& DrawableCustom::getUniformBuffers() const {
    return t; 
 }
gfx::UniformBufferArray& DrawableCustom::mutableUniformBuffers() {
    return t; 
    // TODO: insert return statement here
}
void DrawableCustom::setVertexAttrId(const size_t id) {}
void DrawableCustom::upload(gfx::UploadPass&) {}
void DrawableCustom::updateVertexAttributes(gfx::VertexAttributeArrayPtr,
                                            std::size_t vertexCount,
                                            gfx::DrawMode,
                                            gfx::IndexVectorBasePtr,
                                            const SegmentBase* segments,
                                            std::size_t segmentCount) {}
} // namespace gl
} // namespace mbgl