#ifndef CHONPS_ORTHOGRAPHIC_CAMERA_H
#define CHONPS_ORTHOGRAPHIC_CAMERA_H

#include "Shader.h"

#include <glm/glm.hpp>

namespace Chonps
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera() = default;
		OrthographicCamera(float left, float right, float bottom, float top);
		OrthographicCamera(int width, int height, float scale);

		void SetPosition(const glm::vec3& position) { m_Position = position; UpdateMatrix(); }
		void SetRotation(float rotation) { m_Rotation = rotation; UpdateMatrix(); }
		void SetDimensions(int width, int height, float scale) { m_Width = width < 0 ? -width : width; m_Height = height < 0 ? -height : height; m_Scale = scale; }

		const glm::vec3& GetPositon() const { return m_Position; }
		float GetRotation() const { return m_Rotation; }

		const glm::mat4& GetViewProjectionMatrix() const { return m_CameraMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		void UpdateMatrix();

	private:
		glm::mat4 m_CameraMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		int m_Width;
		int m_Height;
		float m_Scale;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};

	typedef OrthographicCamera Camera2D;
}

#endif