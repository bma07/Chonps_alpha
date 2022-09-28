#ifndef IMGUI_EDITOR_H
#define IMGUI_EDITOR_H

#include "Layer.h"
#include "Window.h"

namespace Chonps
{
	class ImguiEditor : public Layer
	{
	public:
		ImguiEditor(Window* window);
		virtual ~ImguiEditor() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void NewFrame();
		void Render();

	private:
		Window* m_Window;
	};
}


#endif