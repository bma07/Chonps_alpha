#ifndef CHONPS_COMPONENTS_H
#define CHONPS_COMPONENTS_H

#include "Core/DataTypes.h"
#include "Graphics/Mesh.h"

namespace Chonps
{
	struct TransformComponent
	{
		TransformComponent() = default;
		TransformComponent(glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale)
			: translation(translate), rotation(rotate), scale(scale) 
		{
			glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), translate);
			glm::mat4 orientationMat = glm::mat4_cast(glm::quat(glm::radians(rotate)));
			glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

			transform = transform * translationMat * orientationMat * scaleMat;
		}
		TransformComponent(glm::mat4 transformMatrix)
			: transform(transformMatrix) {}

		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		glm::mat4 transform = glm::mat4(1.0f);
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