#include "cepch.h"
#include "Model.h"

#include "File.h"

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

	void Model::Draw(Shader* shader, glm::mat4 Matrix /*= glm::mat4(1.0f)*/)
	{
		for (auto Mesh : m_Meshes)
		{
			Mesh.Draw(shader, Matrix);
		}
	}

	void Model::AttachShader(Shader* shader)
	{
		shader->Bind();
		for (auto mesh : m_Meshes)
			mesh.AttachShader(shader);
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
		for (auto Mesh : m_Meshes)
			Mesh.Delete();
	}


	std::vector<Mesh> loadModel(const std::string& filepath, mt3d modelType)
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
}