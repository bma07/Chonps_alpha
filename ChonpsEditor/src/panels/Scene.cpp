#include "Scene.h"

#include <Core/ECS/Components.h>
#include <Graphics/Renderer.h>
#include <Core/Input.h>
#include <Core/MouseButtonCodes.h>
#include <Core/KeyCodes.h>

namespace Chonps
{
	static Scene* s_Scene;

	Scene::Scene()
	{
		m_Registry.register_component<TransformComponent>();
		m_Registry.register_component<MeshComponent>();

		m_System = m_Registry.register_system<SystemOverlay>();

		Signature signature;
		signature.set(m_Registry.get_component_type<TransformComponent>());
		signature.set(m_Registry.get_component_type<MeshComponent>());
		m_Registry.set_signature<SystemOverlay>(signature);

		s_Scene = this;
	}

	Scene::~Scene()
	{
		for (auto entity : m_Entities)
			m_Registry.destroy_entity(entity);
	}

	Entity Scene::CreateEntity()
	{
		Entity entity = m_Registry.create_entity();
		m_Entities.push_back(entity);
		return entity;
	}

	void Scene::SetEntityName(Entity entity, const std::string& name /*= "Entity"*/)
	{
		m_EntityIDtoName[entity] = name;
		m_EntityNametoID[name] = entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy_entity(entity);
		auto entityIndex = std::find(m_Entities.begin(), m_Entities.end(), entity);
		if (entityIndex != m_Entities.end())
			m_Entities.erase(entityIndex);
		m_EntityNametoID.erase(m_EntityIDtoName[entity]);
		m_EntityIDtoName.erase(entity);
	}

	void Scene::OnUpdateRuntime(float dt /*= 1.0f*/)
	{

	}

	void Scene::OnUpdateEditor(Camera& EditorCamera, float dt /*= 1.0f*/)
	{
		CameraInputs(EditorCamera, dt);
		m_System->OnUpdate();
	}

	void Scene::OnViewportResize()
	{
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		return Entity();
	}

	void Scene::CameraInputs(Camera& camera, float dt /*= 1.0f*/)
	{
		if (Chonps::mouseButtonPressed(m_Window, CHONPS_MOUSE_BUTTON_3))
		{
			// Handles key inputs
			if (Chonps::keyPressed(m_Window, CHONPS_KEY_W))
			{
				m_CameraPosition += (m_CameraSpeed * m_CameraOrientation) * dt;
			}
			if (Chonps::keyPressed(m_Window, CHONPS_KEY_A))
			{
				m_CameraPosition += (m_CameraSpeed * -glm::normalize(glm::cross(m_CameraOrientation, camera.GetUpVector()))) * dt;
			}
			if (Chonps::keyPressed(m_Window, CHONPS_KEY_S))
			{
				m_CameraPosition += (m_CameraSpeed * -m_CameraOrientation) * dt;
			}
			if (Chonps::keyPressed(m_Window, CHONPS_KEY_D))
			{
				m_CameraPosition += (m_CameraSpeed * glm::normalize(glm::cross(m_CameraOrientation, camera.GetUpVector()))) * dt;
			}
			if (Chonps::keyPressed(m_Window, CHONPS_KEY_SPACE))
			{
				m_CameraPosition += (m_CameraSpeed * camera.GetUpVector()) * dt;
			}
			if (Chonps::keyPressed(m_Window, CHONPS_KEY_LEFT_CONTROL))
			{
				m_CameraPosition += (m_CameraSpeed * -camera.GetUpVector()) * dt;
			}
			if (Chonps::keyPressed(m_Window, CHONPS_KEY_LEFT_SHIFT))
			{
				m_CameraSpeed = 8.0f;
			}
			else if (Chonps::keyReleased(m_Window, CHONPS_KEY_LEFT_CONTROL))
			{
				m_CameraSpeed = 4.0f;
			}

			camera.SetPosition(m_CameraPosition);

			float xpos, ypos;
			Chonps::getMousePos(m_Window, &xpos, &ypos);

			if (m_CameraFirstClick)
			{
				m_LastX = xpos;
				m_LastY = ypos;
				m_CameraFirstClick = false;
			}

			float xoffset = xpos - m_LastX;
			float yoffset = m_LastY - ypos; // reversed since y-coordinates range from bottom to top
			m_LastX = xpos;
			m_LastY = ypos;

			xoffset *= m_CameraSensitivity;
			yoffset *= m_CameraSensitivity;

			glm::vec3 newOrientation = glm::rotate(m_CameraOrientation, glm::radians(yoffset), glm::normalize(glm::cross(m_CameraOrientation, camera.GetUpVector())));
			if (abs(glm::angle(newOrientation, camera.GetUpVector()) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				m_CameraOrientation = newOrientation;
			}

			// Rotates the m_CameraOrientation left and right
			m_CameraOrientation = glm::rotate(m_CameraOrientation, glm::radians(-xoffset), camera.GetUpVector());
			camera.SetOrientation(m_CameraOrientation);

			if (xpos > m_Window->GetWidth() - 2)
			{
				Chonps::setMousePos(m_Window, 2.0f, ypos);
				m_LastX = 2.0f;
			}
			else if (xpos < 2.0f)
			{
				Chonps::setMousePos(m_Window, m_Window->GetWidth() - 2.0f, ypos);
				m_LastX = m_Window->GetWidth() - 2.0f;
			}
			if (ypos > m_Window->GetHeight() - 2)
			{
				Chonps::setMousePos(m_Window, xpos, 2.0f);
				m_LastY = 2.0f;
			}
			else if (ypos < 2.0f)
			{
				Chonps::setMousePos(m_Window, xpos, m_Window->GetHeight() - 2.0f);
				m_LastY = m_Window->GetHeight() - 2.0f;
			}
		}
		else if (Chonps::mouseButtonReleased(m_Window, CHONPS_MOUSE_BUTTON_3))
		{
			m_CameraFirstClick = true;
		}
	}


	void SystemOverlay::OnUpdate()
	{
		for (const auto& entity : Entities)
		{
			auto& meshComp = s_Scene->GetRegistry().get_component<MeshComponent>(entity);
			auto& transComp = s_Scene->GetRegistry().get_component<TransformComponent>(entity);

			for (auto mesh : meshComp.meshes)
				mesh.Draw(meshComp.shader, transComp.transform);
		}
	}
}