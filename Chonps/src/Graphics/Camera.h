#ifndef CAMERA_H
#define CAMERA_H

#include "Window.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Chonps
{
	class Camera
	{
	public:
		Camera(int width, int height, glm::vec3 position);

		glm::vec3 Position;
		glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

		int Width;
		int Height;
		float FOVdeg = 45.0f;
		float speed = 0.1f;

		bool firstClick = true;
		float sensitivity = 100.0f;

		void Matrix(Shader* shader, const char* uniform);
		void updateMatrix(float FOVdeg, float nearPlane, float farPlane, float width, float height);
		void Inputs(Window* window);

	public:
		glm::mat4 m_CameraMatrix = glm::mat4(1.0f);
	};
}

#endif