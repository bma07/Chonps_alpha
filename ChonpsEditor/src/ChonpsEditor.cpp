#include "ChonpsEditor.h"

#include <glm/gtc/type_ptr.hpp>
#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>

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

namespace Chonps
{
	ChonpsEditor::ChonpsEditor(std::string name)
		: Layer(name), m_Name(name), m_Window(Application::GetApp().GetWindow())
	{

	}

	void ChonpsEditor::OnAttach()
	{
		m_Camera = Chonps::OrthographicCamera(m_Window->GetWidth(), m_Window->GetHeight(), 0.01f);
		m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		m_Camera.SetRotation(0.0f);

		// Uniform Buffer
		m_UniformBuffer = Chonps::createUniformBuffer(0, sizeof(UniformBufferObject));
		
		// Shader
		m_Shader = Chonps::createShader("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_vert.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan_frag.spv");

		// Vao
		m_VAO = Chonps::createVertexArray();
		m_VAO->Bind();

		Chonps::VertexBuffer* vbo = Chonps::createVertexBuffer(vertices);
		Chonps::IndexBuffer* ibo = Chonps::createIndexBuffer(indices);
		m_VAO->LinkIndexBuffer(ibo);
		m_VAO->LinkVertexBuffer(vbo, 0, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)0);
		m_VAO->LinkVertexBuffer(vbo, 1, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)(3 * sizeof(float)));
		m_VAO->LinkVertexBuffer(vbo, 2, 2, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)(6 * sizeof(float)));

		m_VAO->Unbind();
		vbo->Delete();
		ibo->Delete();

		// Vao2
		m_VAO2 = Chonps::createVertexArray();
		m_VAO2->Bind();

		Chonps::VertexBuffer* vbo2 = Chonps::createVertexBuffer(vertices2);
		Chonps::IndexBuffer* ibo2 = Chonps::createIndexBuffer(indices2);
		m_VAO2->LinkIndexBuffer(ibo2);
		m_VAO2->LinkVertexBuffer(vbo2, 0, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)0);
		m_VAO2->LinkVertexBuffer(vbo2, 1, 3, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)(3 * sizeof(float)));
		m_VAO2->LinkVertexBuffer(vbo2, 2, 2, Chonps::SDT::Float, sizeof(Chonps::vertex), (void*)(6 * sizeof(float)));

		m_VAO2->Unbind();
		vbo2->Delete();
		ibo2->Delete();

		// Mesh
		m_Meshes = Chonps::loadModel("D:/Dev/Chonps/Sandbox/res/models/gltf/Flivver/Flivver.gltf");

		// Textures
		Chonps::Texture::Binding(0, 1);
		m_Texture = Chonps::createTexture("D:/Dev/Chonps/Sandbox/res/textures/lava5.png", Chonps::TexType::Diffuse, Chonps::TexFormat::Auto);
		m_Texture->TexUnit(0);
		m_Texture2 = Chonps::createTexture("D:/Dev/Chonps/Sandbox/res/textures/Checkerboard.png", Chonps::TexType::Diffuse, Chonps::TexFormat::Auto);
		m_Texture->TexUnit(0);

		Chonps::renderPrepareDraw();
	}

	void ChonpsEditor::OnDetach()
	{
		m_UniformBuffer->Delete();
		m_Shader->Delete();
		m_VAO->Delete();
		m_VAO2->Delete();
		m_Texture->Delete();
		m_Texture2->Delete();

		for (auto& meshes : m_Meshes )
			meshes.Delete();
	}

	void ChonpsEditor::OnUpdate()
	{
		float dt = Chonps::Application::GetApp().GetTimestep();

		Chonps::renderClear();
		Chonps::renderClearColor(0.08f, 0.08f, 0.08f, 1.0f);

		m_Camera.SetDimensions(m_Window->GetWidth(), m_Window->GetHeight(), 0.01f);

		//CameraMovment(window, &camera);
		m_Camera.UpdateMatrix();

		Chonps::renderBeginNextFrame();

		UniformBufferObject UBO{};


		m_Shader->Bind();
		m_VAO->Bind();
		m_Texture->Bind();

		UBO.camMatrix = m_Camera.GetViewProjectionMatrix();
		m_UniformBuffer->Bind(&UBO, sizeof(UBO), 0);
		Chonps::renderDraw(m_VAO);


		//Chonps::gui::DrawLine(0, 0, 200, 200);

		Chonps::renderDrawSubmit();
	}

	void ChonpsEditor::OnEvent(Event& e)
	{
		EventType eType = e.GetEventType();

		if (eType == Chonps::EventType::WindowMoved || eType == Chonps::EventType::WindowResize || eType == Chonps::EventType::WindowFramebufferResize)
		{
			OnUpdate();
			m_Window->OnUpdate();
		}
	}

	void ChonpsEditor::OnGuiRender()
	{
		float dt = Chonps::Application::GetApp().GetTimestep() * 1000;

		float currentFrame = Chonps::getTimeSeconds();
		m_FrameCount++;
		if (currentFrame - m_PreviousFrame >= 1.0f)
		{
			m_FPS = m_FrameCount;
			m_PreviousFrame = currentFrame;
			m_FrameCount = 0;
		}

	}
}