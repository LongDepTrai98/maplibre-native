#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/helper/MecatorHelper.hpp>
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
                float scale_z = static_cast<float>(MecatorHelper::computeScaleZForLevel(16));
                auto sphere = threepp::Mesh::create(sphereGeometry, sphereMaterial);
                sphere->scale.set(1.0, 1.0, scale_z); 
                /*X,Y 0 -> 8192, Z*/
                sphere->position.set(4096.0,4096.0,150.0f); 
                scene->add(sphere); 
            }
        }
        void DrawableCustom::Impl::render() 
        {
             renderer->render(*scene, *camera); 
        }
	}
}