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

		void Draw(Shader* shader, mat4 Matrix = mat4(1.0f));
		void SpaceOrientationMode(MTO mode) { m_SpaceOrientationMode = mode; }

		void Translate(float x, float y, float z);
		void Translate(vec3 translation);
		void Rotate(float x, float y, float z);
		void Rotate(vec3 rotation);
		void Scale(float x, float y, float z);
		void Scale(vec3 scaling);

		void SetPosition(float x, float y, float z);
		void SetPosition(vec3 translation);
		void SetOrientation(float x, float y, float z);
		void SetOrientation(vec3 rotation);
		void SetScale(float x, float y, float z);
		void SetScale(vec3 scaling);

		void Delete();

	private:
		std::vector<Mesh> m_Meshes;
		mat4 m_ModelMatrix = mat4(1.0f);
		mat4 m_TranslationMatrix = mat4(1.0f);
		mat4 m_RotationMatrix = mat4(1.0f);
		mat4 m_ScaleMatrix = mat4(1.0f);

		vec3 m_Position = vec3(0.0f, 0.0f, 0.0f);
		quat m_Orientation = quat(1.0f, 0.0f, 0.0f, 0.0f);
		vec3 m_Scale = vec3(1.0f, 1.0f, 1.0f);

		MTO m_SpaceOrientationMode = MTO::Global;
		std::string m_ModelType;

		void GetModelData(const std::string& filepath, mt3d modelType);
	};
}

#endif