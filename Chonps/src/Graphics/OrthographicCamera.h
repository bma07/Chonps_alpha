#ifndef HG_CHONPS_ORTHOGRAPHIC_CAMERA_H
#define HG_CHONPS_ORTHOGRAPHIC_CAMERA_H

#include "Shader.h"

#include <glm/glm.hpp>

namespace Chonps
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera() = default;
		OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar);

		void SetPosition(glm::vec3 inPosition) { position = inPosition; UpdateMatrix(); }
		void SetRotation(float inRotation) { rotation = inRotation; UpdateMatrix(); }
		void SetDimensions(float left, float right, float bottom, float top, float zNear, float zFar)
		{
			m_Left = left; m_Right = right; m_Top = top; m_Bottom = bottom; m_ZNear = zNear; m_ZFar = zFar;
		}

		const glm::vec3 GetPositon() const { return position; }
		float GetRotation() const { return rotation; }

		const glm::mat4& GetProjectionViewMatrix() const { return m_CameraMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		void UpdateMatrix();

		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		float rotation = 0.0f;

	private:
		glm::mat4 m_CameraMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

		float m_Left, m_Right, m_Top, m_Bottom, m_ZNear, m_ZFar;
	};

	typedef OrthographicCamera Camera2D;
}

#endif