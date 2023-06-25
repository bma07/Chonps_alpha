#include "ChonpsEditor.h"

#include <glm/gtc/type_ptr.hpp>
#include <Imgui/imgui.h>
#include <Imgui/imgui_internal.h>

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

namespace Chonps
{
	ChonpsEditor::ChonpsEditor(std::string name)
		: Layer(name), m_Name(name)
	{

	}

	void ChonpsEditor::OnAttach()
	{
		m_Camera = Camera(Chonps::getAppWindowWidth(), Chonps::getAppWindowHeight());

		const std::string resPathDir = "D:/Dev/Chonps/Sandbox/res/"; // TEMPORARY: will remove later

		m_Shader = Chonps::createShaderSp(resPathDir + "shaders/shader.glsl");
		m_LightShader = Chonps::createShaderSp(resPathDir + "shaders/lightShader.glsl");
		m_FrameBufferShader = Chonps::createShaderSp(resPathDir + "shaders/FBOshader.glsl");
		m_CubemapShader = Chonps::createShaderSp(resPathDir + "shaders/cubemapShader.glsl");
		m_PlaneShader = Chonps::createShaderSp(resPathDir + "shaders/infiniteGrid.glsl");

		m_FrameBufferVAO = Chonps::createVertexArraySp();

		m_FrameBufferVAO->Bind();
		Chonps::Shared<Chonps::VBO> FrambeBufferVBO = Chonps::createVertexBufferSp(screen, sizeof(screen));

		m_FrameBufferVAO->LinkVertexBuffer(&(*FrambeBufferVBO), 0, 2, Chonps::SDT::Float, 4 * sizeof(float), (void*)0);
		m_FrameBufferVAO->LinkVertexBuffer(&(*FrambeBufferVBO), 1, 2, Chonps::SDT::Float, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		m_FrameBufferVAO->Unbind();
		FrambeBufferVBO->Unbind();

		Chonps::FrameBufferSpecification fbSpec;
		fbSpec.Width = 800;
		fbSpec.Height = 600;
		fbSpec.Attachments = { Chonps::FBTexFormat::RGBA32F, Chonps::FBTexFormat::DEPTH24STENCIL8 };

		m_FBO = Chonps::createFrameBufferSp(fbSpec);

		Chonps::FrameBufferSpecification MsaafbSpec;
		MsaafbSpec.Width = 800;
		MsaafbSpec.Height = 600;
		MsaafbSpec.Attachments = { Chonps::FBTexFormat::RGBA32F, Chonps::FBTexFormat::DEPTH24STENCIL8 };
		MsaafbSpec.Samples = 8;

		m_MsaaFBO = Chonps::createFrameBufferSp(MsaafbSpec);

		Chonps::FrameBufferSpecification ImguifbSpec;
		ImguifbSpec.Width = 800;
		ImguifbSpec.Height = 600;
		ImguifbSpec.Attachments = { Chonps::FBTexFormat::RGBA32F, Chonps::FBTexFormat::DEPTH24STENCIL8 };

		m_ImguiFBO = Chonps::createFrameBufferSp(ImguifbSpec);

		/*Chonps::FrameBufferSpecification ShadowfbSpec;
		ShadowfbSpec.Width = 1024;
		ShadowfbSpec.Height = 1024;
		ShadowfbSpec.Attachments = { {Chonps::FBTexFormat::DEPTH_COMPONENT} };

		m_ShadowFBO = Chonps::createFrameBufferSp();*/

		std::string cubeMapFaces[6] =
		{
			resPathDir + "cubemaps/Chapel/posx.jpg",
			resPathDir + "cubemaps/Chapel/negx.jpg",
			resPathDir + "cubemaps/Chapel/posy.jpg",
			resPathDir + "cubemaps/Chapel/negy.jpg",
			resPathDir + "cubemaps/Chapel/posz.jpg",
			resPathDir + "cubemaps/Chapel/negz.jpg",
		};

		m_Cubemap = Chonps::createCubemapSp(cubeMapFaces);


		m_Camera.SetUp(glm::vec3(5.0f, 5.0f, 5.0f), 65.0f, 0.01f, 500.0f);
		m_Camera.SetOrientation(glm::vec3(-1.0f, -1.0f, -1.0f));

		glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, m_lightPos);

		m_LightVAO = Chonps::createVertexArraySp();

		m_LightVAO->Bind();
		Chonps::Shared<Chonps::VBO> lightVBO = Chonps::createVertexBufferSp(lightVertices, sizeof(lightVertices));
		Chonps::Shared<Chonps::IBO> lightIBO = Chonps::createIndexBufferSp(lightIndices, sizeof(lightIndices));

		m_LightVAO->LinkIndexBuffer(&(*lightIBO));
		m_LightVAO->LinkVertexBuffer(&(*lightVBO), 0, 3, Chonps::SDT::Float, 5 * sizeof(float), (void*)0);
		m_LightVAO->LinkVertexBuffer(&(*lightVBO), 1, 2, Chonps::SDT::Float, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		m_LightVAO->Unbind();
		lightVBO->Unbind();
		lightIBO->Unbind();

		m_PlaneVAO = Chonps::createVertexArraySp();
		m_PlaneVAO->Bind();
		Chonps::Shared<Chonps::VBO> planeVBO = Chonps::createVertexBufferSp(planeVertices, sizeof(planeVertices));
		Chonps::Shared<Chonps::IBO> planeIBO = Chonps::createIndexBufferSp(planeIndices, sizeof(planeIndices));

		m_PlaneVAO->LinkIndexBuffer(&(*planeIBO));
		m_PlaneVAO->LinkVertexBuffer(&(*planeVBO), 0, 3, Chonps::SDT::Float, 3 * sizeof(float), (void*)0);
		m_PlaneVAO->Unbind();
		planeVBO->Unbind();
		planeIBO->Unbind();

		m_LightShader->Bind();
		Chonps::oglSpec::uploadUniform4mfv(m_LightShader->GetID(), "model", 1, false, glm::value_ptr(lightModel));
		Chonps::oglSpec::uploadUniform4f(m_LightShader->GetID(), "lightColor", lightColor.x, lightColor.y, lightColor.z, lightColor.w);

		m_Shader->Bind();
		Chonps::oglSpec::uploadUniform4f(m_Shader->GetID(), "lightColor", lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		Chonps::oglSpec::uploadUniform3f(m_Shader->GetID(), "lightPos", m_lightPos.x, m_lightPos.y, m_lightPos.z);
		Chonps::oglSpec::uploadUniform1i(m_Shader->GetID(), "skybox", 0);

		m_FrameBufferShader->Bind();
		Chonps::oglSpec::uploadUniform1i(m_FrameBufferShader->GetID(), "screenTexture", 0);

		m_CubemapShader->Bind();
		Chonps::oglSpec::uploadUniform1i(m_CubemapShader->GetID(), "skybox", 0);

		m_SunTexture = Chonps::createTextureSp(resPathDir + "textures/sun.png", Chonps::TexT::Diffuse, Chonps::TexFormat::RGB8, { Chonps::TexFl::Linear, Chonps::TexFl::Linear });
		m_SunTexture->TexUnit(&(*m_LightShader), "texID", 0);

		Chonps::renderInit();

		m_Scene = Chonps::createSharedPtr<Chonps::Scene>();
		Entity entity = m_Scene->CreateEntity();
		m_Scene->SetEntityName(entity, "Flivver");

	//	MeshComponent meshComp(Chonps::loadModel(resPathDir + "models/gltf/Flivver/Flivver.gltf"), &(*m_Shader));
		TransformComponent transComp = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };
		TagComponent tagComp({ "GameObject", "Model", "EditorOnly" });

		m_Scene->GetRegistry().add_component<TransformComponent>(entity, transComp);
	//	m_Scene->GetRegistry().add_component<MeshComponent>(entity, meshComp);
		m_Scene->GetRegistry().add_component<TagComponent>(entity, tagComp);

		Entity entity2 = m_Scene->CreateEntity();
		m_Scene->SetEntityName(entity2, "Camera");

		m_Scene->GetRegistry().add_component<TransformComponent>(entity2, TransformComponent({0.0f, 0.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}));
	//	m_Scene->GetRegistry().add_component<MeshComponent>(entity2, MeshComponent(Chonps::loadModel(resPathDir + "models/gltf/brick/brick.gltf"), &(*m_Shader)));
		m_Scene->GetRegistry().add_component<CameraComponent>(entity2, CameraComponent(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), Chonps::getAppWindowWidth(), Chonps::getAppWindowHeight(), 65.0f, 0.01f, 500.0f));

		m_SceneHierarchy.SetScene(m_Scene);
		m_Properties.SetScene(m_Scene, &m_SceneHierarchy);
	}

	void ChonpsEditor::OnDetach()
	{

	}

	void ChonpsEditor::OnUpdate()
	{
		float dt = Chonps::Application::GetApp().GetTimestep();

		if (m_LastViewportSize != m_ViewportSize)
		{
			m_FBO->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_MsaaFBO->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ImguiFBO->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_Camera.SetDimensions((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
		m_LastViewportSize = m_ViewportSize;


		m_MsaaFBO->Begin();

		Chonps::renderClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		Chonps::renderClear();

		m_SceneHierarchy.OnUpdate();


		m_Cubemap->Use(&(*m_CubemapShader), m_Camera);
		

		Chonps::Renderer::ResetStats();
		
		switch (m_ViewportPlay)
		{
		case 0:
			m_Camera.UpdateMatrix();
			Chonps::renderBeginScene(m_Camera, &(*m_Shader));
			Chonps::oglSpec::uploadUniform3f(m_Shader->GetID(), "lightPos", m_lightPos.x, m_lightPos.y, m_lightPos.z);
			m_Scene->OnUpdateEditor(m_Camera, dt);
			break;
		case 1:
			if (m_Scene->GetPrimaryCameraEntity() > 0)
			{
				m_Scene->GetRegistry().get_component<CameraComponent>(m_Scene->GetPrimaryCameraEntity()).camera.SetDimensions(m_ViewportSize.x, m_ViewportSize.y);
				m_Scene->OnUpdateRuntime(&(*m_Shader), dt);
			}
			else
			{
				CHONPS_WARN("WARNING: SCENE: Primary Camera Entity not set!");
				m_ViewportPlay = false;
			}
			break;
		} 

		m_Stats = Chonps::Renderer::GetStats();
		

		glm::mat4 rot = glm::mat4(1.0f);
		rot = glm::inverse(glm::lookAt(m_lightPos, m_Camera.GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f)));

		Chonps::renderBeginScene(m_Camera, &(*m_LightShader));
		Chonps::oglSpec::uploadUniform4mfv(m_LightShader->GetID(), "model", 1, false, glm::value_ptr(rot));
		m_SunTexture->Bind();
		Chonps::renderDraw(&(*m_LightVAO));

		Chonps::renderEnableCullFace(false);
		Chonps::renderBeginScene(m_Camera, &(*m_PlaneShader));
		Chonps::renderDraw(&(*m_PlaneVAO));
		Chonps::renderEnableCullFace(true);

		Chonps::renderFrameBufferBlit(m_MsaaFBO->GetID(), m_FBO->GetID(), m_MsaaFBO->GetWidth(), m_MsaaFBO->GetHeight());
		m_MsaaFBO->End();

		m_FBO->Begin();
		m_FrameBufferShader->Bind();
		m_FrameBufferVAO->Bind();
		m_FBO->Draw();
		Chonps::renderFrameBufferBlit(m_FBO->GetID(), m_ImguiFBO->GetID(), m_FBO->GetWidth(), m_FBO->GetHeight());
		m_FBO->End();
	}

	void ChonpsEditor::OnEvent(Event& e)
	{
		if (e.GetEventType() == Chonps::EventType::WindowResize)
		{
			m_FBO->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_MsaaFBO->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ImguiFBO->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_Camera.SetDimensions((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
	}

	void ChonpsEditor::OnImGuiRender()
	{
		float dt = Chonps::Application::GetApp().GetTimestep() * 1000;

		float currentFrame = Chonps::getTimeSeconds();
		m_FrameCount++;
		if (currentFrame - m_PreviousFrame >= 1.0f)
		{
			m_FPS = m_FrameCount;
			m_PreviousFrame = currentFrame;
			m_FrameCount = 0;
		}

		ImGui::ShowDemoWindow();

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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("ChonpsEditor", &dockSpaceOpen, window_flags); // Begin Dockspace
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiStyle& style = ImGui::GetStyle();
		float WindowSize = style.WindowMinSize.x;
		style.WindowMinSize.x = 270.0f;
		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id = ImGui::GetID("ChonpsEditor");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
		style.WindowMinSize.x = WindowSize;

		ImGuiStyle* ImguiStyle = &ImGui::GetStyle();
		ImguiStyle->FramePadding = ImVec2(3, 5);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", 0))
				{

				}
				if (ImGui::MenuItem("Open", 0))
				{

				}
				if (ImGui::MenuItem("Save", 0))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit", 0))
					Chonps::Application::GetApp().SetRun(false);

				ImGui::EndMenu();
			}

			ImGui::SameLine(120.0f);
			ImGui::BeginTabBar("Tab");
			if (ImGui::BeginTabItem("Scene"))
			{
				// Scene Heiarchy
				m_SceneHierarchy.OnGuiRender();

				// Properties
				m_Properties.OnGuiRender();
				

				ImGui::Begin("test");

				ImGui::DragFloat("LightX", &m_lightPos.x, 0.1f);
				ImGui::DragFloat("LightY", &m_lightPos.y, 0.1f);
				ImGui::DragFloat("LightZ", &m_lightPos.z, 0.1f);

				ImGui::End();

				ImGui::Begin("Renderer");
				ImGui::Text(("FPS: " + std::to_string(m_FPS) + " | ms: " + std::to_string(dt)).c_str());
				ImGui::Separator();
				ImGui::Text("Statistics:");
				ImGui::Text(("- Triangles: " + std::to_string(m_Stats.triangles)).c_str());
				ImGui::Text(("- Vertices: " + std::to_string(m_Stats.vertices)).c_str());
				ImGui::Text(("- Indices: " + std::to_string(m_Stats.indices)).c_str());
				ImGui::Text(("- Draw Calls: " + std::to_string(m_Stats.drawCalls)).c_str());
				ImGui::End();

				ImGuiWindowClass window_class;
				window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
				ImGui::SetNextWindowClass(&window_class);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);
				ImGui::Begin("Viewport");
				ImGui::PopStyleVar(2);

				ImGui::IsWindowFocused() || ImGui::IsWindowHovered() ? m_ViewportFocused = true : m_ViewportFocused = false;
				if (Chonps::mouseButtonPressed(m_Window, CHONPS_MOUSE_BUTTON_3) && m_ViewportFocused) ImGui::SetWindowFocus();
				ImVec2 viewportSize = ImGui::GetContentRegionAvail();
				m_ViewportSize = glm::vec2(viewportSize.x, viewportSize.y);

				ImVec2 viewportMousePos = ImGui::GetMousePos();
				m_ViewportMousePos = glm::vec2(viewportMousePos.x, viewportMousePos.y);

				ImGui::Image(reinterpret_cast<ImTextureID>(m_ImguiFBO->GetTexID()), viewportSize, ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					ImGui::OpenPopup("popup");
				if (ImGui::BeginPopup("popup"))
				{
					ImGui::Text("This is a popup");
					ImGui::EndPopup();
				}

				ImGui::End();

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Tab2"))
			{
				ImGui::Begin("Window2");
				ImGui::Button("Yes");
				ImGui::End();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();

			ImVec2 region = ImGui::GetContentRegionAvail();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4);
			ImGui::SameLine(0.0f, region.x - 40.0f);
			if (m_ViewportPlay == false)
			{
				if (ImGui::Button("Play"))
					m_ViewportPlay = true;
			}
			else
			{
				if (ImGui::Button("Stop"))
					m_ViewportPlay = false;
			}
			ImGui::PopStyleVar(); // Play Button rounding
			ImGui::EndMenuBar();

		}

		ImGui::End(); // End Dockspace
	}
}