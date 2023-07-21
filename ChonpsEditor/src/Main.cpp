#include <Chonps.h>
#include <filesystem>

#include "ChonpsEditor.h"

namespace fs = std::filesystem;


void CameraMovment(Chonps::Window* window, Chonps::Camera* camera)
{
	const float radius = 10.0f;
	float camX = std::sin(Chonps::getTimeSeconds()) * radius;
	float camZ = std::cos(Chonps::getTimeSeconds()) * radius;

	camera->position = glm::vec3(camX, camera->position.y, camZ);
}

void onEvent(Chonps::Event& e)
{
	if (e.GetEventType() == Chonps::EventType::WindowFramebufferResize || e.GetEventType() == Chonps::EventType::WindowMoved)
	{
		CHONPS_TRACE("{0}", e.GetName());
	}
}

int main()
{
	Chonps::logInit();

	CHONPS_INFO("Initialized Log");

	Chonps::setWindowContext(Chonps::WindowAPI::Glfw);
	Chonps::setRenderContext(Chonps::GraphicsAPI::Vulkan);

	Chonps::createRendererAPI();

	Chonps::renderGammaCorrection(false);
	
	Chonps::Application app("Chonps", 1280, 800);

	Chonps::ChonpsEditor chonpsEditor("ChonpsEditor");

	app.push(&chonpsEditor);

	app.Run();

	app.Terminate();

	Chonps::windowTerminateContext(); 
	Chonps::destroyRendererAPI();

	return 0;
}