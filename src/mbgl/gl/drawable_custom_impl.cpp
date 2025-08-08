#include <mbgl/gl/drawable_custom_impl.hpp>
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
                auto geo = loader.load("C:\\Users\\vbd\\Downloads\\39-stl\\stl\\Dragon 2.5_stl.stl"); 
                const auto sphereGeometry = threepp::SphereGeometry::create(300);
                const auto sphereMaterial = threepp::MeshBasicMaterial::create();
                sphereMaterial->color = threepp::Color::darkgray;
                sphereMaterial->side = threepp::Side::Double;
                auto sphere = threepp::Mesh::create(sphereGeometry, sphereMaterial);
                sphere->scale.set(1.0, 1.0, 0.0747); 
                sphere->position.set(4096.0,4096.0,0.0f); 
                auto mesh = threepp::Mesh::create(geo,sphereMaterial);
                mesh->scale.set(1.0, 1.0, 0.0747);
                scene->add(mesh); 
            }
        }
        void DrawableCustom::Impl::render() 
        {
             renderer->render(*scene, *camera); 
        }
	}
}