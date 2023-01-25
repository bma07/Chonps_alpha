#ifndef CHONPS_CAMERA_H
#define CHONPS_CAMERA_H

#include "Core/Config.h"
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

		void SetUp(glm::vec3 _position, float _FOVdeg, float _nearPlane, float _farPlane);
		void SetDimensions(unsigned int width, unsigned int height) { m_Width = width; m_Height = height; }
		void SetPosition(glm::vec3 _Position) { position = _Position; }
		void SetOrientation(glm::vec3 _Orientation) { orientation = _Orientation; }
		void SetUpVector(glm::vec3 Up) { upVector = Up; }
		void SetFOV(float _FOVdeg, float _nearPlane, float _farPlane);

		const glm::mat4& GetCameraMatrix() const { return m_CameraMatrix; }
		const glm::mat4& GetCameraProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetCameraViewMatrix() const { return m_ViewMatrix; }
		const glm::vec3& GetPosition() const { return position; }
		const glm::vec3& GetOrientation() const { return orientation; }
		const glm::vec3& GetUpVector() const { return upVector; }
		float GetFOVdeg() const { return FOVdeg; }
		float GetFOVrad() const { return FOVdeg * (float)(CHONPS_PI / 180); }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		float GetNearPlane() const { return nearPlane; };
		float GetFarPlane() const { return farPlane; };

		// These variables are public and can be changed directly
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	
		float FOVdeg = 45.0f;
		float nearPlane = 0.01f;
		float farPlane = 1000.0f;
	
	private:
		glm::mat4 m_CameraMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		
		int m_Width;
		int m_Height;
	};

	typedef Camera PerspectiveCamera;
	typedef Camera Camera3D;

	// Optional: This function is optional for creating a Camera class dynamically
	std::shared_ptr<Camera> createCamera(int width, int height);
	Camera* createCameraRp(int width, int height);
}

#endif