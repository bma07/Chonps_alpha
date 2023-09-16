#ifndef HG_CHONPS_CAMERA_H
#define HG_CHONPS_CAMERA_H

#include "Core/Math.h"
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
		Camera(int width, int height, glm::vec3 inPosition, glm::vec3 _orientation, float inFOVdeg, float inNearPlane, float inFarPlane);

		void UpdateMatrix();

		void SetFormat(glm::vec3 inPosition, glm::vec3 _orientation, float inFOVdeg, float inNearPlane, float inFarPlane);
		void SetDimensions(int width, int height) { m_Width = width < 0 ? -width : width; m_Height = height < 0 ? -height : height; }
		void SetPosition(glm::vec3 inPosition) { position = inPosition; }
		void SetOrientation(glm::vec3 inOrientation) { orientation = glm::normalize(inOrientation); if (inOrientation == glm::vec3(0.0f)) CHONPS_CORE_LOG_WARN(Camera, "Orientation vector is set to zero, Camera has no direction vector to look at!"); }
		void SetUpVector(glm::vec3 Up) { upVector = Up; if (Up == glm::vec3(0.0f)) CHONPS_CORE_LOG_WARN(Camera, "Up vector is set to zero, Camera has no Up vector to distinguish from!"); }
		void SetFOV(float inFOVdeg, float inNearPlane, float inFarPlane);

		const glm::mat4& GetProjectionViewMatrix() const { return m_CameraMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
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
		glm::vec3 orientation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	
		float FOVdeg = 45.0f;
		float nearPlane = 0.01f;
		float farPlane = 1000.0f;
	
	private:
		glm::mat4 m_CameraMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		
		int m_Width = 0;
		int m_Height = 0;
	};

	typedef Camera PerspectiveCamera;
	typedef Camera Camera3D;

	// Optional: This function is optional for creating a Camera class dynamically
	std::shared_ptr<Camera> CreateCameraSp(int width, int height);
	Camera* CreateCamera(int width, int height);
}

#endif