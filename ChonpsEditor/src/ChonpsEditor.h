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
		virtual void OnImGuiRender() override;

	private:
		std::string m_Name;
		bool m_UseLayer = true;

		Chonps::Camera m_Camera;
		Chonps::Shared<Chonps::Shader> m_Shader;
		Chonps::Shared<Chonps::Shader> m_LightShader;
		Chonps::Shared<Chonps::Shader> m_FrameBufferShader;
		Chonps::Shared<Chonps::Shader> m_CubemapShader;
		Chonps::Shared<Chonps::Shader> m_PlaneShader;

		Chonps::Shared<Chonps::Cubemap> m_Cubemap;

		Chonps::Shared<Chonps::FBO> m_FBO;
		Chonps::Shared<Chonps::FBO> m_MsaaFBO;
		Chonps::Shared<Chonps::FBO> m_ImguiFBO;
		Chonps::Shared<Chonps::FBO> m_ShadowFBO;


		glm::vec2 m_ViewportSize = glm::vec2(1.0f);
		glm::vec2 m_LastViewportSize = glm::vec2(1.0f);
		glm::vec2 m_ViewportMousePos = glm::vec2(0.0f);
		bool m_ViewportFocused = false;
		bool m_ViewportPlay = false;

		Chonps::Shared<Chonps::VAO> m_VAO;
		Chonps::Shared<Chonps::VAO> m_LightVAO;
		Chonps::Shared<Chonps::VAO> m_FrameBufferVAO;
		Chonps::Shared<Chonps::VAO> m_PlaneVAO;

		Chonps::Window* m_Window = &Chonps::Application::GetApp().GetWindow();

		Chonps::Shared<Chonps::Texture> m_SunTexture;

		Chonps::Shared<Scene> m_Scene;
		SceneHierarchyPanel m_SceneHierarchy;
		PropertiesPanel m_Properties;

		glm::vec3 m_lightPos = glm::vec3(0.5f, 1.5f, 1.5f);

		Chonps::RendererStatistics m_Stats;

		float m_PreviousFrame = 0.0f;
		int m_FrameCount = 0;
		int m_FPS = 0;
	};
}

#endif