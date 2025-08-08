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
                auto geo = loader.load("D:\\GITHUB\\IFCRender\\IFCRender\\assets\\models\\cube.stl"); 
                const auto sphereGeometry = threepp::SphereGeometry::create(300);
                const auto sphereMaterial = threepp::MeshBasicMaterial::create();
                sphereMaterial->color.setHex(0x00ff00);
                ////sphereMaterial->depthFunc = threepp::DepthFunc::LessEqual; 
                //sphereMaterial->depthTest = true;
                //sphereMaterial->depthWrite = true; 
                sphereMaterial->side = threepp::Side::Double;
                sphereMaterial->wireframe = true;
                auto sphere = threepp::Mesh::create(sphereGeometry, sphereMaterial);
                sphere->scale.set(1.0, 1.0, 0.08); 
                ////sphere->position.set(0.0f,10.0f, 0.0f);
                //geo->rotateX(-M_PI * 0.5); 
                //geo->scale(1.0, 1.0, 0.08); 
                //scene->add(sphere); 
                scene->add(sphere); 
            }
        }
        void DrawableCustom::Impl::render() 
        {
             renderer->render(*scene, *camera); 
        }
	}
}