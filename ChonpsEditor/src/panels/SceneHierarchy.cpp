#include "SceneHierarchy.h"

#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>

#include "Core/ECS/Components.h"

namespace Chonps
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Shared<Scene>& scene)
	{
		SetScene(scene);
	}

	void SceneHierarchyPanel::SetScene(const Shared<Scene>& scene)
	{
		m_Scene = scene; // Scene is shared between Scene Hierarchy through Shared pointer
	}

	void SceneHierarchyPanel::OnUpdate()
	{
		if (m_Scene->GetEntities().size() != m_Scene->m_Entities.size())
		{
			m_Scene->m_Entities.clear();
			for (auto entity : m_Scene->GetEntities())
				m_Scene->m_Entities.push_back(entity);
		}
	}

	void SceneHierarchyPanel::OnGuiRender()
	{
		static Entity s_SelectedEntity; // entity that is selected in the scene hierarcht panel, use for changing/deleting entity
		static Entity s_RenameEntity; // Entity selected when entity is being renamed
		static uint32_t s_SelectedComponent; // component that is selected in the scene hierarcht panel, use for changing/deleting entity
		static ComponentTypeInfo compType = nullptr;

		ImGui::Begin("Scene Hierarchy");

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("SceneClicked");
			s_RenameEntity = 0;
		}
		if (ImGui::BeginPopup("SceneClicked"))
		{
			if (ImGui::BeginMenu("New Entity"))
			{
				if (ImGui::MenuItem("Mesh"))
				{
					
				}
				if (ImGui::MenuItem("Camera"))
				{
					
				}
				if (ImGui::BeginMenu("Lighting"))
				{
					if (ImGui::MenuItem("Point Light"))
					{
						
					}
					if (ImGui::MenuItem("Directional Light"))
					{
						
					}
					if (ImGui::MenuItem("Spot Light"))
					{
						
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Empty Entity"))
				{
					CreateEntity();
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			s_SelectedEntity = 0;
			s_SelectedComponent = 0;
			s_RenameEntity = 0;
		}

		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImU32 tablebgColor1 = ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImU32 tablebgColor2 = ImGui::GetColorU32(ImVec4(0.06f, 0.06f, 0.06f, 1.0f));
		ImU32 tablebgColorHovered = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImU32 tablebgColorHighlighted = ImGui::GetColorU32(ImVec4(0.2f, 0.5f, 1.0f, 1.0f));

		if (ImGui::BeginTable("SceneHierarchyPanel", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_NoClip))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

			if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
				ImGui::SameLine(contentRegionAvailable.x - 100.0f - lineHeight * 0.5f);
				ImGui::PushItemWidth(120);

				ImGuiTextFilter filter; // Filter for filtering items in search bar
				static char buff[256];
				ImGui::InputTextWithHint("##searchbar", "Search...", buff, 256);
				std::memcpy(filter.InputBuf, buff, sizeof(filter.InputBuf));
				filter.Build();

				ImGui::PopItemWidth();

				ImGui::PopStyleVar();
				
				ImGui::Spacing();

				int tablebgColor = 0; // Index for table bg color, bg color swaps for every row
				bool entityRightClicked = false;
				bool componentRightClicked = false;

				uint32_t itteration = 0; // Used to keep track each of the entity's components when they are selected

				for (auto entity : m_Scene->m_Entities)
				{
					if (filter.PassFilter(m_Scene->m_EntityIDtoName[entity].c_str())) // Items in the search bar will be filtered
					{
						ImGui::TableNextRow(0, 10);
						ImGui::TableSetColumnIndex(0);

						// Switch between two background colors for table
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColor1);

						bool entityNode = false; // Entity tree node, a tree node is created for each entity that exists
						if (s_RenameEntity != entity)
							entityNode = ImGui::TreeNodeEx((void*)(intptr_t)entity, ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick, m_Scene->m_EntityIDtoName[entity].c_str());
						else // If Entity is being renamed
						{
							static char buff[256];
							if (buff[0] == '\0') std::memcpy(buff, m_Scene->m_EntityIDtoName[s_RenameEntity].c_str(), sizeof(m_Scene->m_EntityIDtoName[s_RenameEntity]));
							ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
							ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
							ImGui::SetKeyboardFocusHere();
							if (ImGui::InputText("##rename", buff, 256, ImGuiInputTextFlags_EnterReturnsTrue))
							{
								if (m_Scene->m_EntityNametoID.find(buff) != m_Scene->m_EntityNametoID.end())
								{
									if (s_RenameEntity == m_Scene->m_EntityNametoID[buff])
										s_RenameEntity = 0;
									else
									{
										CHONPS_WARN("WARNING: Entity with the reanmed name aready exists for another entity! {0}", buff);
										ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.8f, 0.2f, 0.2f, 1.0f)));
									}
								}
								else
								{
									std::string entityName = m_Scene->m_EntityIDtoName[entity];
									m_Scene->m_EntityNametoID.erase(entityName);

									m_Scene->m_EntityIDtoName[entity] = buff;
									m_Scene->m_EntityNametoID[buff] = entity;

									s_RenameEntity = 0;
								}
							}
							std::memset(buff, 0, sizeof(buff));
							ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
							ImGui::PopStyleColor();
						}

						// Set table background if mouse hovered over item or clicked
						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColorHighlighted);
							m_SelectedEntity = entity;
							s_SelectedEntity = entity;
							s_SelectedComponent = 0;
						}
						else if (ImGui::IsItemHovered())
							ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColorHovered);
						
						
						if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
						{
							ImGui::OpenPopup("ItemClicked");
							m_SelectedEntity = entity;
							s_SelectedEntity = entity;
							s_SelectedComponent = 0;
							entityRightClicked = true;
						}

						if (entity == s_SelectedEntity)
							ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColorHighlighted);

						if (entityNode) // Open tree node for specific entity
						{
							ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing()); // Indent 

							for (auto& entityComponents : m_Scene->m_Registry.get_all_components())
							{
								if (m_Scene->m_Registry.has_component(entity, entityComponents->type()))
								{
									ImGui::TableNextRow(0, 10);
									ImGui::TableSetColumnIndex(0);
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColor2);

									ImGui::Selectable(entityComponents->name());
									itteration++;

									// Set table background if mouse hovered over item or clicked
									if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
									{
										ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColorHighlighted);
										s_SelectedComponent = itteration;
										s_SelectedEntity = 0;
									}
									else if (ImGui::IsItemHovered())
										ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColorHovered);


									if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
									{
										ImGui::OpenPopup("ComponentClicked");
										s_SelectedComponent = itteration;
										s_SelectedEntity = 0;
										componentRightClicked = true;
										compType = entityComponents->type();
									}

									if (s_SelectedComponent == itteration)
										ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColorHighlighted);
								}
							}

							ImGui::TableNextRow(0, 10);
							ImGui::TableSetColumnIndex(0);
							ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColor2);

							ImGui::Selectable(("ID: " + std::to_string(entity)).c_str());
							itteration++;

							// Set table background if mouse hovered over item or clicked
							if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
							{
								ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColorHighlighted);
								s_SelectedComponent = itteration;
								s_SelectedEntity = 0;
							}
							else if (ImGui::IsItemHovered())
								ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColorHovered);

							if (s_SelectedComponent == itteration)
								ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, tablebgColorHighlighted);

							ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing()); // Indent back

							ImGui::TreePop();
						}
					}
				}

				ImGui::PopStyleColor(2); // Revert to standard colors

				if (entityRightClicked) ImGui::OpenPopup("ItemClicked"); // This is here due to pop-up not oppening when the entity's tree node is open
				if (ImGui::BeginPopup("ItemClicked")) // Open popup when entity right-slicked
				{
					if (ImGui::BeginMenu("Add Component"))
					{
						if (ImGui::MenuItem("Transform"))
						{
							if (!m_Scene->m_Registry.has_component<TransformComponent>(m_SelectedEntity))
								m_Scene->m_Registry.add_component<TransformComponent>(m_SelectedEntity, TransformComponent());
							else CHONPS_WARN("WARNING: ECS: Entity already has component of this type: {0}", typeid(TransformComponent).name());
						}
						if (ImGui::MenuItem("Mesh"))
						{
							if (!m_Scene->m_Registry.has_component<MeshComponent>(m_SelectedEntity))
								m_Scene->m_Registry.add_component<MeshComponent>(m_SelectedEntity, MeshComponent());
							else CHONPS_WARN("WARNING: ECS: Entity already has component of this type: {0}", typeid(MeshComponent).name());
						}

						ImGui::EndMenu();
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Rename"))
					{
						s_RenameEntity = m_SelectedEntity;
						s_SelectedEntity = 0;
					}

					if (ImGui::MenuItem("Delete"))
					{
						m_Scene->DestroyEntity(m_SelectedEntity);
					}

					ImGui::EndPopup();
				}

				if (componentRightClicked)
					ImGui::OpenPopup("ComponentClicked");
				if (ImGui::BeginPopup("ComponentClicked"))
				{
					if (ImGui::MenuItem("Delete"))
					{
						m_Scene->m_Registry.remove_component(m_SelectedEntity, compType);
					}

					ImGui::EndPopup();
				}

				ImGui::TreePop();
			}
			else ImGui::PopStyleColor(2); // Pop colors if scene menu isn't open

			ImGui::PopStyleVar();

			ImGui::EndTable();
		}
		else ImGui::PopStyleColor(2); // Pop colors if window isn't open
		
		ImGui::End();
	}

	void SceneHierarchyPanel::CreateEntity(const std::string& name)
	{
		if (name.empty())
		{
			Entity entity = m_Scene->CreateEntity();
			std::string entityName = "Entity";
			if (m_Scene->m_Entities.size() < 10)
				entityName += ("00" + std::to_string(entity));
			else if (m_Scene->m_Entities.size() < 100)
				entityName += ("0" + std::to_string(entity));
			else
				entityName += (std::to_string(entity));

			m_Scene->SetEntityName(entity, entityName);
		}
		else
		{
			if (m_Scene->m_EntityNametoID.find(name) == m_Scene->m_EntityNametoID.end())
			{
				Entity entity = m_Scene->CreateEntity();
				m_Scene->SetEntityName(entity, name);

				m_Scene->m_EntityIDtoName[entity] = name;
				m_Scene->m_EntityNametoID[name] = entity;
			}
			else
			{
				CHONPS_WARN("WARNING: SCENE: Another entity with the given name exists: {0}", name);
				std::string entityName = name;
				uint32_t copy = 0;
				while (m_Scene->m_EntityNametoID.find(entityName) != m_Scene->m_EntityNametoID.end())
				{
					entityName = name + std::to_string(copy++);
				}
				Entity entity = m_Scene->CreateEntity();
				m_Scene->SetEntityName(entity, entityName);

				m_Scene->m_EntityIDtoName[entity] = entityName;
				m_Scene->m_EntityNametoID[entityName] = entity;
			}
		}
	}
}