#ifndef CHONPS_PROPERTIES_PANEL_H
#define CHONPS_PROPERTIES_PANEL_H

#include "SceneHierarchy.h"

namespace Chonps
{
	class PropertiesPanel
	{
	public:
		PropertiesPanel() = default;
		PropertiesPanel(const Shared<Scene>& scene, SceneHierarchyPanel* sceneHeirarchy);

		void SetScene(const Shared<Scene>& scene, SceneHierarchyPanel* sceneHeirarchy);
		void OnGuiRender();

	private:
		Shared<Scene> m_Scene;
		SceneHierarchyPanel* m_SceneHierarchy;
	};
}

#endif