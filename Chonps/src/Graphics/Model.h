#ifndef CHONPS_MODEL_H
#define CHONPS_MODEL_H

#include "Mesh.h"

#define CHONPS_LOCAL Chonps::MTO::Local
#define CHONPS_GLOBAL Chonps::MTO::Global

namespace Chonps
{
	enum class ModelType3D
	{
		NA = 0,
		Obj,
		Blend,
		Gltf,
	};

	typedef ModelType3D mt3d;

	enum class ModelTransformationOrientationMode
	{
		Global,
		Local
	};

	typedef ModelTransformationOrientationMode MTO;

	class Model
	{
	public:
		Model() = default;
		Model(const std::string& filepath, mt3d modelType = mt3d::NA);

		std::vector<Mesh> GetMeshes() { return m_Meshes; }
		Mesh GetMesh(uint32_t index = 0) { return m_Meshes[index]; }

		void SetPosition(glm::vec3 newPosition) { m_Position = newPosition; }
		void SetOrientation(glm::quat newOrientation) { m_Orientation = newOrientation; }
		void SetScale(glm::vec3 newScale) { m_Scale = newScale; }

		glm::vec3 GetPosition() { return m_Position; }
		glm::quat GetOrientation() { return m_Orientation; }
		glm::vec3 GetScale() { return m_Scale; }

		void Delete();

	private:
		std::vector<Mesh> m_Meshes;

		glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat m_Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);

		std::string m_ModelType;

		void GetModelData(const std::string& filepath, mt3d modelType);
	};

	std::vector<Mesh> loadModel(const std::string& filepath, mt3d modelType = mt3d::NA);
}

#endif