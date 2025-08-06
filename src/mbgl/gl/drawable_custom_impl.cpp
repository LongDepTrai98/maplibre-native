#include <mbgl/gl/drawable_custom_impl.hpp>
namespace mbgl
{
	namespace gl
	{
		DrawableCustom::Impl::Impl() {}
		void DrawableCustom::Impl::resetState()
		{
			if (m_renderer)
			{
				m_renderer->resetState();
                m_renderer->clearStencil(); 
			}
		}
        void DrawableCustom::Impl::createRenderer(threepp::WindowSize& w_size) 
		{
            if (!m_renderer) 
			{
                m_renderer = std::make_unique<threepp::GLRenderer>(w_size);
                m_renderer->autoClear = false; 
            }
            isCreateRender = true; 
		}
	}
}