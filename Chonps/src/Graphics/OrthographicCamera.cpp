#include "cepch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RendererAPI.h"

namespace Chonps
{
	// Use this constructor for the left, right, bottom and top farthest parts of the window
	// Note: Center of window is (0,0) so left and bottom values should be negative and all values should be half the window width
	// or height to center along the origin (ex. Window of 800px by 600px should be (-400, 400, -300, 300))
	// Note: Ortho camera is scaled to the pixel width and height of the screen, recommend using single digit numbers to scale up the vertices 
	// drawn in camera matrix (ex. Window of 800px by 600px should be scaled down to (-4.0, 4.0, -3.0, 3.0))
	// Note: In other Rendering APIs such as Vulkan, the y-coords may be flipped. In this case the bottom and top params will be switched
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar)
		: m_Left(left), m_Right(right), m_Bottom(bottom), m_Top(top), m_ZNear(zNear), m_ZFar(zFar)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::UpdateMatrix()
	{
		m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_ZNear, m_ZFar);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));
		m_ViewMatrix = glm::inverse(transform);

		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}