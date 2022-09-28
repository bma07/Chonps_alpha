#ifndef APPLICATION_H
#define APPLICATION_H

#include"Core.h"

#include "Window.h"
#include "LayerStack.h"

#include "Events/Event.h"
#include "Events/ApplicationEvents.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"

#include "Imgui/ImguiEditor.h"

namespace Chonps
{
	class Application
	{
	public:
		Application(std::string Title, unsigned int width, unsigned int height, bool fullScreen = false);

		void Run();
		void OnEvent(Event& e);

		// Layers
		void add_layer(Layer* layer);
		void add_overlay(Layer* layer);
		void insert_layer(Layer* layer, unsigned int index);
		void remove_layer(Layer* layer);
		void delete_layer(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& GetApp() { return *s_Instance; }

	private:
		std::unique_ptr<Window> m_Window;
		LayerStack m_LayerStack;
		ImguiEditor* m_ImguiEditor;
		bool m_Running = true;

		bool OnWindowClose(WindowCloseEvent& e);


		static Application* s_Instance;
	};
}


#endif