#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/helper/MecatorHelper.hpp>
#include <threepp/threepp.hpp>
#include <memory>
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
                camera = std::make_unique<threepp::Camera>();
			}
            if (!scene) 
			{
                scene = std::make_unique<threepp::Scene>();
                /*const auto sphereGeometry = threepp::SphereGeometry::create(300);
                const auto sphereMaterial = threepp::MeshLambertMaterial::create();
                sphereMaterial->color = threepp::Color::darkgray;
                sphereMaterial->side = threepp::Side::Double;
                sphereMaterial->wireframe = false; 
                float scale_z = static_cast<float>(MecatorHelper::computeScaleZForLevel(16));
                auto sphere = threepp::Mesh::create(sphereGeometry, sphereMaterial);
                sphere->scale.set(1.0, 1.0, scale_z); 
                auto meter = MecatorHelper::pixelToMeter(scale_z, 150); 
                sphere->position.set(4096.0,4096.0,static_cast<float>(meter)); 
                scene->add(sphere); 
                const auto boxGeometry = threepp::BoxGeometry::create(200,200,200);
                const auto boxMaterial = threepp::MeshLambertMaterial::create();
                auto zoom = MecatorHelper::solveZoomLevelFor1MeterPerPixel(); 
                boxMaterial->color.setRGB(1, 0, 0);
                boxMaterial->side = threepp::Side::Double; 
                auto box = threepp::Mesh::create(boxGeometry, boxMaterial);
                box->scale.set(1.0, 1.0, scale_z); 
                box->position.set(0.0, 0.0, static_cast<float>(MecatorHelper::pixelToMeter(scale_z,100)));
                scene->add(box); */
                auto a_light = threepp::AmbientLight::create(0xffffee, std::optional(0.4f));
                scene->add(a_light);
                std::shared_ptr<threepp::Light> directionalLight1 = threepp::DirectionalLight::create(0xffeeff, std::optional(0.7f));
                directionalLight1->position.set(1, 1, 1);
                scene->add(directionalLight1);
                std::shared_ptr<threepp::Light> directionalLight2 = threepp::DirectionalLight::create(0xffeeff, std::optional(0.7f));
                directionalLight2->position.set(-1, 0.5, -1);
                scene->add(directionalLight2);
            }
        }
        void DrawableCustom::Impl::render() 
        {
             renderer->render(*scene, *camera); 
        }
	}
}