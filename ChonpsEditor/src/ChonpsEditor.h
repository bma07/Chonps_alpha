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

		void CameraMovment(Chonps::Window* window, Chonps::Camera* camera, float dt);
		bool MouseScrolled(MouseScrolledEvent& e);

	private:
		// Layer Info
		std::string m_Name;
		bool m_UseLayer = true;

		Window* m_Window;

		OrthographicCamera m_Camera;
		Camera m_SceneCamera;

		UniformBuffer* m_UniformBuffer;
		UniformBuffer* m_CameraUniformBuffer;
		UniformBuffer* m_SceneCameraUniformBuffer;

		ShaderLibrary m_ShaderLibrary;

		Shader* m_Shader;
		Shader* m_FBOShader;
		Shader* m_CubemapShader;
		Shader* m_GuiShader;
		Shader* m_PreStencilShader;

		Cubemap* m_Cubemap;

		VertexArray* m_VAO;
		VertexArray* m_VAO2;
		VertexBuffer* m_VBO2;
		IndexBuffer* m_IBO2;

		std::vector<Mesh> m_Meshes;

		TextureLayout* m_TextureLayout;
		Chonps::Font m_Font;

		FrameBuffer* m_FBO;

		// Input
		int m_MouseDragRange = 5;
		bool m_FirstClickX = false, m_FirstClickY = false;
		float m_MouseClickDownX, m_MouseClickDownY;
		uint32_t m_OldViewportWidth, m_OldViewportHeight;

		// Viewport
		uint32_t m_ViewportWidth, m_ViewportHeight, m_ViewportX, m_ViewportY;

		// Entity Component System
		Chonps::Shared<Scene> m_Scene;
		SceneHierarchyPanel m_SceneHierarchy;
		PropertiesPanel m_Properties;


		// Render stats & fps
		Chonps::RendererStatistics m_Stats;
		float m_PreviousFrame = 0.0f;
		int m_FrameCount = 0;
		int m_FPS = 0;

		// Scene Camera Movement Settings
		float m_CameraSpeed = 5.0f;
		float m_CameraSpeedScrollValue = 2.0f;

		bool m_CameraFirstClick = false;
		float m_CameraSensitivity = 0.1f;
		float m_LastMouseX, m_LastMouseY;
		float m_CameraMouseMoveOffWindowOffset = 5.0f;
	};
}

#endif