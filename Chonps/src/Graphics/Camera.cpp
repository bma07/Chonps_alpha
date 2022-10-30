#include "cepch.h"
#include "Camera.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Chonps
{
	Camera::Camera(int width, int height)
		: m_Width(width), m_Height(height) {}

	void Camera::UploadMatrix(Shader* shader, const char* uniform)
	{
		uploadUniform4mfv(shader->GetID(), uniform, 1, false, glm::value_ptr(m_CameraMatrix));
	}

	// Updates Camera Matrix, call in loop
	void Camera::UpdateMatrix()
	{
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOVdeg), (float)m_Width / (float)m_Height, m_NearPlane, m_FarPlane);

		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	// Sets up Camera position and FOV
	void Camera::SetUp(glm::vec3 position, float FOVdeg, float nearPlane, float farPlane)
	{
		m_Position = position;
		SetFOV(FOVdeg, nearPlane, farPlane);
	}

	// Sets Field of View (FOV) for camera
	// NOTE: Call this function after setting the position and orientation of the camera
	void Camera::SetFOV(float FOVdeg, float nearPlane, float farPlane)
	{
		m_FOVdeg = FOVdeg;
		m_NearPlane = nearPlane;
		m_FarPlane = farPlane;

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOVdeg), (float)m_Width / (float)m_Height, m_NearPlane, m_FarPlane);

		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	Camera* createCamera(int width, int height)
	{
		return new Camera(width, height);
	}
}