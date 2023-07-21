#ifndef CHONPS_APPLICATION_H
#define CHONPS_APPLICATION_H

#include "Config.h"

#include "Window.h"
#include "LayerStack.h"

#include "Events/Event.h"
#include "Events/ApplicationEvents.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"

#include "Input.h"

namespace Chonps
{
	class Application
	{
	public:
		Application(std::string Title, unsigned int width, unsigned int height, bool fullScreen = false);

		void Run();
		void OnEvent(Event& e);

		void Terminate();

		// Layers
		void push(Layer* layer);
		void push_back(Layer* layer);
		void pop();
		void pop_back();
		void insert(Layer* layer, unsigned int index);
		void remove(Layer* layer);
		void destroy(Layer* layer);

		inline static Application& GetApp() { return *s_Instance; }
		inline Window* GetWindow() { return m_Window; }
		inline float GetTimestep() { return m_Timestep; }
		inline void UpdateWindowRender(bool update) { m_UpdateWindowRender = update; }
		inline bool Running() { return m_Running; }
		inline void SetRun(bool run) { m_Running = run; }

	private:
		Window* m_Window;
		LayerStack m_LayerStack;
		float m_Timestep = 0.0f;
		bool m_Running = true;
		bool m_UpdateWindowRender = true;

		bool OnWindowClose(WindowCloseEvent& e);


		static Application* s_Instance;
	};

	static unsigned int getAppWindowWidth()
	{
		return Application::GetApp().GetWindow()->GetWidth();
	}

	static unsigned int getAppWindowHeight()
	{
		return Application::GetApp().GetWindow()->GetHeight();
	}
}


#endif