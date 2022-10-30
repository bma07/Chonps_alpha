#ifndef CHONPS_ORTHOGRAPHIC_CAMERA_H
#define CHONPS_ORTHOGRAPHIC_CAMERA_H

#include "Shader.h"

#include <glm/glm.hpp>

namespace Chonps
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		OrthographicCamera(float width, float height, float scale);

		const glm::vec3& GetPositon() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; UpdateMatrix(); }

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; UpdateMatrix(); }

		const glm::mat4& GetCameraMatrix() const { return m_CameraMatrix; }
		const glm::mat4& GetCameraProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetCameraViewMatrix() const { return m_ViewMatrix; }

		void UpdateMatrix();
		void UploadMatrix(Shader* shader, const char* uniform);

	private:
		glm::mat4 m_CameraMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};

	typedef OrthographicCamera Camera2D;
}

#endif