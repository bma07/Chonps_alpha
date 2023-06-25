#include "cepch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RendererAPI.h"

namespace Chonps
{
	// Use this constructor for the left, right, bottom and top farthest parts of the window
	// NOTE: Center of window is (0,0) so left and bottom values should be negative and all values should be half the window width
	// or height to center along the origin (ex. Window of 800px by 600px should be (-400, 400, -300, 300))
	// NOTE: Ortho camera is scaled to the pixel width and height of the screen, recommend using single digit numbers to scale up the vertices 
	// drawn in camera matrix (ex. Window of 800px by 600px should be scaled down to (-4.0, 4.0, -3.0, 3.0))
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	// Use this constructor for the width and height of the window with a given scale
	// NOTE: scale of 1.0 will be the pixel width and height of the window
	// Vertices may appear extremely small, try using scale value 0.01 or similar
	OrthographicCamera::OrthographicCamera(float width, float height, float scale)
		: m_ViewMatrix(1.0f)
	{
		float halfWidth = width / 2;
		float halfHeight = height / 2;

		m_ProjectionMatrix = glm::mat4(glm::ortho(-halfWidth * scale, halfWidth * scale, -halfHeight * scale, halfHeight * scale, -1.0f, 1.0f));
		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::UpdateMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));
		m_ViewMatrix = glm::inverse(transform);
		m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::UploadMatrix(Shader* shader, const char* uniform)
	{
		oglSpec::uploadUniform4mfv(shader->GetID(), uniform, 1, false, glm::value_ptr(m_CameraMatrix));
	}
}