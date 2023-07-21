#ifndef CHONPS_EDITOR_H
#define CHONPS_EDITOR_H

#include <Chonps.h>
#include "panels/Scene.h"
#include "panels/SceneHierarchy.h"
#include "panels/Properties.h"

namespace Chonps
{
	class ChonpsEditor : public Layer
	{
	public:
		ChonpsEditor(std::string name);

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& e) override;
		virtual void OnGuiRender() override;

	private:
		// Layer Info
		std::string m_Name;
		bool m_UseLayer = true;

		Window* m_Window;

		OrthographicCamera m_Camera;

		UniformBuffer* m_UniformBuffer;

		Shader* m_Shader;
		Shader* m_Shader2;

		VertexArray* m_VAO;
		VertexArray* m_VAO2;

		std::vector<Mesh> m_Meshes;

		Texture* m_Texture;
		Texture* m_Texture2;

		// Entity Component System
		Chonps::Shared<Scene> m_Scene;
		SceneHierarchyPanel m_SceneHierarchy;
		PropertiesPanel m_Properties;


		// Render stats & fps
		Chonps::RendererStatistics m_Stats;
		float m_PreviousFrame = 0.0f;
		int m_FrameCount = 0;
		int m_FPS = 0;
	};
}

#endif