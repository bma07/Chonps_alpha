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
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
		m_Width = std::abs(left) + std::abs(right);
		m_Height = std::abs(bottom) + std::abs(top);
		m_Scale = 1.0f;
	}

	// Use this constructor for the width and height of the window with a given scale
	// NOTE: scale of 1.0 will be the pixel width and height of the window
	// Vertices may appear extremely small, try using scale value 0.01 or similar
	OrthographicCamera::OrthographicCamera(int width, int height, float scale)
		: m_ViewMatrix(1.0f), m_Width(width), m_Height(height), m_Scale(scale)
	{
		float halfWidth = width / 2;
		float halfHeight = height / 2;

		m_ProjectionMatrix = glm::mat4(glm::ortho(-halfWidth * scale, halfWidth * scale, -halfHeight * scale, halfHeight * scale, -1.0f, 1.0f));
		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::UpdateMatrix()
	{
		m_ProjectionMatrix = glm::mat4(glm::ortho(-m_Width * 0.5f * m_Scale, m_Width * 0.5f * m_Scale, -m_Height * 0.5f * m_Scale, m_Height * 0.5f * m_Scale, -1.0f, 1.0f));
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));
		m_ViewMatrix = glm::inverse(transform);

		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}