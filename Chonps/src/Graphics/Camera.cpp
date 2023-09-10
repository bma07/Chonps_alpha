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
	{
		m_Width = width < 0 ? -width : width;
		m_Height = height < 0 ? -height : height;
	}

	Camera::Camera(int width, int height, glm::vec3 inPosition, glm::vec3 inOrientation, float inFOVdeg, float inNearPlane, float inFarPlane)
		: position(inPosition), orientation(inOrientation), FOVdeg(inFOVdeg), nearPlane(inNearPlane), farPlane(inFarPlane)
	{
		m_Width = width < 0 ? -width : width;
		m_Height = height < 0 ? -height : height;
	}

	// Updates Camera Matrix, call in loop
	void Camera::UpdateMatrix()
	{
		glm::vec3 look = position + orientation;
		m_ViewMatrix = glm::lookAt(position, position + orientation, upVector);

		if (m_Width > 0 && m_Height > 0)
			m_ProjectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)m_Width / (float)m_Height, nearPlane, farPlane);
		
		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	// Sets up Camera position and FOV
	void Camera::SetFormat(glm::vec3 inPosition, glm::vec3 inOrientation, float inFOVdeg, float inNearPlane, float inFarPlane)
	{
		position = inPosition;
		orientation = glm::normalize(inOrientation);
		SetFOV(inFOVdeg, inNearPlane, inFarPlane);
	}

	// Sets Field of View (FOV) for camera
	// NOTE: Call this function after setting the position and orientation of the camera
	void Camera::SetFOV(float inFOVdeg, float inNearPlane, float inFarPlane)
	{
		FOVdeg = inFOVdeg;
		nearPlane = inNearPlane;
		farPlane = inFarPlane;

		UpdateMatrix();
	}

	std::shared_ptr<Camera> CreateCameraSp(int width, int height)
	{
		return std::make_shared<Camera>(width, height);
	}

	Camera* CreateCamera(int width, int height)
	{
		return new Camera(width, height);
	}
}