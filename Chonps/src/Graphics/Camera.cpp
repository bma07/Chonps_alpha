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
		m_ViewMatrix = glm::lookAt(position, position + orientation, upVector);
		m_ProjectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)m_Width / (float)m_Height, nearPlane, farPlane);

		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	// Sets up Camera position and FOV
	void Camera::SetUp(glm::vec3 _position, float _FOVdeg, float _nearPlane, float _farPlane)
	{
		position = _position;
		SetFOV(_FOVdeg, _nearPlane, _farPlane);
	}

	// Sets Field of View (FOV) for camera
	// NOTE: Call this function after setting the position and orientation of the camera
	void Camera::SetFOV(float _FOVdeg, float _nearPlane, float _farPlane)
	{
		FOVdeg = _FOVdeg;
		nearPlane = _nearPlane;
		farPlane = _farPlane;

		m_ViewMatrix = glm::lookAt(position, position + orientation, upVector);
		m_ProjectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)m_Width / (float)m_Height, nearPlane, farPlane);

		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	std::shared_ptr<Camera> createCamera(int width, int height)
	{
		return std::make_shared<Camera>(width, height);
	}

	Camera* createCameraRp(int width, int height)
	{
		return new Camera(width, height);
	}
}