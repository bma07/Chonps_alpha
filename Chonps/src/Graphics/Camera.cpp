#include "cepch.h"
#include "Camera.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "RendererAPI.h"

namespace Chonps
{
	Camera::Camera(int width, int height)
		: m_Width(width), m_Height(height) {}

	// Updates Camera Matrix, call in loop
	void Camera::UpdateMatrix()
	{
		m_ViewMatrix = glm::lookAt(position, orientation, upVector);

		if (m_Width > 0 && m_Height > 0)
			m_ProjectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)m_Width / (float)m_Height, nearPlane, farPlane);

		// Vulkan's y-coordinate is flipped which is why we flip the proj matrix
		if (getGraphicsAPI() == GraphicsAPI::Vulkan)
			m_ProjectionMatrix[1][1] *= -1;
		
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

		UpdateMatrix();
	}

	std::shared_ptr<Camera> createCameraSp(int width, int height)
	{
		return std::make_shared<Camera>(width, height);
	}

	Camera* createCamera(int width, int height)
	{
		return new Camera(width, height);
	}
}