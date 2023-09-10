#include "cepch.h"
#include "Application.h"

#include "Graphics/Graphics.h"

#include "Timer.h"
#include "GUI/ChonpsGui.h"

namespace Chonps
{
	Application* Application::s_Instance = nullptr;

	Application::Application(uint32_t width, uint32_t height, std::string title, bool fullscreen, bool resizable, uint32_t minWidth, uint32_t minHeight)
	{
		CHONPS_CORE_ASSERT(!s_Instance, "Application already exists! Cannot create more than one Application!");
		s_Instance = this;

		if (getGraphicsAPI() == GraphicsAPI::None)
		{
			CHONPS_CORE_WARN("WARNING: APPLICATION: Graphics API was not set before hand! Automatically setting up API...");
			setRenderContext(GraphicsAPI::OpenGL);
		}

		m_Window = createWindow(width, height, title, fullscreen, resizable, minWidth, minHeight);
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		setWindowContextRenderTarget(m_Window);

		renderInit();
	}

	Application::Application(WindowData winCreateInfo)
	{
		CHONPS_CORE_ASSERT(!s_Instance, "Application already exists! Cannot create more than one Application!");
		s_Instance = this;

		if (getGraphicsAPI() == GraphicsAPI::None)
		{
			CHONPS_CORE_WARN("WARNING: APPLICATION: Graphics API was not set before hand! Automatically setting up API...");
			setRenderContext(GraphicsAPI::OpenGL);
		}

		m_Window = createWindow(winCreateInfo);
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		setWindowContextRenderTarget(m_Window);

		renderInit();
	}

	void Application::Run()
	{
		float previousFrameTime = 0.0f;

		while (m_Running)
		{
			float currentTime = getTimeSeconds();
			m_Timestep = currentTime - previousFrameTime;
			previousFrameTime = currentTime;

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			for (Layer* layer : m_LayerStack)
				layer->OnGuiRender();

			if (m_UpdateWindowRender)
			{
				m_Window->OnUpdate();
				renderClear();
			}
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

	void Application::Terminate()
	{
		m_Window->Delete();
		m_LayerStack.clear();
	}

	// Layers
	void Application::push(Layer* layer)
	{
		m_LayerStack.push(layer);
	}

	void Application::push_back(Layer* layer)
	{
		m_LayerStack.push_back(layer);
	}

	void Application::pop()
	{
		m_LayerStack.pop();
	}

	void Application::pop_back()
	{
		m_LayerStack.pop_back();
	}

	void Application::insert(Layer* layer, unsigned int index)
	{
		m_LayerStack.insert(layer, index);
	}

	void Application::remove(Layer* layer)
	{
		m_LayerStack.remove(layer);
	}

	void Application::destroy(Layer* layer)
	{
		m_LayerStack.destroy(layer);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}