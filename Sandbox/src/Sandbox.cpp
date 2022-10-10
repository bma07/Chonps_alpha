#include <iostream>
#include <Chonps.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <Imgui/imgui.h>

#include <filesystem>
namespace fs = std::filesystem;

#include <GLFW/glfw3.h>

class Layer1 : public Chonps::Layer
{
public:
	Layer1(std::string name)
		: m_Name(name), m_Camera(Chonps::getAppWindowWidth(), Chonps::getAppWindowHeight())
	{
		float vertices[] =
		{
			// COORDINATES		/		COLOR			/	TEX COORD
			1.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
			1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		};

		uint32_t indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		const std::string resPathDir = (fs::current_path()).string() + "/res/"; // TEMPORARY: will remove later

		m_Shader = std::unique_ptr<Chonps::Shader>(Chonps::createShader(resPathDir + "shaders/shader.glsl"));

		m_Camera.SetUp(glm::vec3(0.0f, 0.0f, 5.0f), 45.0f, 0.01f, 1000.0f);

		m_VAO = std::unique_ptr<Chonps::VAO>(Chonps::createVAO());

		m_VAO->Bind();
		Chonps::VBO* vbo = Chonps::createVBO(vertices, sizeof(vertices));
		Chonps::EBO* ebo = Chonps::createEBO(indices, sizeof(indices));

		m_VAO->LinkEBO(ebo);
		m_VAO->LinkVBO(vbo, 0, Chonps::SDT::Float3, Chonps::SDT::Float3, 8 * sizeof(float), (void*)0);
		m_VAO->LinkVBO(vbo, 1, Chonps::SDT::Float3, Chonps::SDT::Float3, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		m_VAO->LinkVBO(vbo, 2, Chonps::SDT::Float2, Chonps::SDT::Float2, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		m_VAO->Unbind();
		vbo->Unbind();
		ebo->Unbind();

		m_Texture1 = std::unique_ptr<Chonps::Texture>(Chonps::createTexture(resPathDir + "textures/Checkerboard.png"));
		m_Texture2 = std::unique_ptr<Chonps::Texture>(Chonps::createTexture(resPathDir + "textures/cppLogo.png"));
		m_Texture1->TexUnit(&(*m_Shader), "texUnit", 0);
		m_Texture2->TexUnit(&(*m_Shader), "texUnit", 0);

		Chonps::renderInit();
		Chonps::Application::GetApp().UpdateWindowRender(false);
	}

	virtual ~Layer1() override
	{
		
	}

	virtual void OnAttach() override
	{
		
	}

	virtual void OnDetach() override
	{

	}

	virtual void OnUpdate() override
	{
		Chonps::renderClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		Chonps::renderClear();

		m_Camera.UpdateMatrix(m_Window->GetWidth(), m_Window->GetHeight());
		m_Camera.UploadMatrix(&(*m_Shader), "camMatrix");

		m_Shader->Bind();

		m_VAO->Bind();

		m_Texture1->Bind();
		Chonps::renderDraw(&(*m_VAO));
		m_Texture2->Bind();
		Chonps::renderDraw(&(*m_VAO));

		float ts = Chonps::Application::GetApp().GetTimestep();

		// Handles key inputs
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_W))
		{
			m_CameraPosition += (m_CameraSpeed * m_CameraOrientation) * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_A))
		{
			m_CameraPosition += (m_CameraSpeed * -glm::normalize(glm::cross(m_CameraOrientation, m_Camera.GetUpVector()))) * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_S))
		{
			m_CameraPosition += (m_CameraSpeed * -m_CameraOrientation) * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_D))
		{
			m_CameraPosition += (m_CameraSpeed * glm::normalize(glm::cross(m_CameraOrientation, m_Camera.GetUpVector()))) * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_SPACE))
		{
			m_CameraPosition += (m_CameraSpeed * m_Camera.GetUpVector()) * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_LEFT_CONTROL))
		{
			m_CameraPosition += (m_CameraSpeed * -m_Camera.GetUpVector()) * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_LEFT_SHIFT))
		{
			m_CameraSpeed = 8.0f;
		}
		else if (Chonps::keyReleased(m_Window, CHONPS_KEY_LEFT_CONTROL))
		{
			m_CameraSpeed = 4.0f;
		}

		m_Camera.SetPosition(m_CameraPosition);

		if (Chonps::mouseButtonPressed(m_Window, CHONPS_MOUSE_BUTTON_1))
		{
			// Hides mouse cursor
			Chonps::setMouseModeHide(m_Window, true);

			// Prevents camera from jumping on the first click
			if (m_CameraFirstClick)
			{
				Chonps::setMousePos(m_Window, ((float)m_Window->GetWidth() / 2), (float)(m_Window->GetHeight() / 2));
				m_CameraFirstClick = false;
			}

			// Stores the coordinates of the cursor
			float mouseX;
			float mouseY;
			// Fetches the coordinates of the cursor
			Chonps::getMousePos(m_Window, &mouseX, &mouseY);

			// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
			// and then "transforms" them into degrees 
			float rotX = m_CameraSensitivity * (float)(mouseY - (m_Window->GetHeight() / 2)) / m_Window->GetHeight();
			float rotY = m_CameraSensitivity * (float)(mouseX - (m_Window->GetWidth() / 2)) / m_Window->GetWidth();

			// Calculates upcoming vertical change in the m_CameraOrientation
			glm::vec3 newOrientation = glm::rotate(m_CameraOrientation, glm::radians(-rotX), glm::normalize(glm::cross(m_CameraOrientation, m_Camera.GetUpVector())));

			// Decides whether or not the next vertical m_CameraOrientation is legal or not
			if (abs(glm::angle(newOrientation, m_Camera.GetUpVector()) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				m_CameraOrientation = newOrientation;
			}

			// Rotates the m_CameraOrientation left and right
			m_CameraOrientation = glm::rotate(m_CameraOrientation, glm::radians(-rotY), m_Camera.GetUpVector());
			m_Camera.SetOrientation(m_CameraOrientation);

			// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
			Chonps::setMousePos(m_Window, ((float)m_Window->GetWidth() / 2), (float)(m_Window->GetHeight() / 2));
		}
		else if (Chonps::mouseButtonReleased(m_Window, CHONPS_MOUSE_BUTTON_1))
		{
			// Unhides cursor since camera is not looking around anymore
			Chonps::setMouseModeHide(m_Window, false);
			// Makes sure the next time the camera looks around it doesn't jump
			m_CameraFirstClick = true;
		}

		m_Window->OnUpdate();
	}

	virtual void OnEvent(Chonps::Event& e) override
	{
		
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Console");
		ImGui::Text("oh no");
		ImGui::End();
	}

	virtual bool LayerStatus() override
	{
		return m_UseLayer;
	}

private:
	std::string m_Name;
	bool m_UseLayer = true;

	std::unique_ptr<Chonps::Shader> m_Shader;
	Chonps::Camera m_Camera;
	std::unique_ptr<Chonps::VAO> m_VAO;

	Chonps::Window* m_Window = &Chonps::Application::GetApp().GetWindow();

	std::unique_ptr<Chonps::Texture> m_Texture1;
	std::unique_ptr<Chonps::Texture> m_Texture2;

	glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 m_CameraOrientation = glm::vec3(0.0f, 0.0f, -1.0f);
	
	float m_CameraSpeed = 4.0f;
	bool m_CameraFirstClick = true;
	float m_CameraSensitivity = 100.0f;
};

class OrthoLayer : public Chonps::Layer
{
public:
	OrthoLayer(std::string name)
		: m_Name(name), m_Camera(-4.0f, 4.0f, -3.0f, 3.0f)
	{
		float vertices[] =
		{
			// COORDINATES		/		COLOR			/	TEX COORD
			1.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
			1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		};

		uint32_t indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		const std::string resPathDir = (fs::current_path()).string() + "/res/"; // TEMPORARY: will remove later

		m_Shader = std::unique_ptr<Chonps::Shader>(Chonps::createShader(resPathDir + "shaders/shader.glsl"));

		m_VAO = std::unique_ptr<Chonps::VAO>(Chonps::createVAO());

		m_VAO->Bind();
		Chonps::VBO* vbo = Chonps::createVBO(vertices, sizeof(vertices));
		Chonps::EBO* ebo = Chonps::createEBO(indices, sizeof(indices));

		m_VAO->LinkEBO(ebo);
		m_VAO->LinkVBO(vbo, 0, Chonps::SDT::Float3, Chonps::SDT::Float3, 8 * sizeof(float), (void*)0);
		m_VAO->LinkVBO(vbo, 1, Chonps::SDT::Float3, Chonps::SDT::Float3, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		m_VAO->LinkVBO(vbo, 2, Chonps::SDT::Float2, Chonps::SDT::Float2, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		m_VAO->Unbind();
		vbo->Unbind();
		ebo->Unbind();

		m_Texture1 = std::unique_ptr<Chonps::Texture>(Chonps::createTexture(resPathDir + "textures/Checkerboard.png"));
		m_Texture2 = std::unique_ptr<Chonps::Texture>(Chonps::createTexture(resPathDir + "textures/cppLogo.png"));
		m_Texture1->TexUnit(&(*m_Shader), "texUnit", 0);
		m_Texture2->TexUnit(&(*m_Shader), "texUnit", 0);

		Chonps::renderInit();
	}

	virtual void OnUpdate() override
	{
		float ts = Chonps::Application::GetApp().GetTimestep();

		if (Chonps::keyPressed(m_Window, CHONPS_KEY_W))
		{
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_A))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_S))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_D))
		{
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_Q))
		{
			m_CameraRotation += m_CameraRotationSpeed * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_E))
		{
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		}
		if (Chonps::keyPressed(m_Window, CHONPS_KEY_LEFT_SHIFT))
		{
			m_CameraMoveSpeed = 6.0f;
			m_CameraRotationSpeed = 250.0f;
		}
		else if (Chonps::keyReleased(m_Window, CHONPS_KEY_LEFT_SHIFT))
		{
			m_CameraMoveSpeed = 2.0f;
			m_CameraRotationSpeed = 100.0f;
		}

		Chonps::renderClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		Chonps::renderClear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
		m_Camera.UploadMatrix(&(*m_Shader), "camMatrix");

		m_Shader->Bind();

		m_VAO->Bind();

		m_Texture1->Bind();
		Chonps::renderDraw(&(*m_VAO));
		m_Texture2->Bind();
		Chonps::renderDraw(&(*m_VAO));
	}

private:
	std::string m_Name;
	bool m_UseLayer = true;

	std::unique_ptr<Chonps::Shader> m_Shader;
	Chonps::OrthographicCamera m_Camera;
	std::unique_ptr<Chonps::VAO> m_VAO;

	Chonps::Window* m_Window = &Chonps::Application::GetApp().GetWindow();

	std::unique_ptr<Chonps::Texture> m_Texture1;
	std::unique_ptr<Chonps::Texture> m_Texture2;

	glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	float m_CameraMoveSpeed = 2.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 100.0f;
};

int main()
{
	Chonps::Log::Init();

	CHONPS_INFO("Initialized Log");
	
	Chonps::setWindowAPI(Chonps::WindowAPI::Glfw);
	Chonps::setRenderAPI(Chonps::API::OpenGL);

	Chonps::Application app("Chonps", 800, 600);
	
	//Layer1 l1("l1");
	OrthoLayer ortho("ortho");
	
	app.add_layer(&ortho);
	
	app.Run();


	Chonps::windowTerminateAPI();

	return 0;
}