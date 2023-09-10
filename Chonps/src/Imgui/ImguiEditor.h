#ifndef HG_CHONPS_IMGUI_EDITOR_H
#define HG_CHONPS_IMGUI_EDITOR_H

#include "Core/Layer.h"
#include "Core/Window.h"

namespace Chonps
{
	class ImguiEditor : public Layer
	{
	public:
		ImguiEditor(Window* window);
		virtual ~ImguiEditor() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnGuiRender() override;

		void NewFrame();
		void Render();

	private:
		Window* m_Window;
	};
}


#endif