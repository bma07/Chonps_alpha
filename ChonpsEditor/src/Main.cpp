#include <Chonps.h>
#include "ChonpsEditor.h"

int main()
{
	Chonps::logInit();

	CHONPS_INFO("Initialized Log");

	Chonps::setWindowContext(Chonps::WindowAPI::Glfw);
	Chonps::setRenderContext(Chonps::RenderAPI::OpenGL);

	Chonps::renderGammaCorrection(true);

	Chonps::Application app("Chonps", 1280, 800);

	Chonps::ChonpsEditor chonpsEditor("ChonpsEditor");

	app.add_layer(&chonpsEditor);

	app.Run();

	Chonps::windowTerminateContext();

	return 0;
}