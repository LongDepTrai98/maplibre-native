#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/helper/MecatorHelper.hpp>
#include <threepp/threepp.hpp>
#include <threepp/core/Raycaster.hpp>
#include <threepp/helpers/CameraHelper.hpp>
#include <memory>
#include <iostream>

namespace mbgl
{
	namespace gl
	{
		DrawableCustom::Impl::Impl() {}
        void DrawableCustom::Impl::createRenderer(threepp::WindowSize w_size) 
		{
            static std::unique_ptr<threepp::GLRenderer> rendererInstance;
            if (!rendererInstance) 
			{
                rendererInstance = std::make_unique<threepp::GLRenderer>(w_size);
                rendererInstance->autoClear = false; 
                rendererInstance->localClippingEnabled = true;
            }
            renderer = rendererInstance.get(); 
        }
        void DrawableCustom::Impl::render() 
        {
            renderer->render(*scene, *camera); 
        }
        void DrawableCustom::Impl::createScene() 
        {
            if (!camera) {
                camera = std::make_unique<threepp::PerspectiveCamera>();
            }
            if (!scene) {
                scene = std::make_unique<threepp::Scene>();
            }
            if (!ray) {
                ray = std::make_unique<threepp::Raycaster>();
                ray->params.lineThreshold = 0.1f;
            }
        }
        void DrawableCustom::Impl::setRayMouse(float norX, float norY) 
        {
            if (ray)
            {
                ray->setFromCamera(threepp::Vector2(norX,norY),*camera); 
                const auto intersects = ray->intersectObjects(scene->children);
                if (intersects.size() != 0) {
                    std::cout << "intersect" << std::endl;
                };
            }
        }
	}
}