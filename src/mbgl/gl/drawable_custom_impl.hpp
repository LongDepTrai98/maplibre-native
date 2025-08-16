#include <mbgl/gfx/drawable_impl.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/gl/drawable_custom.hpp>
#include <threepp/canvas/WindowSize.hpp>
#include <memory>
#include <vector>
namespace threepp
{
	class GLRenderer; 
	class Scene; 
	class Camera;
    class PerspectiveCamera;
    class Raycaster; 
	class CameraHelper; 
};
namespace mbgl{
namespace gl {
	using namespace platform;
	class DrawableCustom::Impl final
	{
        public: 
			Impl(); 
			~Impl() = default;
        public: 
			void createRenderer(threepp::WindowSize& size); 
			void render();
            void setRayMouse(float norX, float norY); 
		public:
			std::unique_ptr<threepp::GLRenderer> renderer{nullptr}; 
			std::unique_ptr<threepp::Scene> scene{nullptr}; 
			std::unique_ptr<threepp::PerspectiveCamera> camera{nullptr};
            std::unique_ptr<threepp::Raycaster> ray{nullptr};  
	};
}
}