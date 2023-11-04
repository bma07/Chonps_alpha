#include <Chonps.h>
#include "ChonpsEditor.h"

#ifdef CHONPS_DEBUG
	#define CHONPS_ENABLE_VALIDATION_LAYERS true
#else
	#define CHONPS_ENABLE_VALIDATION_LAYERS false
#endif

int main()
{
	Chonps::logInit();

	CHONPS_INFO("Initialized Log");

	Chonps::setWindowContext(Chonps::WindowAPI::Glfw);
	Chonps::setGraphicsContext(Chonps::GraphicsContext::Vulkan);

	Chonps::RendererBackends rendererBackends{};
	rendererBackends.enableColorBlend = true;
	rendererBackends.enableDepthTesting = true;
	rendererBackends.enableStencilTesting = false;
	rendererBackends.enableCullFace = true;
	rendererBackends.cullFaceMode = Chonps::CullFaceMode::Back;
	rendererBackends.cullFrontFace = Chonps::CullFrontFace::CounterClockwise;
	rendererBackends.topologyType = Chonps::TopologyType::Triangle;
	rendererBackends.depthOpCompare = Chonps::CompareOperation::LessOrEqual;
	rendererBackends.vks.textureBinding = 0;
	rendererBackends.vks.samplerBinding = 1;
	rendererBackends.vks.frameBufferBinding = 0;
	rendererBackends.vks.cubemapBinding = 0;
	rendererBackends.vks.enableValidationLayers = true;

	Chonps::createRendererAPI(&rendererBackends);

	Chonps::renderGammaCorrection(false);

	Chonps::textureBinding(0, 1, 0, 0);

	Chonps::WindowData winCreateInfo{};
	winCreateInfo.title = "Chonps";
	winCreateInfo.width = 1280;
	winCreateInfo.height = 800;
	winCreateInfo.minWidth = 500;
	winCreateInfo.minHeight = 300;
	winCreateInfo.maxWidth = -1;
	winCreateInfo.maxHeight = -1;
	winCreateInfo.fullscreen = false;
	winCreateInfo.resizable = true;

	Chonps::Application app(winCreateInfo);

	Chonps::ChonpsEditor chonpsEditor("ChonpsEditor");

	app.push(&chonpsEditor);

	app.Run();

	app.Terminate();

	Chonps::windowTerminateContext();
	Chonps::destroyRendererAPI();

	return 0;
}