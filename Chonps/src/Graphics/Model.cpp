#include "cepch.h"
#include "Model.h"

#include "Core/File.h"

#include "ModelLoaders/OBJ/OBJModelLoader.h"
#include "ModelLoaders/gltf/gltfModelLoader.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Chonps
{
	Model::Model(const std::string& filepath, mt3d modelType /*= mt3d::NA*/)
	{
		size_t fileLength = filepath.length();
		size_t lastDot = filepath.rfind('.') + 1;
		auto count = fileLength - lastDot;
		m_ModelType = filepath.substr(lastDot, count);

		GetModelData(filepath, modelType);
	}

	void Model::GetModelData(const std::string& filepath, mt3d modelType)
	{
		if (modelType == mt3d::Obj || m_ModelType == "obj")
		{
			m_Meshes = loadOBJModel(filepath);
		}
		if (modelType == mt3d::Gltf || m_ModelType == "gltf")
		{
			m_Meshes = loadgltfModel(filepath);
		}
	}

	void Model::Delete()
	{
		for (auto& Mesh : m_Meshes)
			Mesh.Delete();
	}


	std::vector<Mesh> loadModel(const std::string& filepath, mt3d modelType /*= mt3d::NA*/)
	{
		std::vector<Mesh> meshes;

		size_t fileLength = filepath.length();
		size_t lastDot = filepath.rfind('.') + 1;
		auto count = fileLength - lastDot;
		std::string modelTypeName = filepath.substr(lastDot, count);

		if (modelType == mt3d::Obj || modelTypeName == "obj")
		{
			meshes = loadOBJModel(filepath);
			return meshes;
		}
		else if (modelType == mt3d::Gltf || modelTypeName == "gltf")
		{
			meshes = loadgltfModel(filepath);
			return meshes;
		}
		else CHONPS_CORE_ERROR("ERROR: MODEL: Could not load model! Model file type not supported: {0}", modelTypeName);

		return meshes;
	}

	Mesh loadModelBatched(const std::string& filepath, mt3d modelType)
	{
		size_t fileLength = filepath.length();
		size_t lastDot = filepath.rfind('.') + 1;
		auto count = fileLength - lastDot;
		std::string modelTypeName = filepath.substr(lastDot, count);

		if (modelType == mt3d::Obj || modelTypeName == "obj")
		{
			// return loadOBJModel(filepath);
		}
		else if (modelType == mt3d::Gltf || modelTypeName == "gltf")
		{
			return loadgltfModelBatched(filepath);
		}
		else CHONPS_CORE_ERROR("ERROR: MODEL: Could not load model! Model file type not supported: {0}", modelTypeName);

		return Mesh();
	}
}