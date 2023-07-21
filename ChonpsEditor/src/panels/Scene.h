#ifndef CHONPS_SCENE_H
#define CHONPS_SCENE_H

#include <Core/ECS/Registry.h>
#include <Graphics/Camera.h>
#include <Core/Application.h>

namespace Chonps
{
	class SystemOverlay : public System
	{
	public:
		void OnUpdate();
	};

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity();
		void SetEntityName(Entity entity, const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		Registry& GetRegistry() { return m_Registry; }
		const std::vector<Entity>& GetEntities() { return m_Entities; }

		void OnUpdateRuntime(Shader* shader, float dt = 1.0f);
		void OnUpdateEditor(Camera& EditorCamera, float dt = 1.0f);
		void OnViewportResize();

		Entity GetPrimaryCameraEntity() { return m_PrimaryCameraEntity; }
		void SetPrimaryCameraEntity(Entity entity) { m_PrimaryCameraEntity = entity; }

	private:
		friend class SceneHierarchyPanel;
		friend class PropertiesPanel;

		Registry m_Registry;
		std::vector<Entity> m_Entities;
		std::unordered_map<std::string, Entity> m_EntityNametoID;
		std::unordered_map<Entity, std::string> m_EntityIDtoName;
		Entity m_PrimaryCameraEntity = 0;

		std::shared_ptr<SystemOverlay> m_System;
		
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		Chonps::Window* m_Window = Chonps::Application::GetApp().GetWindow();

		// WASD movement for Camera Editor
		void CameraInputs(Camera& camera, float dt = 1.0f);

		float m_CameraSpeed = 4.0f;
		bool m_CameraFirstClick = true;
		float m_CameraSensitivity = 0.1f;
		float m_LastX = 0.0f, m_LastY = 0.0f;
	};
}

#endif