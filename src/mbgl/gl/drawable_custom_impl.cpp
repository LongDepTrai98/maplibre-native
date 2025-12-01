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
                rendererInstance->setClearColor(threepp::Color(0x000000), 0.0f); 
                //rendererInstance->toneMapping = threepp::ToneMapping::ACESFilmic; 
                //rendererInstance->toneMappingExposure = 1.0; 
                //rendererInstance->outputEncoding = threepp::Encoding::sRGB; 
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
            auto addLight_lambda = [&](threepp::Scene& scene) {
                scene.add(threepp::AmbientLight::create(0xffffff, std::optional(0.6f)));
                std::shared_ptr<threepp::Object3D> target = threepp::Object3D::create(); 
                target->position.set(4096, 10, 4096); 
                //// 2. Key Light
                auto keyLight = threepp::DirectionalLight::create(0xffffff, std::optional(0.7f));
                keyLight->position.set(1, 1, 1).multiplyScalar(4096);
                keyLight->setTarget(*target); 
                scene.add(keyLight);

                auto keyLight2 = threepp::DirectionalLight::create(0xffffff, std::optional(0.7f));
                keyLight2->position.set(-1, 1, -1).multiplyScalar(4096);
                keyLight2->setTarget(*target);
                scene.add(keyLight2);

                //// 3. Fill Light
                auto fillLight = threepp::DirectionalLight::create(0xffffff, std::optional(0.7f));
                fillLight->position.set(1, -0.5, 0.5).multiplyScalar(4096);
                fillLight->setTarget(*target); 
                scene.add(fillLight);

                // 4. Back Light (Rim)
                auto backLight = threepp::DirectionalLight::create(0xffffff, std::optional(0.7f));
                backLight->position.set(1, 1, -1).multiplyScalar(4096);
                backLight->setTarget(*target); 
                scene.add(backLight);
                scene.add(target); 
                
            }; 
            //addLight_lambda(*scene); 
        }
        void DrawableCustom::Impl::setRayMouse(float norX, float norY) 
        {
         
        }
	}
}