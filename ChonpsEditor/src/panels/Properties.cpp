#include "Properties.h"

#include "Core/ECS/Components.h"

#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>

namespace Chonps
{
	PropertiesPanel::PropertiesPanel(const Shared<Scene>& scene, SceneHierarchyPanel* sceneHeirarchy)
	{
		SetScene(scene, sceneHeirarchy);
	}

	void PropertiesPanel::SetScene(const Shared<Scene>& scene, SceneHierarchyPanel* sceneHeirarchy)
	{
		m_Scene = scene;
		m_SceneHierarchy = sceneHeirarchy;
	}

	void PropertiesPanel::OnGuiRender()
	{
		ImGui::Begin("Properties");
		
		Entity selectedEntity = m_SceneHierarchy->GetSelectedEntity();

		if (m_Scene->m_Registry.has_component<TransformComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("Transform"))
			{

			}
		}
		if (m_Scene->m_Registry.has_component<MeshComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("Mesh"))
			{
				auto component = m_Scene->m_Registry.get_component<MeshComponent>(selectedEntity);
				ImGui::Indent(20);
				ImGui::Text(("Meshes: " + std::to_string(component.meshes.size())).c_str());
			}
		}

		ImGui::End();
	}
}