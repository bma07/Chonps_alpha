#ifndef CHONPS_COMPONENTS_H
#define CHONPS_COMPONENTS_H

#include "Core/DataTypes.h"
#include "Graphics/Mesh.h"

#include <set>

namespace Chonps
{
	struct TagComponent
	{
		TagComponent() = default;
		TagComponent(const std::string& tag) { tags.insert(tag); }
		TagComponent(std::vector<std::string>& tagsarr)
		{
			for (auto tag : tagsarr)
				tags.insert(tag);
		}
		TagComponent(const std::initializer_list<std::string>& tagsarr)
		{
			for (auto tag : tagsarr)
				tags.insert(tag);
		}

		std::unordered_set<std::string> tags;
	};

	struct TransformComponent
	{
		TransformComponent() = default;
		TransformComponent(glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale)
			: position(translate), rotation(rotate), scale(scale) {}

		glm::mat4 transform()
		{
			glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);
			glm::mat4 orientationMat = glm::mat4_cast(glm::quat(glm::radians(rotation)));
			glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

			return glm::mat4(1.0f) * translationMat * orientationMat * scaleMat;
		}

		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	};

	struct MeshComponent
	{
		MeshComponent() = default;
		MeshComponent(std::vector<Mesh> mesh)
			: meshes(mesh) {}
		MeshComponent(std::vector<Mesh> mesh, Shader* shader)
			: meshes(mesh), shader(shader)
		{
			for (auto& meshes : mesh)
				meshes.AttachShader(shader);
		}

		Shader* shader;
		std::vector<Mesh> meshes;
	};

	struct ShaderComponent
	{
		ShaderComponent() = default;
		ShaderComponent(Shader* shader)
			: shader(shader) {}

		Shader* shader;
	};

	struct CameraComponent
	{
		CameraComponent() = default;
		CameraComponent(glm::vec3 position, glm::vec3 orientation, float FOVdeg, float nearPlane, float farPlane)
		{
			camera.SetUp(position, FOVdeg, nearPlane, farPlane);
			camera.SetOrientation(orientation);
		}
		CameraComponent(Camera cam)
			: camera(cam) {}
		
		Camera camera;
	};

	struct UserDefinedComponent
	{
		UserDefinedComponent() {};
		virtual ~UserDefinedComponent() {};
	};
	
	// Check if a class/struct is derived from another class
	template<typename Base, typename T>
	inline bool instanceof(const T)
	{
		return std::is_base_of<Base, T>::value;
	}

	// Check is a component is an instance of UserDefinedComponent
	template <typename T>
	inline bool userComponentInstance(const T)
	{
		instanceof<UserDefinedComponent>(T);
	}
}

#endif