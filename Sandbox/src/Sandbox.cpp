#include <iostream>
#include <Chonps.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <Imgui/imgui.h>
#include <glad/glad.h>

#include <filesystem>
namespace fs = std::filesystem;

class Layer1 : public Chonps::Layer
{
public:
	Layer1(std::string name)
		: m_Name(name), m_Camera(Chonps::getAppWindowWidth(), Chonps::getAppWindowHeight())
	{

		float lightVertices[] =
		{ //     COORDINATES   //  TEX UV
			-0.1f, -0.1f, 0.0f,	 0.0f, 0.0f,
			-0.1f,  0.1f, 0.0f,  0.0f, 1.0f,
			 0.1f,  0.1f, 0.0f,  1.0f, 1.0f,
			 0.1f, -0.1f, 0.0f,	 1.0f, 0.0f,
		};

		uint32_t lightIndices[] =
		{
			0, 1, 2,
			0, 2, 3,
		};

		float screen[] =
		{
			// Coords    // texCoords
			 1.0f, -1.0f,  1.0f, 0.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 1.0f,

			 1.0f,  1.0f,  1.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 1.0f
		};

		float planeVertices[] =
		{
			-1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 
			 1.0f, -1.0f, 0.0f,
		};

		uint32_t planeIndices[] =
		{
			0, 1, 2,
			0, 2, 3,
		};

		const std::string resPathDir = "D:/Dev/Chonps/Sandbox/res/"; // TEMPORARY: will remove later

		m_Model = Chonps::Model("res/models/sphere/sphere.obj");
		m_Model.SpaceOrientationMode(CHONPS_LOCAL);
		

		m_Shader = std::unique_ptr<Chonps::Shader>(Chonps::createShader(resPathDir + "shaders/shader.glsl"));
		m_LightShader = std::unique_ptr<Chonps::Shader>(Chonps::createShader(resPathDir + "shaders/lightShader.glsl"));
		m_FrameBufferShader = std::unique_ptr<Chonps::Shader>(Chonps::createShader(resPathDir + "shaders/FBOshader.glsl"));
		m_CubemapShader = std::unique_ptr<Chonps::Shader>(Chonps::createShader(resPathDir + "shaders/cubemapShader.glsl"));
		m_PlaneShader = std::unique_ptr<Chonps::Shader>(Chonps::createShader(resPathDir + "shaders/infiniteGrid.glsl"));

		m_FrameBufferVAO = std::unique_ptr<Chonps::VAO>(Chonps::createVertexArray());
		
		m_FrameBufferVAO->Bind();
		Chonps::VBO* FrambeBufferVBO = Chonps::createVertexBuffer(screen, sizeof(screen));

		m_FrameBufferVAO->LinkVertexBuffer(FrambeBufferVBO, 0, 2, Chonps::SDT::Float, 4 * sizeof(float), (void*)0);
		m_FrameBufferVAO->LinkVertexBuffer(FrambeBufferVBO, 1, 2, Chonps::SDT::Float, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		m_FrameBufferVAO->Unbind();
		FrambeBufferVBO->Unbind();

		Chonps::FrameBufferSpecification fbSpec;
		fbSpec.Width = 800;
		fbSpec.Height = 600;
		fbSpec.Attachments = { Chonps::FBTexFormat::RGBA32F, Chonps::FBTexFormat::DEPTH24STENCIL8 };

		m_FBO = std::unique_ptr<Chonps::FBO>(Chonps::createFrameBuffer(fbSpec));
		
		Chonps::FrameBufferSpecification MsaafbSpec;
		MsaafbSpec.Width = 800;
		MsaafbSpec.Height = 600;
		MsaafbSpec.Attachments = { Chonps::FBTexFormat::RGBA32F, Chonps::FBTexFormat::DEPTH24STENCIL8 };
		MsaafbSpec.Samples = 8;

		m_MsaaFBO = std::unique_ptr<Chonps::FBO>(Chonps::createFrameBuffer(MsaafbSpec));
		
		Chonps::FrameBufferSpecification ImguifbSpec;
		ImguifbSpec.Width = 800;
		ImguifbSpec.Height = 600;
		ImguifbSpec.Attachments = { Chonps::FBTexFormat::RGBA32F, Chonps::FBTexFormat::DEPTH24STENCIL8 };
		
		m_ImguiFBO = std::unique_ptr<Chonps::FBO>(Chonps::createFrameBuffer(ImguifbSpec));

		std::string cubeMapFaces[6] =
		{
			resPathDir + "cubemaps/Chapel/posx.jpg",
			resPathDir + "cubemaps/Chapel/negx.jpg",
			resPathDir + "cubemaps/Chapel/posy.jpg",
			resPathDir + "cubemaps/Chapel/negy.jpg",
			resPathDir + "cubemaps/Chapel/posz.jpg",
			resPathDir + "cubemaps/Chapel/negz.jpg",
		};

		m_Cubemap = std::unique_ptr<Chonps::Cubemap>(Chonps::createCubemap(cubeMapFaces));


		m_Camera.SetUp(glm::vec3(0.0f, 0.0f, 5.0f), 65.0f, 0.01f, 500.0f);

		glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, m_lightPos);

		m_LightVAO = std::unique_ptr<Chonps::VAO>(Chonps::createVertexArray());

		m_LightVAO->Bind();
		Chonps::VBO* lightVBO = Chonps::createVertexBuffer(lightVertices, sizeof(lightVertices));
		Chonps::IBO* lightIBO = Chonps::createIndexBuffer(lightIndices, sizeof(lightIndices));
		
		m_LightVAO->LinkIndexBuffer(lightIBO);
		m_LightVAO->LinkVertexBuffer(lightVBO, 0, 3, Chonps::SDT::Float, 5 * sizeof(float), (void*)0);
		m_LightVAO->LinkVertexBuffer(lightVBO, 1, 2, Chonps::SDT::Float, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		m_LightVAO->Unbind();
		lightVBO->Unbind();
		lightIBO->Unbind();

		m_PlaneVAO = std::unique_ptr<Chonps::VAO>(Chonps::createVertexArray());
		m_PlaneVAO->Bind();
		Chonps::VBO* planeVBO = Chonps::createVertexBuffer(planeVertices, sizeof(planeVertices));
		Chonps::IBO* planeIBO = Chonps::createIndexBuffer(planeIndices, sizeof(planeIndices));

		m_PlaneVAO->LinkIndexBuffer(planeIBO);
		m_PlaneVAO->LinkVertexBuffer(planeVBO, 0, 3, Chonps::SDT::Float, 3 * sizeof(float), (void*)0);
		m_PlaneVAO->Unbind();
		planeVBO->Unbind();
		planeIBO->Unbind();

		m_LightShader->Bind();
		Chonps::uploadUniform4mfv(m_LightShader->GetID(), "model", 1, false, glm::value_ptr(lightModel));
		Chonps::uploadUniform4f(m_LightShader->GetID(), "lightColor", lightColor.x, lightColor.y, lightColor.z, lightColor.w);

		m_Shader->Bind();
		Chonps::uploadUniform4f(m_Shader->GetID(), "lightColor", lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		Chonps::uploadUniform3f(m_Shader->GetID(), "lightPos", m_lightPos.x, m_lightPos.y, m_lightPos.z);
		Chonps::uploadUniform1i(m_Shader->GetID(), "skybox", 0);

		m_FrameBufferShader->Bind();
		Chonps::uploadUniform1i(m_FrameBufferShader->GetID(), "screenTexture", 0);

		m_CubemapShader->Bind();
		Chonps::uploadUniform1i(m_CubemapShader->GetID(), "skybox", 0);

		m_SunTexture = std::unique_ptr<Chonps::Texture>(Chonps::createTexture(resPathDir + "textures/sun.png", Chonps::TexT::Diffuse, Chonps::TexF::Linear));
		m_SunTexture->TexUnit(&(*m_LightShader), "texID", 0);



		Chonps::renderInit();
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

	void CameraInputs(float ts)
	{
		if (Chonps::mouseButtonPressed(m_Window, CHONPS_MOUSE_BUTTON_3))
		{
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

			float xpos, ypos;
			Chonps::getMousePos(m_Window, &xpos, &ypos);

			if (m_CameraFirstClick)
			{
				m_LastX = xpos;
				m_LastY = ypos;
				m_CameraFirstClick = false;
			}

			float xoffset = xpos - m_LastX;
			float yoffset = m_LastY - ypos; // reversed since y-coordinates range from bottom to top
			m_LastX = xpos;
			m_LastY = ypos;

			xoffset *= m_CameraSensitivity;
			yoffset *= m_CameraSensitivity;

			glm::vec3 newOrientation = glm::rotate(m_CameraOrientation, glm::radians(yoffset), glm::normalize(glm::cross(m_CameraOrientation, m_Camera.GetUpVector())));
			if (abs(glm::angle(newOrientation, m_Camera.GetUpVector()) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				m_CameraOrientation = newOrientation;
			}

			// Rotates the m_CameraOrientation left and right
			m_CameraOrientation = glm::rotate(m_CameraOrientation, glm::radians(-xoffset), m_Camera.GetUpVector());
			m_Camera.SetOrientation(m_CameraOrientation);

			if (xpos > m_Window->GetWidth() - 2)
			{
				Chonps::setMousePos(m_Window, 2.0f, ypos);
				m_LastX = 2.0f;
			}
			else if (xpos < 2.0f)
			{
				Chonps::setMousePos(m_Window, m_Window->GetWidth() - 2.0f, ypos);
				m_LastX = m_Window->GetWidth() - 2.0f;
			}
			if (ypos > m_Window->GetHeight() - 2)
			{
				Chonps::setMousePos(m_Window, xpos, 2.0f);
				m_LastY = 2.0f;
			}
			else if (ypos < 2.0f)
			{
				Chonps::setMousePos(m_Window, xpos, m_Window->GetHeight() - 2.0f);
				m_LastY = m_Window->GetHeight() - 2.0f;
			}
		}
		else if (Chonps::mouseButtonReleased(m_Window, CHONPS_MOUSE_BUTTON_3))
		{
			m_CameraFirstClick = true;
		}
	}

	virtual void OnUpdate() override
	{
		float ts = Chonps::Application::GetApp().GetTimestep();

		if (m_LastViewportSize != m_ViewportSize)
		{
			m_FBO->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_MsaaFBO->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_ImguiFBO->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_Camera.SetDimensions(m_ViewportSize.x, m_ViewportSize.y);
		}
		m_LastViewportSize = m_ViewportSize;

		if (m_ViewportFocused)
		{
			CameraInputs(ts);
		}

		m_MsaaFBO->Begin();

		Chonps::renderClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		Chonps::renderClear();

		m_Camera.UpdateMatrix();

		m_Cubemap->Use(&(*m_CubemapShader), m_Camera);

		Chonps::renderBeginScene(m_Camera, &(*m_Shader));

		for (int i = 0; i < 10; i++)
		{
			Chonps::mat4 translate = Chonps::mat4(1.0f);
			translate = glm::translate(translate, glm::vec3(3.0f * (float)i, 0.0f, 0.0f));
			m_Model.Draw(&(*m_Shader), translate);
		}

		m_Model.SetPosition(m_ModelPos);
		m_Model.SetOrientation(m_ModelRot);
		m_Model.SetScale(m_ModelScale);

		Chonps::mat4 rot = Chonps::mat4(1.0f);
		rot = glm::inverse(glm::lookAt(m_lightPos, m_CameraPosition, glm::vec3(0.0f, 1.0f, 0.0f)));

		Chonps::renderBeginScene(m_Camera, &(*m_LightShader));
		Chonps::uploadUniform4mfv(m_LightShader->GetID(), "model", 1, false, glm::value_ptr(rot));
		m_SunTexture->Bind();
		Chonps::renderDraw(&(*m_LightVAO));

		/*Chonps::renderDisableCullFace();
		Chonps::renderBeginScene(m_Camera, &(*m_PlaneShader));
		Chonps::renderDraw(&(*m_PlaneVAO));
		Chonps::renderEnableCullFace();*/
		
		Chonps::renderFrameBufferBlit(m_MsaaFBO->GetID(), m_FBO->GetID(), m_MsaaFBO->GetWidth(), m_MsaaFBO->GetHeight());
		m_MsaaFBO->End();

		m_FBO->Begin();
		m_FrameBufferShader->Bind();
		m_FrameBufferVAO->Bind();
		m_FBO->Draw();
		Chonps::renderFrameBufferBlit(m_FBO->GetID(), m_ImguiFBO->GetID(), m_FBO->GetWidth(), m_FBO->GetHeight());
		m_FBO->End();
	}

	virtual void OnEvent(Chonps::Event& e) override
	{
		if (e.GetEventType() == Chonps::EventType::WindowResize)
		{
			m_FBO->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_MsaaFBO->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_ImguiFBO->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_Camera.SetDimensions(m_ViewportSize.x, m_ViewportSize.y);
		}
	}

	virtual void OnImGuiRender() override
	{
		float ts = Chonps::Application::GetApp().GetTimestep() * 1000;

		float currentFrame = Chonps::getTimeSeconds();
		m_FrameCount++;
		if (currentFrame - m_PreviousFrame >= 1.0f)
		{
			m_FPS = m_FrameCount;
			m_PreviousFrame = currentFrame;
			m_FrameCount = 0;
		}

		static bool dockSpaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus; 
		
		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("ChonpsEditor", &dockSpaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("ChonpsEditor");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
				if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
				if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
				ImGui::Separator();

				if (ImGui::MenuItem("Exit", 0))
					Chonps::Application::GetApp().SetRun(false);
					
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("console");
		ImGui::Text(("FPS: " + std::to_string(m_FPS) + " | Miliseconds: " + std::to_string(ts)).c_str());
		ImGui::Checkbox("Play", &m_ViewportPlay);
		float* pos[3] = { &m_ModelPos.x, &m_ModelPos.y, &m_ModelPos.z };
		ImGui::SliderFloat3("Position", *pos, -10.0f, 10.0f);
		float* rotate[3] = { &m_ModelRot.x, &m_ModelRot.y, &m_ModelRot.z };
		ImGui::SliderFloat3("Rotation", *rotate, -360.0f, 360.0f);
		float* scale[3] = { &m_ModelScale.x, &m_ModelScale.y, &m_ModelScale.z };
		ImGui::SliderFloat3("Scale", *scale, -10.0f, 10.0f);
		ImGui::End();


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImGui::IsWindowFocused() || ImGui::IsWindowHovered() ? m_ViewportFocused = true : m_ViewportFocused = false;
		if (Chonps::mouseButtonPressed(m_Window, CHONPS_MOUSE_BUTTON_3) && m_ViewportFocused) ImGui::SetWindowFocus();
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = Chonps::vec2(viewportSize.x, viewportSize.y);
		
		ImVec2 viewportMousePos = ImGui::GetMousePos();
		m_ViewportMousePos = Chonps::vec2(viewportMousePos.x, viewportMousePos.y);
		ImGui::Image(reinterpret_cast<ImTextureID>(m_ImguiFBO->GetTexID()), viewportSize, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	virtual bool LayerStatus() override
	{
		return m_UseLayer;
	}

private:
	std::string m_Name;
	bool m_UseLayer = true;

	Chonps::Camera m_Camera;
	std::unique_ptr<Chonps::Shader> m_Shader;
	std::unique_ptr<Chonps::Shader> m_LightShader;
	std::unique_ptr<Chonps::Shader> m_FrameBufferShader;
	std::unique_ptr<Chonps::Shader> m_CubemapShader;
	std::unique_ptr<Chonps::Shader> m_PlaneShader;

	std::unique_ptr<Chonps::Cubemap> m_Cubemap;

	std::unique_ptr<Chonps::FBO> m_FBO;
	std::unique_ptr<Chonps::FBO> m_MsaaFBO;
	std::unique_ptr<Chonps::FBO> m_ImguiFBO;


	Chonps::vec2 m_ViewportSize = Chonps::vec2(1.0f);
	Chonps::vec2 m_LastViewportSize = Chonps::vec2(1.0f);
	Chonps::vec2 m_ViewportMousePos = Chonps::vec2(0.0f);
	bool m_ViewportFocused = false;
	bool m_ViewportPlay = false;

	std::unique_ptr<Chonps::VAO> m_VAO;
	std::unique_ptr<Chonps::VAO> m_LightVAO;
	std::unique_ptr<Chonps::VAO> m_FrameBufferVAO;
	std::unique_ptr<Chonps::VAO> m_PlaneVAO;

	Chonps::Window* m_Window = &Chonps::Application::GetApp().GetWindow();

	std::unique_ptr<Chonps::Texture> m_SunTexture;

	glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 m_CameraOrientation = glm::vec3(0.0f, 0.0f, -1.0f);
	
	float m_CameraSpeed = 4.0f;
	bool m_CameraFirstClick = true;
	float m_CameraSensitivity = 0.1f;
	float m_LastX = 0.0f, m_LastY = 0.0f;

	Chonps::Model m_Model;
	Chonps::vec3 m_ModelPos = Chonps::vec3(0.0f, 0.0f, 0.0f);
	Chonps::vec3 m_ModelRot = Chonps::vec3(0.0f, 0.0f, 0.0f);
	Chonps::vec3 m_ModelScale = Chonps::vec3(1.0f, 1.0f, 1.0f);

	Chonps::vec3 m_lightPos = Chonps::vec3(0.5f, 1.5f, 1.5f);

	float m_PreviousFrame = 0.0f;
	int m_FrameCount = 0;
	int m_FPS = 0;
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

		m_VAO = std::unique_ptr<Chonps::VAO>(Chonps::createVertexArray());

		m_VAO->Bind();
		Chonps::VBO* vbo = Chonps::createVertexBuffer(vertices, sizeof(vertices));
		Chonps::IBO* ibo = Chonps::createIndexBuffer(indices, sizeof(indices));

		m_VAO->LinkIndexBuffer(ibo);
		m_VAO->LinkVertexBuffer(vbo, 0, Chonps::SDT::Float3, Chonps::SDT::Float3, 8 * sizeof(float), (void*)0);
		m_VAO->LinkVertexBuffer(vbo, 1, Chonps::SDT::Float3, Chonps::SDT::Float3, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		m_VAO->LinkVertexBuffer(vbo, 2, Chonps::SDT::Float2, Chonps::SDT::Float2, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		m_VAO->Unbind();
		vbo->Unbind();
		ibo->Unbind();

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

	virtual void OnImGuiRender() override
	{
		// If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
		// In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
		// In this specific demo, we are not using DockSpaceOverViewport() because:
		// - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
		// - we allow the host window to have padding (when opt_padding == true)
		// - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
		// TL;DR; this demo is more complicated than what you would normally use.
		// If we removed all the options we are showcasing, this demo would become:
		//     void ShowExampleAppDockSpace()
		//     {
		//         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		//     }

		static bool dockSpaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
				if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
				if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
				ImGui::Separator();

				if (ImGui::MenuItem("Close", NULL, false))
					dockSpaceOpen = false;
				ImGui::EndMenu();
			}
			
			ImGui::EndMenuBar();
		}

		ImGui::End();
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

