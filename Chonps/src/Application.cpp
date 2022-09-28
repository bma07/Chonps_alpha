#include "cepch.h"
#include "Application.h"

namespace Chonps
{
	Application* Application::s_Instance = nullptr;

	Application::Application(std::string Title, unsigned int width, unsigned int height, bool fullScreen /* = false */)
	{
		CHONPS_CORE_ASSERT(!s_Instance, "Application already exists! Cannot create more than one Application!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(createWindow(Title, width, height, fullScreen));
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		m_ImguiEditor = new ImguiEditor(&(*m_Window));
		add_overlay(m_ImguiEditor);
	}

	void Application::Run()
	{
		while (m_Running)
		{
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