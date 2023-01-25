#include <Chonps.h>
#include "ChonpsEditor.h"

std::vector<Chonps::vertex> vertices =
{
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}},
	{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
};

std::vector<uint32_t> indices =
{
	0, 1, 2, 2, 3, 0
};

int main()
{
	Chonps::logInit();

	CHONPS_INFO("Initialized Log");

	Chonps::setWindowContext(Chonps::WindowAPI::Glfw);
	Chonps::setRenderContext(Chonps::RenderAPI::Vulkan);

	/*Chonps::renderGammaCorrection(true);

	Chonps::Application app("Chonps", 1280, 800);

	Chonps::ChonpsEditor chonpsEditor("ChonpsEditor");

	app.add_layer(&chonpsEditor);

	app.Run();*/

	Chonps::Shared<Chonps::Window> window = Chonps::createWindow("Window", 800, 600);

	//Chonps::gui::SetCurrentWindow(&(*window));

	Chonps::renderInit();

	Chonps::Shader* shader = Chonps::createShaderRp("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_vert.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_frag.spv");
	
	Chonps::VertexArray* vao = Chonps::createVertexArrayRp();
	vao->Bind();

	Chonps::VertexBuffer* vbo = Chonps::createVertexBufferRp(vertices);
	Chonps::IndexBuffer* ibo = Chonps::createIndexBufferRp(indices);
	vao->LinkVertexBuffer(vbo, 0, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)0);
	vao->LinkVertexBuffer(vbo, 1, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)(3 * sizeof(float)));
	vao->LinkIndexBuffer(ibo);

	shader->Bind();
	vbo->Bind();
	ibo->Bind();

	while (window->WindowIsOpen())
	{
		vao->Bind();
		Chonps::renderDraw(vao);

		//Chonps::gui::DrawLine(0, 0, 200, 200);

		window->OnUpdate();
	}

	window->Delete();
	shader->Delete();
	vbo->Delete();
	ibo->Delete();
	vao->Delete();

	Chonps::windowTerminateContext();

	return 0;
}