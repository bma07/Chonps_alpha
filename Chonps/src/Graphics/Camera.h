#ifndef CHONPS_CAMERA_H
#define CHONPS_CAMERA_H

#include "Core/Core.h"
#include "Core/Window.h"
#include "Shader.h"

#include "Core/DataTypes.h"

namespace Chonps
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(int width, int height);

		void UploadMatrix(Shader* shader, const char* uniform);
		void UpdateMatrix();

		void SetUp(glm::vec3 position, float FOVdeg, float nearPlane, float farPlane);
		void SetDimensions(unsigned int width, unsigned int height) { m_Width = width; m_Height = height; }

		const glm::mat4& GetCameraMatrix() const { return m_CameraMatrix; }
		const glm::mat4& GetCameraProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetCameraViewMatrix() const { return m_ViewMatrix; }

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(glm::vec3 position) { m_Position = position; }

		const glm::vec3& GetOrientation() const { return m_Orientation; }
		void SetOrientation(glm::vec3 Orientation) { m_Orientation = Orientation; }

		const glm::vec3& GetUpVector() const { return m_Up; }
		void SetUpVector(glm::vec3 Up) { m_Up = Up; }
		
		void SetFOV(float FOVdeg, float nearPlane, float farPlane);
		float GetFOVdeg() const { return m_FOVdeg; }
		float GetFOVrad() const { return m_FOVdeg * (float)(PI / 180); }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		float GetNearPlane() const { return m_NearPlane; };
		float GetFarPlane() const { return m_FarPlane; };

	private:
		glm::mat4 m_CameraMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		
		glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

		int m_Width;
		int m_Height;

		float m_FOVdeg = 45.0f;
		float m_NearPlane = 0.01f;
		float m_FarPlane = 1000.0f;
	};

	typedef Camera PerspectiveCamera;
	typedef Camera Camera3D;

	// Optional: This function is optional for creating a Camera class dynamically
	std::shared_ptr<Camera> createCamera(int width, int height);
	Camera* createCameraRp(int width, int height);
}

#endif