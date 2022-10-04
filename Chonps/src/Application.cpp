#include "cepch.h"
#include "Application.h"

#include "Graphics/Graphics.h"

#include <glad/glad.h>

// TEMPORARY: will remove later
#include <filesystem>
namespace fs = std::filesystem;

namespace Chonps
{
	Application* Application::s_Instance = nullptr;

	Application::Application(std::string Title, unsigned int width, unsigned int height, bool fullScreen /* = false */)
	{
		CHONPS_CORE_ASSERT(!s_Instance, "Application already exists! Cannot create more than one Application!");
		s_Instance = this;

		if (getGraphicsContext() == API::None) renderInit(API::OpenGL);

		m_Window = std::unique_ptr<Window>(createWindow(Title, width, height, fullScreen));
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		m_ImguiEditor = new ImguiEditor(&(*m_Window));
		add_overlay(m_ImguiEditor);
	}

	void Application::Run()
	{
		float vertices[] =
		{
			0.5f, -0.25f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, -0.25f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		unsigned int indices[] =
		{
			0, 1, 2
		};

		const std::string resPathDir = (fs::current_path()).string() + "/res/"; // TEMPORARY: will remove later

		Shader* shader = createShader(resPathDir + "shaders/vertex.glsl", resPathDir + "shaders/fragment.glsl");

		VAO* vao = createVAO();

		vao->Bind();
		VBO* vbo = createVBO(vertices, sizeof(vertices));
		EBO* ebo = createEBO(indices, sizeof(indices));

		vao->LinkAttributes(*vbo, 0, SDT::Float3, SDT::Float3, 6 * sizeof(float), (void*)0);
		vao->LinkAttributes(*vbo, 1, SDT::Float3, SDT::Float3, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		vao->Unbind();
		vbo->Unbind();
		ebo->Unbind();

		while (m_Running)
		{
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			shader->Bind();

			vao->Bind();
			glDrawElements(GL_TRIANGLES, sizeof(vertices) / sizeof(int), GL_UNSIGNED_INT, 0);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImguiEditor->NewFrame();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImguiEditor->Render();

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	// Layers
	void Application::add_layer(Layer* layer)
	{
		m_LayerStack.add(layer);
	}

	void Application::add_overlay(Layer* layer)
	{
		m_LayerStack.add_overlay(layer);
	}

	void Application::insert_layer(Layer* layer, unsigned int index)
	{
		m_LayerStack.insert(layer, index);
	}

	void Application::remove_layer(Layer* layer)
	{
		m_LayerStack.remove(layer);
	}

	void Application::delete_layer(Layer* layer)
	{
		m_LayerStack.delete_layer(layer);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}