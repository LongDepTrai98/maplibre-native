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
#include <mbgl/renderer/layer_tweaker.hpp>
namespace mbgl {
namespace gl {
    class StateGuard
    {
    public: 
        StateGuard()
        {
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fbo);
            glGetIntegerv(GL_CURRENT_PROGRAM, &program);
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
            glGetIntegerv(GL_VIEWPORT, viewport);
            glGetBooleanv(GL_DEPTH_TEST, &depthTest);
            glGetIntegerv(GL_DEPTH_FUNC, &depth_func);
            glGetBooleanv(GL_CULL_FACE, &cullFace_enabled);
            glGetIntegerv(GL_CULL_FACE_MODE, &cullface_mode);
            glGetBooleanv(GL_BLEND, &blend_enabled);
            glGetIntegerv(GL_BLEND_SRC_RGB, &srcRGB);
            glGetIntegerv(GL_BLEND_DST_RGB, &dstRGB);
            glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlpha);
            glGetIntegerv(GL_BLEND_DST_ALPHA, &dstAlpha);
            glGetBooleanv(GL_POLYGON_OFFSET_FILL, &polygonOffset_enabled);
            glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &polygonOffset_factor);
            glGetFloatv(GL_POLYGON_OFFSET_UNITS, &polygonOffset_unit);
            glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask_enabled);
            glGetBooleanv(GL_COLOR_WRITEMASK, color_mask);
            glGetIntegerv(GL_FRONT_FACE, &frontFace);
            glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &clearStencil);
            glGetIntegerv(GL_STENCIL_FUNC, &prevFunc_Stencil);
            glGetIntegerv(GL_STENCIL_REF, &prevRef_Stencil);
            glGetIntegerv(GL_STENCIL_VALUE_MASK, &prevMask_Stencil);
            glGetBooleanv(GL_STENCIL_TEST, &stencil_test_enabled);
            glGetIntegerv(GL_STENCIL_FAIL, &sfail);
            glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &dpfail);
            glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &dppass);
            glGetFloatv(GL_LINE_WIDTH, &line_width);
            glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
            glGetBooleanv(GL_SCISSOR_TEST, &scisscor_enabled);
            glGetIntegerv(GL_SCISSOR_BOX, scissorBox);
        }
        ~StateGuard() 
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glUseProgram(program);
            glBindVertexArray(vao);
            //glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
            if (depthTest)
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);
            glDepthFunc(depth_func); 
            if (cullFace_enabled)
                glEnable(GL_CULL_FACE);
            else
                glDisable(GL_CULL_FACE);
            glCullFace(cullface_mode); 
        
            if (blend_enabled)
                glEnable(GL_BLEND);
            else
                glDisable(GL_BLEND);
            glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);

            glLineWidth(line_width); 

            glDepthMask(depthMask_enabled); 
            glColorMask(color_mask[0], color_mask[1], color_mask[2], color_mask[3]); 

            glFrontFace(static_cast<GLenum>(frontFace));
        
            glClearStencil(clearStencil);
            glStencilFunc(prevFunc_Stencil, prevRef_Stencil, static_cast<GLuint>(prevMask_Stencil));
            glGetIntegerv(GL_STENCIL_WRITEMASK, &prevMask_Stencil_Write);
            glStencilMask(static_cast<GLuint>(prevMask_Stencil_Write));
            if (stencil_test_enabled)
                glEnable(GL_STENCIL_TEST);
            else
                glDisable(GL_STENCIL_TEST);

            if (scisscor_enabled)
                glEnable(GL_SCISSOR_TEST);
            else
                glDisable(GL_SCISSOR_TEST); 
            glStencilOp(sfail, dpfail, dppass);
            glScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
            if (polygonOffset_enabled) glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(polygonOffset_factor,polygonOffset_unit); 
            glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        }
    public: 
        GLint fbo = 0;
        GLint program = 0;
        GLint vao = 0;
        GLint viewport[4] = {0, 0, 0, 0};
        //depth test
        GLboolean depthTest = GL_FALSE;
        GLint depth_func = 0;
        GLboolean cullFace_enabled = GL_FALSE;
        GLint cullface_mode = 0; 
        GLboolean blend_enabled = GL_FALSE;
        GLint srcRGB;
        GLint dstRGB;
        GLint srcAlpha;
        GLint dstAlpha;
        GLboolean polygonOffset_enabled = GL_FALSE; 
        GLfloat polygonOffset_factor = 0.0f;
        GLfloat polygonOffset_unit = 0.0f; 
        GLfloat line_width = 0.0f;
        GLboolean depthMask_enabled = GL_FALSE; 
        GLboolean color_mask[4]; 
        GLint frontFace = 0; 
        GLint clearStencil; 
        GLint prevFunc_Stencil;
        GLint prevRef_Stencil;
        GLint prevMask_Stencil;
        GLint prevMask_Stencil_Write; 
        GLboolean stencil_test_enabled; 
        GLint sfail, dpfail, dppass;
        GLfloat clearColor[4] = {0.f, 0.f, 0.f, 0.f}; // RGBA
        GLboolean scisscor_enabled;
        GLint scissorBox[4];
    };
    class TextureGuard
    {
        public: 
            TextureGuard(GLenum target_)
                : target(target_)
            {
                glGetIntegerv(GL_ACTIVE_TEXTURE, &prevActiveTexture);
                activeTextureUnit = prevActiveTexture - GL_TEXTURE0;
                switch (target) {
                    case GL_TEXTURE_2D:
                        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevBinding);
                        break;
                    case GL_TEXTURE_CUBE_MAP:
                        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &prevBinding);
                        break;
                    case GL_TEXTURE_3D:
                        glGetIntegerv(GL_TEXTURE_BINDING_3D, &prevBinding);
                        break;
                    case GL_TEXTURE_2D_ARRAY:
                        glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &prevBinding);
                        break;
                    // Add other texture types if needed
                    default:
                        prevBinding = 0;
                }
            }; 
            ~TextureGuard() 
            {
                glActiveTexture(static_cast<GLenum>(prevActiveTexture));
                glBindTexture(target, static_cast<GLuint>(prevBinding));
            }; 
        public: 
            GLenum target;
            GLint prevBinding = 0;
            GLint prevActiveTexture = GL_TEXTURE0;
            GLint activeTextureUnit = 0;
    };

    DrawableCustom::DrawableCustom(std::string name_) : Drawable(std::move(name_)),impl(std::make_unique<Impl>())
    {
        draw_type = DrawableType::DrawableCustom;
        std::shared_ptr<style::CustomDrawableTweaker> tweaker = std::make_shared<style::CustomDrawableTweaker>(); 
        addTweaker(tweaker); 
        setIs3D(true); 
        setDepthType(gfx::DepthMaskType::ReadWrite);
        setRenderPass(RenderPass::Translucent); 
    }
    DrawableCustom::~DrawableCustom() {}
    void DrawableCustom::draw(PaintParameters& parameters) const {
        auto& context = static_cast<gl::Context&>(parameters.context);
        context.setDepthMode(parameters.depthModeFor3D());
        auto viewport = context.viewport.getCurrentValue().size; 
        //3d 
        StateGuard guard; 
        TextureGuard t_2d(GL_TEXTURE_2D); 
        TextureGuard t_cube(GL_TEXTURE_CUBE_MAP); 
        TextureGuard t_3d(GL_TEXTURE_3D); 
        TextureGuard t_2d_array(GL_TEXTURE_2D_ARRAY); 
        {
            threepp::WindowSize w_size(viewport.width, viewport.height);
            if (!impl->renderer) {
                impl->createRenderer(w_size);
            }
            if (tweakers.size() != 0) {
                style::CustomDrawableTweaker* tweaker = static_cast<style::CustomDrawableTweaker*>(tweakers[0].get());
                auto tile_matrix = tweaker->tile_matrix; 
                threepp::Matrix4 tile_three_matrix; 
                tile_three_matrix.fromArray(tile_matrix);
                impl->camera->projectionMatrix = tile_three_matrix; 
            }
            impl->renderer->setSize(w_size);
            impl->renderer->resetState(); 
            impl->render(); 
        }
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

    void mbgl::style::CustomDrawableTweaker::init(gfx::Drawable&) 
    {
        /*INIT for custom drawable*/
    }

    void mbgl::style::CustomDrawableTweaker::execute(gfx::Drawable& drawable, PaintParameters& parameters)
    {
        if (!drawable.getTileID().has_value()) {
            return;
        }
        const UnwrappedTileID tileID = drawable.getTileID()->toUnwrapped();
        tile_matrix = LayerTweaker::getTileMatrix(
            tileID,
            parameters, 
            {{0, 0}}, 
            style::TranslateAnchorType::Viewport,
            true,
            true, 
            drawable,
            false);
    }
