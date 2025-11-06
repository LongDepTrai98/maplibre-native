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
            auto addLight_lambda = [&](threepp::Scene& scene) {
                auto a_light = threepp::AmbientLight::create(threepp::Color(0.3f, 0.3f, 0.35f), 0.5f);
                scene.add(a_light);
                scene.add(threepp::HemisphereLight::create(threepp::Color(0.9f, 0.9f, 1.0f), threepp::Color(0.5f, 0.5f, 0.5f), 0.6f));
                std::shared_ptr<threepp::Light> directionalLight1 = threepp::DirectionalLight::create(
                    0xffeeff, std::optional(0.7f));
                directionalLight1->position.set(1, 1, 1);
                scene.add(directionalLight1);
                std::shared_ptr<threepp::Light> directionalLight2 = threepp::DirectionalLight::create(
                    0xffeeff, std::optional(0.7f));
                directionalLight2->position.set(-1, 0.5, -1);
                scene.add(directionalLight2);
            }; 
            addLight_lambda(*scene); 
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