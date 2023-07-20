#include <Chonps.h>

#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

namespace fs = std::filesystem;

std::vector<Chonps::vertex> vertices = 
{
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
};

std::vector<Chonps::vertex> vertices2 =
{
	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

std::vector<uint32_t> indices = 
{
	0, 1, 2, 2, 3, 0,
};

std::vector<uint32_t> indices2 =
{
	0, 1, 2, 2, 3, 0,
};


struct UniformBufferObject
{
	glm::mat4 camMatrix;
};


void CameraMovment(Chonps::Window* window, Chonps::Camera* camera)
{
	const float radius = 10.0f;
	float camX = std::sin(Chonps::getTimeSeconds()) * radius;
	float camZ = std::cos(Chonps::getTimeSeconds()) * radius;

	camera->position = glm::vec3(camX, camera->position.y, camZ);
}


int main()
{
	Chonps::logInit();

	CHONPS_INFO("Initialized Log");

	Chonps::setWindowContext(Chonps::WindowAPI::Glfw);
	Chonps::setRenderContext(Chonps::GraphicsAPI::Vulkan);

	Chonps::createRendererAPI();

	Chonps::renderGammaCorrection(true);
	
	/*
	Chonps::Application app("Chonps", 1280, 800);

	Chonps::ChonpsEditor chonpsEditor("ChonpsEditor");

	app.add_layer(&chonpsEditor);

	app.Run();*/

	Chonps::Window* window = Chonps::createWindow("Window", 800, 600);

	Chonps::gui::SetCurrentWindow(window);

	Chonps::renderInit();

	Chonps::Texture::Binding(0, 1);
	Chonps::Texture* texture = Chonps::createTexture("D:/Dev/Chonps/Sandbox/res/textures/brick.png", Chonps::TexType::Diffuse, Chonps::TexFormat::RGBA8);
	texture->TexUnit(0);
	Chonps::Texture* texture2 = Chonps::createTexture("D:/Dev/Chonps/Sandbox/res/textures/Checkerboard.png", Chonps::TexType::Diffuse, Chonps::TexFormat::RGBA8);
	texture2->TexUnit(0);

	Chonps::UniformBuffer* ubo = Chonps::createUniformBuffer(0, sizeof(UniformBufferObject));

	Chonps::Shader* shader = Chonps::createShader("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_vert.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_frag.spv");
	Chonps::Shader* shader2 = Chonps::createShader("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_vert2.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_frag2.spv");
	Chonps::Shader* windowShader = Chonps::createShader("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkanWindow_vert.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkanWindow_frag.spv");
	
	Chonps::gui::SetShader(windowShader);

	Chonps::VertexArray* vao = Chonps::createVertexArray();
	vao->Bind();

	Chonps::VertexBuffer* vbo = Chonps::createVertexBuffer(vertices);
	Chonps::IndexBuffer* ibo = Chonps::createIndexBuffer(indices);
	vao->LinkIndexBuffer(ibo);
	vao->LinkVertexBuffer(vbo, 0, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)0);
	vao->LinkVertexBuffer(vbo, 1, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)(3 * sizeof(float)));
	vao->LinkVertexBuffer(vbo, 2, 2, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)(6 * sizeof(float)));

	vao->Unbind();
	vbo->Delete();
	ibo->Delete();

	
	Chonps::VertexArray* vao2 = Chonps::createVertexArray();
	vao2->Bind();

	Chonps::VertexBuffer* vbo2 = Chonps::createVertexBuffer(vertices2);
	Chonps::IndexBuffer* ibo2 = Chonps::createIndexBuffer(indices2);
	vao2->LinkIndexBuffer(ibo2);
	vao2->LinkVertexBuffer(vbo2, 0, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)0);
	vao2->LinkVertexBuffer(vbo2, 1, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)(3 * sizeof(float)));
	vao2->LinkVertexBuffer(vbo2, 2, 2, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)(6 * sizeof(float)));

	vao2->Unbind();
	vbo2->Delete();
	ibo2->Delete();

	std::vector<Chonps::Mesh> mesh = Chonps::loadModel("D:/Dev/Chonps/Sandbox/res/models/gltf/Flivver/Flivver.gltf");

	Chonps::Camera camera(window->GetWidth(), window->GetHeight());
	camera.SetUp(glm::vec3(2.0f, 2.0f, 2.0f), 60.0f, 0.01f, 100.0f);
	camera.SetOrientation(glm::vec3(0.0f, 0.0f, 0.0f));

	Chonps::renderPrepareDraw();

	Chonps::Timer timer;
	uint32_t frames = 0;
	float timefps = 0;

	while (window->WindowIsOpen())
	{
		timer.Begin();

		Chonps::renderClear();
		Chonps::renderClearColor(0.08f, 0.08f, 0.08f, 1.0f);

		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		camera.SetDimensions(window->GetWidth(), window->GetHeight());
		//CameraMovment(window, &camera);
		camera.UpdateMatrix();

		Chonps::renderBeginNextFrame();

		UniformBufferObject UBO{};

		glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		
		shader->Bind();
		vao->Bind();
		texture->Bind();

		UBO.camMatrix = camera.GetCameraMatrix() * model * glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
		ubo->Bind(&UBO, sizeof(UBO), 0);
		Chonps::renderDraw(vao->GetIndexCount());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		
		for (int i = 0; i < 10; i++)
		{
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, -(float)i * 2));
			UBO.camMatrix = camera.GetCameraMatrix() * model * scale * translate;
			ubo->Bind(&UBO, sizeof(UBO), 0);
			for (auto& meshes : mesh)
			{
				Chonps::renderDraw(meshes.vertexArray);
			}
		}

		shader2->Bind();
		vao2->Bind();
		texture2->Bind();
		UBO.camMatrix = camera.GetCameraMatrix() * model * glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 1.0f));
		ubo->Bind(&UBO, sizeof(UBO), 0);
		Chonps::renderDraw(vao2);

		
		//Chonps::gui::DrawLine(0, 0, 200, 200);

		Chonps::renderDrawSubmit();
		CHONPS_TRACE("{0}", timer.ElapsedMilli());

		window->OnUpdate();

		frames++;
		timefps += timer.Elapsed();

		if (timefps >= 1.0f)
		{
			//CHONPS_TRACE("{0}", frames);
			frames = 0;
			timefps = 0.0f;
		}

		timer.Reset();
	}

	window->Delete();
	shader->Delete();
	shader2->Delete();
	windowShader->Delete();
	vao->Delete();
	vao2->Delete();
	ubo->Delete();
	texture->Delete();
	texture2->Delete();

	for (auto& meshes : mesh)
		meshes.Delete();

	Chonps::windowTerminateContext(); 
	Chonps::destroyRendererAPI();

	return 0;
}