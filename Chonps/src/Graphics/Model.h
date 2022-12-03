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
		Model() {}
		Model(const std::string& filepath, mt3d modelType = mt3d::NA);

		void Draw(Shader* shader, glm::mat4 Matrix = glm::mat4(1.0f));
		void AttachShader(Shader* shader);

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