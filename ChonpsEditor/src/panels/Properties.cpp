#include "Properties.h"

#include "Core/ECS/Components.h"
#include "Core/Math.h"
#include "Imgui/ImguiWindow.h"

#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>

#define IMGUI_SPACING ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

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
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		ImGui::Begin("Components");
		
		Entity selectedEntity = m_SceneHierarchy->GetSelectedEntity();

		// Components Collapsing Headers ------------------------------------------------------------- /

		if (m_Scene->m_Registry.has_component<TransformComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("Transform"))
			{
				TransformComponent* component = &m_Scene->m_Registry.get_component<TransformComponent>(selectedEntity);

				ImGui::Indent(20);

				ImGui::Text("Position:");
				if (ImGui::IsItemClicked())
					component->position = { 0.0f, 0.0f, 0.0f };

				ImGui::DragFloat(" x##1", &component->position.x, 0.1f);
				ImGui::DragFloat(" y##1", &component->position.y, 0.1f);
				ImGui::DragFloat(" z##1", &component->position.z, 0.1f);
				ImGui::Text("Rotation:");
				if (ImGui::IsItemClicked())
					component->rotation = { 0.0f, 0.0f, 0.0f };

				ImGui::DragFloat(" x##2", &component->rotation.x, 1.0f);
				ImGui::DragFloat(" y##2", &component->rotation.y, 1.0f);
				ImGui::DragFloat(" z##2", &component->rotation.z, 1.0f);
				ImGui::Text("Scale:");
				if (ImGui::IsItemClicked())
					component->scale = { 1.0f, 1.0f, 1.0f };

				ImGui::DragFloat(" x##3", &component->scale.x, 0.1f);
				ImGui::DragFloat(" y##3", &component->scale.y, 0.1f);
				ImGui::DragFloat(" z##3", &component->scale.z, 0.1f);

				ImGui::Unindent(20);
				IMGUI_SPACING;
			}
		}

		if (m_Scene->m_Registry.has_component<MeshComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("Mesh"))
			{
				MeshComponent* component = &m_Scene->m_Registry.get_component<MeshComponent>(selectedEntity);
				
				ImGui::Indent(20);

				ImGui::Text(("Meshes: " + std::to_string(component->meshes.size())).c_str());

				ImVec2 region = ImGui::GetContentRegionAvail();

				if (ImGui::ButtonEx(m_Scene->m_EntityIDtoName[selectedEntity].c_str(), ImVec2(region.x - 20, 0)))
				{
					CHONPS_TRACE("Open File");
				}
				
				ImGui::Unindent(20);
				IMGUI_SPACING;
			}
		}

		if (m_Scene->m_Registry.has_component<CameraComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("Camera"))
			{
				CameraComponent* component = &m_Scene->m_Registry.get_component<CameraComponent>(selectedEntity);

				TransformComponent* transform = &m_Scene->m_Registry.get_component<TransformComponent>(selectedEntity);
				component->camera.position = transform->position;

				ImVec2 region = ImGui::GetContentRegionAvail();

				ImGui::Indent(20);

				if (ImGui::Button(" Primary Camera ", ImVec2(region.x - 65.0f, 0)))
				{
					m_Scene->SetPrimaryCameraEntity(selectedEntity);
				}
				Chonps::imguiTooltip("Set as Primary Camera Entity", 1.5f);

				ImGui::Separator();
				
				ImGui::Text("Orientation:");
				if (ImGui::IsItemClicked())
					component->camera.SetOrientation(glm::vec3(0.0f, 0.0f, 0.0f));

				glm::vec3* camOri = &component->camera.orientation;
				ImGui::DragFloat(" x##Cam2", &camOri->x, 0.01f);
				ImGui::DragFloat(" y##Cam2", &camOri->y, 0.01f);
				ImGui::DragFloat(" z##Cam2", &camOri->z, 0.01f);

				static int s_CurrentSelectedViewMode = 0;
				const char* camViewMode[2] = { " Perspective", " Orthographic" };
				ImGui::Text("View");
				ImGui::Combo("##CamViewMode", &s_CurrentSelectedViewMode, camViewMode, 2);

				ImGui::Text("FOV:");
				static int s_CurrentSelectedAngleType = 0;
				if (s_CurrentSelectedAngleType == 0)
					ImGui::DragFloat("##CamFOVdeg", &component->camera.FOVdeg, 1.0f, 1.0f, 120.0f, "%.3f deg");
				else
				{
					float camFOVrad = radians(component->camera.FOVdeg);
					ImGui::DragFloat("##CamFOVrad", &camFOVrad, 0.01f, 0.0174533f, 2.0944f, "%.3f rad");
					component->camera.FOVdeg = degrees(camFOVrad);
				}
				const char* angleMeasureType[2] = { " degrees", " radians" };
				ImGui::Combo("##CamAngleMeasureType", &s_CurrentSelectedAngleType, angleMeasureType, 2);

				ImGui::Text("Near/Far Plane");
				float* camNearFarPlane[2] = { &component->camera.nearPlane, &component->camera.farPlane };
				ImGui::DragFloatRange2("##CamNearFarPlane", camNearFarPlane[0], camNearFarPlane[1], 0.5f, 0.001f, 1000.0f);

				ImGui::Text("Up Vector:");
				glm::vec3* camUpVec = &component->camera.upVector;
				float* camUpVecArr[3] = { &camUpVec->x, &camUpVec->y, &camUpVec->z };
				ImGui::DragFloat3("##CamUpVec", *camUpVecArr, 0.05f, 0.0f, 1.0f);

				ImGui::Unindent(20);
				IMGUI_SPACING;
			}
		}

		if (m_Scene->m_Registry.has_component<TagComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("Tag"))
			{
				TagComponent* component = &m_Scene->m_Registry.get_component<TagComponent>(selectedEntity);

				ImGui::Indent(20);

				static std::string s_tagName;
				static bool s_InputTag = false;

				ImVec2 region = ImGui::GetContentRegionAvail();
				if (ImGui::BeginListBox("##tagListBox", ImVec2(region.x - 20, 0)))
				{
					for (auto tag : component->tags)
					{
						if (ImGui::Selectable((" - " + tag).c_str(), s_tagName == tag))
							s_tagName = tag;
					}
					
					if (!s_InputTag)
						if (ImGui::Selectable("##AddTag"))
							s_InputTag = true;
					if (s_InputTag)
					{
						char buff[256] = "";
						ImGui::SetKeyboardFocusHere();
						ImGui::SetNextItemWidth(region.x - 20);
						if (ImGui::InputText("##TagInput", buff, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll))
						{
							if (buff != nullptr && buff[0] != '\0') component->tags.insert(buff);
							s_InputTag = false;
						}
						
						if(!ImGui::IsItemHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left))) s_InputTag = false;
					}

					ImGui::EndListBox();
				}
				
				ImGui::Unindent(20);
				IMGUI_SPACING;
			}
		}

		// ------------------------------------------------------------------------------------------- /

		ImGui::End();
		ImGui::PopStyleVar();
	}
}