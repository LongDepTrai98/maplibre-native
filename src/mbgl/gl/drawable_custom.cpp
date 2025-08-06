#include <mbgl/gl/drawable_custom.hpp>
#include <mbgl/gl/texture2d.hpp>
#include <mbgl/gl/upload_pass.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/gl/vertex_attribute_gl.hpp>
#include <mbgl/gl/vertex_buffer_resource.hpp>
#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/shaders/segment.hpp>
#include <mbgl/shaders/gl/shader_program_gl.hpp>
#include <mbgl/util/instrumentation.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/gfx/drawable_impl.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
namespace mbgl {
namespace gl {
DrawableCustom::DrawableCustom(std::string name_) : Drawable(std::move(name_)),impl(std::make_unique<Impl>())
{
    draw_type = DrawableType::DrawableCustom;
}
DrawableCustom::~DrawableCustom() {}
void DrawableCustom::draw(PaintParameters& parameters) const {
    auto& context = static_cast<gl::Context&>(parameters.context);
    auto viewport = context.viewport.getCurrentValue().size; 
    //3d 
    //  Framebuffer binding
    GLint fbo_ = 0;

    // Current shader program
    GLint program_ = 0;

    // Vertex Array Object binding
    GLint vao_ = 0;

    // Viewport dimensions (x, y, width, height)
    GLint viewport_[4] = {0, 0, 0, 0};

    // Depth test state
    GLboolean depthTest_ = GL_FALSE;

    // Blending state
    GLboolean blend_ = GL_FALSE;

    // Cull face state
    GLboolean cullFace_ = GL_FALSE;
    GLfloat clearColor_[4] = {0.f, 0.f, 0.f, 0.f}; // RGBA
    GLint srcRGB, dstRGB, srcAlpha, dstAlpha;
    glDisable(GL_STENCIL_TEST);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fbo_);
    glGetIntegerv(GL_CURRENT_PROGRAM, &program_);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao_);
    glGetIntegerv(GL_VIEWPORT, viewport_);
    glGetBooleanv(GL_DEPTH_TEST, &depthTest_);
    glGetBooleanv(GL_BLEND, &blend_);
    glGetBooleanv(GL_CULL_FACE, &cullFace_);
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor_); 
    glGetIntegerv(GL_BLEND_SRC_RGB, &srcRGB);
    glGetIntegerv(GL_BLEND_DST_RGB, &dstRGB);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &dstAlpha);
    if (depthTest_)
    {
        int a = 3; 
    }
    if (blend_)
    {
        int a = 3; 
    }
    threepp::WindowSize w_size(viewport.width,viewport.height); 
    if (!impl->isCreateRender)
    {
        impl->createRenderer(w_size); 
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glUseProgram(program_);
    glBindVertexArray(vao_);
    glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
    if (depthTest_)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    if (blend_)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    if (cullFace_)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    glClearColor(clearColor_[0], clearColor_[1], clearColor_[2], clearColor_[3]);
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}
void DrawableCustom::setIndexData(gfx::IndexVectorBasePtr, std::vector<UniqueDrawSegment> segments) {}
void DrawableCustom::setVertices(std::vector<uint8_t>&&, std::size_t, gfx::AttributeDataType) {}
const gfx::UniformBufferArray& DrawableCustom::getUniformBuffers() const {
    return tmp_array_gl; 
 }
gfx::UniformBufferArray& DrawableCustom::mutableUniformBuffers() {
     return tmp_array_gl; 
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