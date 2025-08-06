#include <mbgl/gfx/drawable_impl.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/gl/drawable_custom.hpp>
#include <memory>
#include <vector>
#include <threepp/threepp.hpp>
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
		public:
			std::unique_ptr<threepp::GLRenderer> renderer{nullptr}; 
	};
}
}