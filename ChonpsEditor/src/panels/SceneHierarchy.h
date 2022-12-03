#ifndef CHONPS_SCENE_HIEARCHY_H
#define CHONPS_SCENE_HIEARCHY_H

#include "Scene.h"

namespace Chonps
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Shared<Scene>& scene);

		void SetScene(const Shared<Scene>& scene);
		void OnUpdate();
		void OnGuiRender();

		Entity GetSelectedEntity() { return m_SelectedEntity; }
		void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

	private:
		Shared<Scene> m_Scene; // Note: SceneHierachy is friend class of Scene so all private membors of Scene can be accessed
		Entity m_SelectedEntity;

		void CreateEntity(const std::string& name = std::string());
	};
}

#endif