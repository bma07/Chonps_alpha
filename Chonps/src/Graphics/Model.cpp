#include "cepch.h"
#include "Model.h"

#include "File.h"
#include "ModelLoaders/OBJ/OBJModelLoader.h"

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

	void Model::Draw(Shader* shader)
	{
		for (auto Mesh : m_Meshes)
		{
			Mesh.Draw(shader, m_ModelMatrix * m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix);
		}
	}

	void Model::GetModelData(const std::string& filepath, mt3d modelType)
	{
		if (modelType == mt3d::Obj || m_ModelType == "obj")
		{
			m_Meshes = loadOBJModel(filepath);
		}
	}

	// Translates the model by the amount given in coordinates
	void Model::Translate(float x, float y, float z)
	{
		mat4 translate = mat4(1.0f);
		translate = glm::translate(translate, vec3(x, y, z));
		m_TranslationMatrix = m_SpaceOrientationMode == MTO::Global ? translate * m_TranslationMatrix : m_TranslationMatrix * translate;
	}

	// Translates the model by the amount given
	void Model::Translate(vec3 translation)
	{
		mat4 translate = mat4(1.0f);
		translate = glm::translate(translate, translation);
		m_TranslationMatrix = m_SpaceOrientationMode == MTO::Global ? translate * m_TranslationMatrix : m_TranslationMatrix * translate;
	}

	// Rotates the model by the amount given in coordinates
	void Model::Rotate(float x, float y, float z)
	{
		glm::quat newOrientation = glm::quat(glm::radians(glm::vec3(x, y, z))) * m_Orientation;
		m_SpaceOrientationMode == MTO::Global ? m_ModelMatrix = glm::mat4_cast(newOrientation) : m_RotationMatrix = glm::mat4_cast(newOrientation);
		m_Orientation = newOrientation;
	}

	// Rotates the model by the amount given
	void Model::Rotate(vec3 rotation)
	{
		glm::quat newOrientation = glm::quat(glm::radians(rotation)) * m_Orientation;
		m_SpaceOrientationMode == MTO::Global ? m_ModelMatrix = glm::mat4_cast(newOrientation) : m_RotationMatrix = glm::mat4_cast(newOrientation);
		m_Orientation = newOrientation;
	}

	// Scales the model by the amount given in coordinates
	void Model::Scale(float x, float y, float z)
	{
		mat4 scale = mat4(1.0f);
		scale = glm::scale(scale, vec3(x, y, z));
		m_ScaleMatrix = m_SpaceOrientationMode == MTO::Global ? scale * m_ScaleMatrix : m_ScaleMatrix * scale;
	}

	// Scales the model by the amount given
	void Model::Scale(vec3 scaling)
	{
		mat4 scale = mat4(1.0f);
		scale = glm::scale(scale, scaling);
		m_ScaleMatrix = m_SpaceOrientationMode == MTO::Global ? scale * m_ScaleMatrix : m_ScaleMatrix * scale;
	}

	// Set precise position, rotation and scale in global coordinates 
	void Model::SetPosition(float x, float y, float z)
	{
		m_Position = vec3(x, y, z);
		m_TranslationMatrix = glm::translate(m_Position);
	}

	void Model::SetPosition(vec3 translation)
	{
		m_Position = translation;
		m_TranslationMatrix = glm::translate(m_Position);
	}

	void Model::SetOrientation(float x, float y, float z)
	{
		m_Orientation = quat(glm::radians(vec3(x, y, z)));
		m_RotationMatrix = glm::mat4_cast(glm::quat(glm::radians(vec3(0.0f))) * m_Orientation);
	}

	void Model::SetOrientation(vec3 rotation)
	{
		m_Orientation = quat(glm::radians(rotation));
		m_RotationMatrix = glm::mat4_cast(quat(glm::radians(vec3(0.0f))) * m_Orientation);
	}

	void Model::SetScale(float x, float y, float z)
	{
		m_Scale = vec3(x, y, z);
		m_ScaleMatrix = glm::scale(m_Scale);
	}

	void Model::SetScale(vec3 scaling)
	{
		m_Scale = scaling;
		m_ScaleMatrix = glm::scale(m_Scale);
	}

	void Model::Delete()
	{
		for (auto Mesh : m_Meshes)
			Mesh.Delete();
	}
}