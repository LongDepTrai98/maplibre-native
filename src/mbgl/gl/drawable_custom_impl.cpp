#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/helper/MecatorHelper.hpp>
#include <threepp/threepp.hpp>
#include <threepp/core/Raycaster.hpp>
#include <memory>
#include <iostream>
namespace mbgl
{
	namespace gl
	{
		DrawableCustom::Impl::Impl() {}
        void DrawableCustom::Impl::createRenderer(threepp::WindowSize& w_size) 
		{
            if (!renderer) 
			{
                renderer = std::make_unique<threepp::GLRenderer>(w_size);
                renderer->autoClear = false; 
            }
			if (!camera)
			{
                camera = std::make_unique<threepp::PerspectiveCamera>();
			}
            if (!scene) 
			{
                scene = std::make_unique<threepp::Scene>();
            }
            if (!ray)
            {
                ray = std::make_unique<threepp::Raycaster>(); 
                ray->params.lineThreshold = 0.1f;
            }

        }
        void DrawableCustom::Impl::render() 
        {
            const auto intersects = ray->intersectObjects(scene->children);
            if (intersects.size() != 0) {
                std::cout << "intersect" << std::endl;
            };
             renderer->render(*scene, *camera); 
        }
        void DrawableCustom::Impl::setRayMouse(float norX, float norY) 
        {
            if (ray)
            {
                ray->setFromCamera(threepp::Vector2(norX,norY),*camera); 
            }
        }
	}
}