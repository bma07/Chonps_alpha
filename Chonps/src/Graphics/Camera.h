#ifndef CHONPS_CAMERA_H
#define CHONPS_CAMERA_H

#include "Core.h"
#include "Window.h"
#include "Shader.h"

#include "DataTypes.h"

namespace Chonps
{
	class Camera
	{
	public:
		Camera(int width, int height);

		void UploadMatrix(Shader* shader, const char* uniform);
		void UpdateMatrix();

		void SetUp(vec3 position, float FOVdeg, float nearPlane, float farPlane);
		void SetDimensions(unsigned int width, unsigned int height) { m_Width = width; m_Height = height; }

		const mat4& GetCameraMatrix() const { return m_CameraMatrix; }
		const mat4& GetCameraProjectionMatrix() const { return m_ProjectionMatrix; }
		const mat4& GetCameraViewMatrix() const { return m_ViewMatrix; }

		const vec3& GetPosition() const { return m_Position; }
		void SetPosition(vec3 position) { m_Position = position; }

		const vec3& GetOrientation() const { return m_Orientation; }
		void SetOrientation(vec3 Orientation) { m_Orientation = Orientation; }

		const vec3& GetUpVector() const { return m_Up; }
		void SetUpVector(vec3 Up) { m_Up = Up; }
		
		void SetFOV(float FOVdeg, float nearPlane, float farPlane);
		float GetFOVdeg() const { return m_FOVdeg; }
		float GetFOVrad() const { return m_FOVdeg * (float)(PI / 180); }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		float GetNearPlane() const { return m_NearPlane; };
		float GetFarPlane() const { return m_FarPlane; };

	private:
		mat4 m_CameraMatrix = mat4(1.0f);
		mat4 m_ProjectionMatrix = mat4(1.0f);
		mat4 m_ViewMatrix = mat4(1.0f);
		
		vec3 m_Position = vec3(0.0f, 0.0f, 0.0f);
		vec3 m_Orientation = vec3(0.0f, 0.0f, -1.0f);
		vec3 m_Up = vec3(0.0f, 1.0f, 0.0f);

		int m_Width;
		int m_Height;

		float m_FOVdeg = 45.0f;
		float m_NearPlane = 0.01f;
		float m_FarPlane = 1000.0f;
	};

	typedef Camera PerspectiveCamera;
	typedef Camera Camera3D;

	// NOTE: The Camera class does not need to be created dynamically since it is platform independent
	// Optional: This function is optional for creating a Camera class dynamically
	Camera* createCamera(int width, int height);
}

#endif