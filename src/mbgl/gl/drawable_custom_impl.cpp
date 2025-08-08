#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/helper/MecatorHelper.hpp>
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
                camera = std::make_unique<threepp::Camera>();
			}
            if (!scene) 
			{
                scene = std::make_unique<threepp::Scene>();
                threepp::STLLoader loader; 
                const auto sphereGeometry = threepp::SphereGeometry::create(300);
                const auto sphereMaterial = threepp::MeshBasicMaterial::create();
                sphereMaterial->color = threepp::Color::darkgray;
                sphereMaterial->side = threepp::Side::Double;
                sphereMaterial->wireframe = false; 
                float scale_z = static_cast<float>(MecatorHelper::computeScaleZForLevel(16));
                auto sphere = threepp::Mesh::create(sphereGeometry, sphereMaterial);
                sphere->scale.set(1.0, 1.0, scale_z); 
                auto meter = MecatorHelper::pixelToMeter(scale_z, 150); 
                std::cout << meter << std::endl; 
                sphere->position.set(4096.0,4096.0,static_cast<float>(meter)); 
                scene->add(sphere); 
                const auto boxGeometry = threepp::BoxGeometry::create(200,200,200);
                const auto boxMaterial = threepp::MeshBasicMaterial::create();
                boxMaterial->color.setRGB(1, 0, 0);
                boxMaterial->transparent = true;
                boxMaterial->opacity = 0.5f;
                auto box = threepp::Mesh::create(boxGeometry, boxMaterial);
                box->scale.set(1.0, 1.0, scale_z); 
                box->position.set(0.0, 0.0, static_cast<float>(MecatorHelper::pixelToMeter(scale_z,100)));
                scene->add(box); 
            }
        }
        void DrawableCustom::Impl::render() 
        {
             renderer->render(*scene, *camera); 
        }
	}
}