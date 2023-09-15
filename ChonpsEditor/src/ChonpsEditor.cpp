#include "ChonpsEditor.h"

#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
namespace fs = std::filesystem;

float vertices[] =
{
	-0.5f,  0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.0f, 0.0f,
	 0.5f, -0.5f, 1.0f, 0.0f,
	 0.5f,  0.5f, 1.0f, 1.0f,
};

std::vector<Chonps::vertex> vertices2 =
{
	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

uint32_t indices[] =
{
	0, 1, 2, 0, 2, 3,
};

std::vector<uint32_t> indices2 =
{
	0, 1, 2, 2, 3, 0,
};


struct UniformBufferObject
{
	glm::mat4 camMatrix;
	ALIGN(16) int texIndex;
};

struct CameraMatrixUniformBuffer
{
	glm::mat4 camMatrix;
};

namespace Chonps
{
	void CameraRotate(Camera* camera)
	{
		const float radius = 5.0f;
		float camX = std::sin(getTimeSeconds()) * radius;
		float camY = std::sin(getTimeSeconds() * 0.5f) * radius;
		float camZ = std::cos(getTimeSeconds()) * radius;

		camera->position = glm::vec3(camX, camY, camZ);
	}

	void UpdateVerts()
	{
		float change = std::sin(getTimeSeconds());

		for (auto& vertex : vertices2)
		{
			vertex.pos.z = change;
		}
	}

	void ChonpsEditor::CameraMovment(Window* window, Camera* camera, float dt)
	{
		if (keyPressed(m_Window, CHONPS_KEY_W))
			camera->position += (m_CameraSpeed * camera->orientation) * dt;

		if (keyPressed(m_Window, CHONPS_KEY_A))
			camera->position += (m_CameraSpeed * -glm::normalize(glm::cross(camera->orientation, camera->upVector))) * dt;

		if (keyPressed(m_Window, CHONPS_KEY_S))
			camera->position += (m_CameraSpeed * -camera->orientation) * dt;

		if (keyPressed(m_Window, CHONPS_KEY_D))
			camera->position += (m_CameraSpeed * glm::normalize(glm::cross(camera->orientation, camera->upVector))) * dt;

		if (keyPressed(m_Window, CHONPS_KEY_SPACE))
			camera->position += (m_CameraSpeed * camera->upVector) * dt;

		if (keyPressed(m_Window, CHONPS_KEY_LEFT_CONTROL))
			camera->position += (m_CameraSpeed * -camera->upVector) * dt;

		if (mouseButtonPressed(window, CHONPS_MOUSE_BUTTON_2))
		{
			auto mousePos = getMousePos(window);
			
			if (m_CameraFirstClick)
			{
				m_LastMouseX = mousePos.x;
				m_LastMouseY = mousePos.y;
				m_CameraFirstClick = false;
			}
			float xoffset = mousePos.x - m_LastMouseX;
			float yoffset = m_LastMouseY - mousePos.y; // reversed since y-coordinates range from bottom to top
			m_LastMouseX = mousePos.x;
			m_LastMouseY = mousePos.y;
			xoffset *= m_CameraSensitivity;
			yoffset *= m_CameraSensitivity;

			glm::vec3 newOrientation = glm::rotate(camera->orientation, glm::radians(yoffset), glm::normalize(glm::cross(camera->orientation, camera->upVector)));
			if (abs(glm::angle(glm::normalize(newOrientation), camera->upVector) - glm::radians(90.0f)) <= glm::radians(85.0f))
			{
				if (abs(glm::angle(glm::normalize(newOrientation), glm::normalize(camera->upVector)) - glm::radians(90.0f)) <= glm::radians(85.0f))
					camera->orientation = newOrientation;
			}

			// Rotates the orientation left and right
			camera->orientation = glm::normalize(glm::rotate(camera->orientation, glm::radians(-xoffset), camera->upVector));


			if (mousePos.x > m_Window->GetWidth() - m_CameraMouseMoveOffWindowOffset)
			{
				setMousePos(m_Window, m_CameraMouseMoveOffWindowOffset, mousePos.y);
				m_LastMouseX = m_CameraMouseMoveOffWindowOffset;
			}
			else if (mousePos.x < m_CameraMouseMoveOffWindowOffset)
			{
				setMousePos(m_Window, m_Window->GetWidth() - m_CameraMouseMoveOffWindowOffset, mousePos.y);
				m_LastMouseX = m_Window->GetWidth() - m_CameraMouseMoveOffWindowOffset;
			}
			if (mousePos.y > m_Window->GetHeight() - m_CameraMouseMoveOffWindowOffset)
			{
				setMousePos(m_Window, mousePos.x, m_CameraMouseMoveOffWindowOffset);
				m_LastMouseY = m_CameraMouseMoveOffWindowOffset;
			}
			else if (mousePos.y < m_CameraMouseMoveOffWindowOffset)
			{
				setMousePos(m_Window, mousePos.x, m_Window->GetHeight() - m_CameraMouseMoveOffWindowOffset);
				m_LastMouseY = m_Window->GetHeight() - m_CameraMouseMoveOffWindowOffset;
			}
		}
		else if (mouseButtonReleased(m_Window, CHONPS_MOUSE_BUTTON_2))
			m_CameraFirstClick = true;
		
	}

	float cameraSpeedValueChange(float speed)
	{
		return static_cast<float>(speed > 0.0f ? 0.02f * CHONPS_EULER * std::pow(speed - 0.1f, 2) : 0.0f);
	}

	bool ChonpsEditor::MouseScrolled(MouseScrolledEvent& e)
	{
		m_CameraSpeed = cameraSpeedValueChange(m_CameraSpeedScrollValue += e.GetMouseScrollY());

		if (m_CameraSpeedScrollValue < 0)
			m_CameraSpeedScrollValue = 0;
		else if (m_CameraSpeedScrollValue > 138)
			m_CameraSpeedScrollValue = 138;

		return true;
	}

	ChonpsEditor::ChonpsEditor(std::string name)
		: Layer(name), m_Name(name), m_Window(Application::GetApp().GetWindow())
	{

	}

	void ChonpsEditor::OnAttach()
	{
		gui::Init();
		m_Window->SetVSync(false);

		// Viewport
		m_ViewportWidth = 800;
		m_ViewportHeight = 600;
		m_ViewportX = 100;
		m_ViewportY = 100;

		// Camera
		m_Camera = OrthographicCamera(0.0f, static_cast<float>(m_Window->GetWidth()), 0.0f, static_cast<float>(m_Window->GetHeight()), -1.0f, 1.0f);
		m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		m_Camera.SetRotation(0.0f);

		// Scene Camera
		m_SceneCamera = Camera(m_Window->GetWidth(), m_Window->GetHeight());
		m_SceneCamera.SetFormat(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), 65.0f, 0.01f, 1000.0f);

		// Uniform Buffers
		m_UniformBuffer = createUniformBuffer(0, sizeof(UniformBufferObject), BufferType::Storage);
		
		BufferBindingInfo bufferInfo{};
		bufferInfo.uniformBuffer = m_UniformBuffer;
		bufferInfo.shaderStage = ShaderStage::Vertex;
		bufferInfo.bufferType = BufferType::Storage;
		bufferInfo.binding = 0;

		m_SceneCameraUniformBuffer = createUniformBuffer(1, sizeof(CameraMatrixUniformBuffer), BufferType::Uniform);

		BufferBindingInfo sceneCameraBufferInfo{};
		sceneCameraBufferInfo.uniformBuffer = m_SceneCameraUniformBuffer;
		sceneCameraBufferInfo.shaderStage = ShaderStage::Vertex;
		sceneCameraBufferInfo.bufferType = BufferType::Uniform;
		sceneCameraBufferInfo.binding = 1;

		m_CameraUniformBuffer = createUniformBuffer(2, sizeof(CameraMatrixUniformBuffer), BufferType::Uniform);

		BufferBindingInfo cameraBufferInfo{};
		cameraBufferInfo.uniformBuffer = m_CameraUniformBuffer;
		cameraBufferInfo.shaderStage = ShaderStage::Vertex;
		cameraBufferInfo.bufferType = BufferType::Uniform;
		cameraBufferInfo.binding = 2;

		std::vector<BufferBindingInfo> buffers = { bufferInfo, sceneCameraBufferInfo, cameraBufferInfo };

		BufferBindingCreateLayoutsInfo bufferBindingInfo{};
		bufferBindingInfo.bindingCount = static_cast<uint32_t>(buffers.size());
		bufferBindingInfo.pBufferBindings = buffers.data();

		BufferLayout bufferLayout = createBufferLayout(&bufferBindingInfo, 0);


		// Vao
		m_VAO = createVertexArray();
		m_VAO->Bind();

		VertexBuffer* vbo = createVertexBuffer(vertices, sizeof(vertices));
		IndexBuffer* ibo = createIndexBuffer(indices, sizeof(indices));

		std::vector<VertexLayout> layouts1 =
		{
			{ 0, 2, SDT::Float, 4 * sizeof(float), (void*)0 },
			{ 1, 2, SDT::Float, 4 * sizeof(float), (void*)(2 * sizeof(float)) },
		};

		VertexLayoutLinkInfo linkInfo{};
		linkInfo.pLayouts = layouts1.data();
		linkInfo.layoutCount = static_cast<uint32_t>(layouts1.size());

		m_VAO->LinkBuffers(vbo, ibo, &linkInfo);

		m_VAO->Unbind();
		vbo->Delete();
		ibo->Delete();

		// Vao2
		m_VAO2 = createVertexArray();

		m_VBO2 = createVertexBuffer(vertices2, BufferState::Dynamic);
		m_IBO2 = createIndexBuffer(indices2, BufferState::Dynamic);

		std::vector<VertexLayout> layouts2 =
		{
			{ 0, 3, SDT::Float, sizeof(vertex), (void*)0 },
			{ 1, 3, SDT::Float, sizeof(vertex), (void*)(3 * sizeof(float)) },
			{ 2, 2, SDT::Float, sizeof(vertex), (void*)(6 * sizeof(float)) }
		};

		VertexLayoutLinkInfo linkInfo2{};
		linkInfo2.pLayouts = layouts2.data();
		linkInfo2.layoutCount = static_cast<uint32_t>(layouts2.size());

		m_VAO2->LinkBuffers(m_VBO2, m_IBO2, &linkInfo2);

		m_VAO2->Unbind();


		m_Font = Font("D:/Dev/Chonps/ChonpsEditor/res/Fonts/Helvetica.ttf");
		gui::AddFontFromTTF(m_Font);

		// Mesh
		m_Meshes = loadModel("D:/Dev/Chonps/ChonpsEditor/res/models/gltf/saul_goodmans_office/scene.gltf");

		// Textures
		Texture* texture = createTexture("D:/Dev/Chonps/ChonpsEditor/res/textures/brick.png", TexType::Diffuse);
		Texture* texture2 = createTexture("D:/Dev/Chonps/ChonpsEditor/res/textures/Checkerboard.png", TexType::Diffuse);

		std::vector<TextureCreateInfo> textures =
		{
			{ texture, 0 },
			{ texture2, 1 }
		};

		m_TextureLayout = createTextureLayout(textures.data(), textures.size(), 1);

		CubemapCreateInfo cubemapCreateInfo{};
		cubemapCreateInfo.posx = "D:/Dev/Chonps/ChonpsEditor/res/cubemaps/Chapel/posx.jpg";
		cubemapCreateInfo.negx = "D:/Dev/Chonps/ChonpsEditor/res/cubemaps/Chapel/negx.jpg";
		cubemapCreateInfo.posy = "D:/Dev/Chonps/ChonpsEditor/res/cubemaps/Chapel/posy.jpg";
		cubemapCreateInfo.negy = "D:/Dev/Chonps/ChonpsEditor/res/cubemaps/Chapel/negy.jpg";
		cubemapCreateInfo.posz = "D:/Dev/Chonps/ChonpsEditor/res/cubemaps/Chapel/posz.jpg";
		cubemapCreateInfo.negz = "D:/Dev/Chonps/ChonpsEditor/res/cubemaps/Chapel/negz.jpg";

		m_Cubemap = createCubemap(cubemapCreateInfo, 1);

		// FrameBuffers
		std::vector<FrameBufferColorAttachment> fbColorAttachment = { { 0, FrameBufferColorFormat::RGBA8 }, { 1, FrameBufferColorFormat::RGBA8 } };
		FrameBufferDepthAttachment fbDepthAttachment = { 2, FrameBufferDepthFormat::DEPTH24STENCIL8 };

		FrameBufferSpecificationInfo frameBufferSpec{};
		frameBufferSpec.width = m_Window->GetWidth();
		frameBufferSpec.height = m_Window->GetHeight();
		frameBufferSpec.samples = Sample::SampleCount_8_Bit;
		frameBufferSpec.colorAttachmentCount = static_cast<uint32_t>(fbColorAttachment.size());
		frameBufferSpec.colorAttachments = fbColorAttachment.data();
		frameBufferSpec.depthAttachment = &fbDepthAttachment;
		frameBufferSpec.textureFilter = TexFilter::Nearest;
		frameBufferSpec.textureWrap = TexWrap::ClampToEdge;

		m_FBO = createFrameBuffer(frameBufferSpec, 1);

		// Pipelines
		RenderPass renderPass = retrieveRenderPass(m_FBO);

		ColorBlendAttachment attachment1{};
		attachment1.colorWriteMask = { true, true, true, true };
		attachment1.blendEnable = true;
		attachment1.srcColorBlendFactor = ColorBlendFactor::SrcAlpha;
		attachment1.dstColorBlendFactor = ColorBlendFactor::OneMinusSrcAlpha;
		attachment1.colorBlendOp = ColorBlendOperation::Add;
		attachment1.srcAlphaBlendFactor = ColorBlendFactor::SrcAlpha;
		attachment1.dstAlphaBlendFactor = ColorBlendFactor::OneMinusSrcAlpha;
		attachment1.alphaBlendOp = ColorBlendOperation::Add;
		ColorBlendAttachment attachment2 = attachment1;

		std::vector<ColorBlendAttachment> colorBlendAttachments = { attachment1, attachment2 };

		PipelineSpecification pipelineSpec = getStandardPipelineSpecification();
		pipelineSpec.multisampling.rasterizationSamples = Sample::SampleCount_8_Bit;
		pipelineSpec.colorBlend.colorBlendAttachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
		pipelineSpec.colorBlend.pColorBlendAttachments = colorBlendAttachments.data();

		// Scene Pipeline
		std::vector<VertexLayout> sceneLayout =
		{
			{ 0, 3, SDT::Float, sizeof(vertex), (void*)0 },
			{ 1, 3, SDT::Float, sizeof(vertex), (void*)(3 * sizeof(float)) },
			{ 2, 2, SDT::Float, sizeof(vertex), (void*)(6 * sizeof(float)) },
		};

		DescriptorImageLayoutIncludes layoutInclude = { 1, ImageLayoutOption::TextureImages };

		PipelineLayoutInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.bindingDescription = sizeof(vertex);
		pipelineLayoutInfo.vertexLayoutLinkInfo.layoutCount = static_cast<uint32_t>(sceneLayout.size());
		pipelineLayoutInfo.vertexLayoutLinkInfo.pLayouts = sceneLayout.data();
		pipelineLayoutInfo.bufferLayoutsCount = 1;
		pipelineLayoutInfo.pBufferLayouts = &bufferLayout;
		pipelineLayoutInfo.layoutIncludeCount = 1;
		pipelineLayoutInfo.pLayoutIncludes = &layoutInclude;
		pipelineLayoutInfo.renderPass = &renderPass;
		pipelineLayoutInfo.pipelineSpecification;
		pipelineLayoutInfo.pipelineSpecification = &pipelineSpec;

		m_Shader = createShader("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/shader_vert.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/shader_frag.spv", &pipelineLayoutInfo);

		// Cubemap Pipeline
		std::vector<VertexLayout> cubemapLayout =
		{
			{ 0, 3, SDT::Float, 3 * sizeof(float), (void*)0 },
		};

		DescriptorImageLayoutIncludes cubemaplayoutInclude = { 1, ImageLayoutOption::CubemapImages };

		PipelineLayoutInfo cubemappipelineLayoutInfo{};
		cubemappipelineLayoutInfo.bindingDescription = 3 * sizeof(float);
		cubemappipelineLayoutInfo.vertexLayoutLinkInfo.layoutCount = static_cast<uint32_t>(cubemapLayout.size());
		cubemappipelineLayoutInfo.vertexLayoutLinkInfo.pLayouts = cubemapLayout.data();
		cubemappipelineLayoutInfo.bufferLayoutsCount = 1;
		cubemappipelineLayoutInfo.pBufferLayouts = &bufferLayout;
		cubemappipelineLayoutInfo.layoutIncludeCount = 1;
		cubemappipelineLayoutInfo.pLayoutIncludes = &cubemaplayoutInclude;
		cubemappipelineLayoutInfo.renderPass = &renderPass;
		cubemappipelineLayoutInfo.pipelineSpecification = &pipelineSpec;

		m_CubemapShader = createShader("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/cubemap_vert.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/cubemap_frag.spv", &cubemappipelineLayoutInfo);

		// Framebuffer Pipeline
		std::vector<VertexLayout> FBlayouts =
		{
			{ 0, 2, SDT::Float, 4 * sizeof(float), (void*)0 },
			{ 1, 2, SDT::Float, 4 * sizeof(float), (void*)(2 * sizeof(float)) },
		};

		DescriptorImageLayoutIncludes FBlayoutInclude = { 1, ImageLayoutOption::FrameBufferImages };

		PipelineLayoutInfo FBpipelineLayoutInfo{};
		FBpipelineLayoutInfo.bindingDescription = 4 * sizeof(float);
		FBpipelineLayoutInfo.vertexLayoutLinkInfo.layoutCount = static_cast<uint32_t>(FBlayouts.size());
		FBpipelineLayoutInfo.vertexLayoutLinkInfo.pLayouts = FBlayouts.data();
		FBpipelineLayoutInfo.bufferLayoutsCount = 1;
		FBpipelineLayoutInfo.pBufferLayouts = &bufferLayout;
		FBpipelineLayoutInfo.layoutIncludeCount = 1;
		FBpipelineLayoutInfo.pLayoutIncludes = &FBlayoutInclude;

		m_FBOShader = createShader("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/FBOshader_vert.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/FBOshader_frag.spv", &FBpipelineLayoutInfo);

		// PreStencil Pipeline
		std::vector<VertexLayout> guilayouts =
		{
			{ 0, 2, SDT::Float, 9 * sizeof(float), (void*)(0) },
			{ 1, 4, SDT::Float, 9 * sizeof(float), (void*)(2 * sizeof(float)) },
			{ 2, 2, SDT::Float, 9 * sizeof(float), (void*)(6 * sizeof(float)) },
			{ 3, 1, SDT::Float, 9 * sizeof(float), (void*)(8 * sizeof(float)) },
		};


		PipelineSpecification preStencilPipelineSpec = getStandardPipelineSpecification();
		preStencilPipelineSpec.rasterizer.cullMode = CullFaceMode::Disable;
		preStencilPipelineSpec.depthstencil.enableStencil = true;
		preStencilPipelineSpec.depthstencil.stencil.compareOp = CompareOperation::Always;
		preStencilPipelineSpec.depthstencil.stencil.failOp = StencilOperation::Keep;
		preStencilPipelineSpec.depthstencil.stencil.depthFailOp = StencilOperation::Keep;
		preStencilPipelineSpec.depthstencil.stencil.passOp = StencilOperation::Replace;
		preStencilPipelineSpec.depthstencil.stencil.reference = 1;
		preStencilPipelineSpec.depthstencil.stencil.compareMask = 0xff;
		preStencilPipelineSpec.depthstencil.stencil.writeMask = 0xff;

		PipelineLayoutInfo preStencilPipelineLayoutInfo{};
		preStencilPipelineLayoutInfo.bindingDescription = 9 * sizeof(float);
		preStencilPipelineLayoutInfo.vertexLayoutLinkInfo.layoutCount = static_cast<uint32_t>(guilayouts.size());
		preStencilPipelineLayoutInfo.vertexLayoutLinkInfo.pLayouts = guilayouts.data();
		preStencilPipelineLayoutInfo.bufferLayoutsCount = 1;
		preStencilPipelineLayoutInfo.pBufferLayouts = &bufferLayout;
		preStencilPipelineLayoutInfo.layoutIncludeCount = 0;
		preStencilPipelineLayoutInfo.pLayoutIncludes = nullptr;
		preStencilPipelineLayoutInfo.pipelineSpecification = &preStencilPipelineSpec;

		m_PreStencilShader = createShader("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/preStencil_vert.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/preStencil_frag.spv", &preStencilPipelineLayoutInfo);

		// GUI Pipeline
		PipelineSpecification guiPipelineSpec = getStandardPipelineSpecification();
		guiPipelineSpec.rasterizer.cullMode = CullFaceMode::Disable;
		guiPipelineSpec.depthstencil.enableStencil = true;
		guiPipelineSpec.depthstencil.stencil.compareOp = CompareOperation::Equal;
		guiPipelineSpec.depthstencil.stencil.failOp = StencilOperation::Keep;
		guiPipelineSpec.depthstencil.stencil.depthFailOp = StencilOperation::Keep;
		guiPipelineSpec.depthstencil.stencil.passOp = StencilOperation::IncrementAndClamp;
		guiPipelineSpec.depthstencil.stencil.reference = 1;
		guiPipelineSpec.depthstencil.stencil.compareMask = 0xff;
		guiPipelineSpec.depthstencil.stencil.writeMask = 0x00;
		guiPipelineSpec.depthstencil.enableDepth = false;

		PipelineLayoutInfo guipipelineLayoutInfo{};
		guipipelineLayoutInfo.bindingDescription = 9 * sizeof(float);
		guipipelineLayoutInfo.vertexLayoutLinkInfo.layoutCount = static_cast<uint32_t>(guilayouts.size());
		guipipelineLayoutInfo.vertexLayoutLinkInfo.pLayouts = guilayouts.data();
		guipipelineLayoutInfo.bufferLayoutsCount = 1;
		guipipelineLayoutInfo.pBufferLayouts = &bufferLayout;
		guipipelineLayoutInfo.layoutIncludeCount = 0;
		guipipelineLayoutInfo.pLayoutIncludes = nullptr;
		guipipelineLayoutInfo.pipelineSpecification = &guiPipelineSpec;

		m_GuiShader = createShader("D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/gui_vert.spv", "D:/Dev/Chonps/ChonpsEditor/res/shaders/vulkan/spvShaders/gui_frag.spv", &guipipelineLayoutInfo);
		gui::SetShader(m_PreStencilShader, m_GuiShader, m_CameraUniformBuffer);

	}

	void ChonpsEditor::OnDetach()
	{
		m_UniformBuffer->Delete();
		m_CameraUniformBuffer->Delete();
		m_SceneCameraUniformBuffer->Delete();
		m_Cubemap->Delete();
		m_Shader->Delete();
		m_FBOShader->Delete();
		m_CubemapShader->Delete();
		m_GuiShader->Delete();
		m_PreStencilShader->Delete();
		m_VAO->Delete();
		m_VAO2->Delete();
		m_VBO2->Delete();
		m_IBO2->Delete();
		m_TextureLayout->Delete();
		m_FBO->Delete();
		m_Font.Delete();

		for (auto& meshes : m_Meshes)
			meshes.Delete();

		gui::Terminate();
	}

	void ChonpsEditor::OnUpdate()
	{
		float dt = Application::GetApp().GetTimestep();

		auto [mx, my] = getMousePos(m_Window);
		my = m_Window->GetHeight() - my;
		float vpmx = mx - m_ViewportX;
		float vpmy = my - m_ViewportY;
		//CHONPS_TRACE("{0}, {1}", vpmx, vpmy);

		// Input
		/*if (mouseButtonPressed(m_Window, CHONPS_MOUSE_BUTTON_1) && within<float>(mx, m_ViewportX, m_MouseDragRange) || m_FirstClickX)
		{
			if (!m_FirstClickX)
			{
				m_FirstClickX = true;
				m_MouseClickDownX = mx;
				m_OldViewportWidth = m_ViewportWidth;
			}
			int dragAmount = m_MouseClickDownX - mx;

			m_ViewportX = mx;
			m_ViewportWidth = m_OldViewportWidth + dragAmount;
		}
		else if (mouseButtonPressed(m_Window, CHONPS_MOUSE_BUTTON_1) && within<float>(my, m_ViewportY, m_MouseDragRange) || m_FirstClickY)
		{
			if (!m_FirstClickY)
			{
				m_FirstClickY = true;
				m_MouseClickDownY = my;
				m_OldViewportHeight = m_ViewportHeight;
			}
			int dragAmount = m_MouseClickDownY - my;

			m_ViewportY = my;
			m_ViewportHeight = m_OldViewportHeight + dragAmount;
		}
		if (mouseButtonReleased(m_Window, CHONPS_MOUSE_BUTTON_1))
		{
			m_FirstClickX = false;
			m_FirstClickY = false;
		}*/

		renderClear();
		renderClearColor(0.08f, 0.08f, 0.08f, 1.0f);

		m_Camera.SetDimensions(0.0f, static_cast<float>(m_Window->GetWidth()), 0.0f, static_cast<float>(m_Window->GetHeight()), -1.0f, 1.0f);
		m_Camera.UpdateMatrix();

		m_SceneCamera.SetDimensions(m_ViewportWidth, m_ViewportHeight);
		CameraMovment(m_Window, &m_SceneCamera, dt);
		m_SceneCamera.UpdateMatrix();

		renderBeginNextFrame();

		m_FBO->Resize(m_Window->GetWidth(), m_Window->GetHeight());
		m_FBO->Viewport(m_ViewportX, m_ViewportY, m_ViewportWidth, m_ViewportHeight);
		m_FBO->Begin();

		renderClear();
		renderClearColor(0.08f, 0.08f, 0.08f, 1.0f);

		UniformBufferObject UBO{};
		CameraMatrixUniformBuffer camUBO{};
		uint32_t drawCallCount = 0;

		m_CubemapShader->Bind();

		renderBindBufferSet(m_CubemapShader, m_SceneCameraUniformBuffer, 0);

		glm::mat4 view = glm::mat4(glm::mat3(m_SceneCamera.GetViewMatrix()));
		glm::mat4 projection = m_SceneCamera.GetProjectionMatrix();

		camUBO.camMatrix = projection * view;
		m_SceneCameraUniformBuffer->Bind(&camUBO, sizeof(camUBO), 0);

		m_Cubemap->Bind(m_CubemapShader);

		// Render Objects
		m_Shader->Bind();
		renderBindBufferSet(m_Shader, m_UniformBuffer, 0);


		UBO.camMatrix = m_SceneCamera.GetProjectionViewMatrix() * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		for (auto& mesh : m_Meshes)
		{
			UBO.texIndex = 0;
			m_UniformBuffer->Bind(&UBO, sizeof(UBO), sizeof(UBO) * drawCallCount);

			mesh.vertexArray->Bind();
			mesh.textures->Bind(m_Shader);
			renderDrawIndexedInstanced(mesh.vertexArray->GetIndexCount(), 1, drawCallCount);

			drawCallCount++;
		}

		m_FBO->End();

		renderPassBegin();

		renderClearColor(0.5f, 0.5f, 0.5f, 1.0f);

		m_FBOShader->Bind();

		renderBindBufferSet(m_FBOShader, m_CameraUniformBuffer, 0);

		camUBO.camMatrix = m_Camera.GetProjectionViewMatrix();

		m_CameraUniformBuffer->Bind(&camUBO, sizeof(camUBO), 0);

		m_FBO->Draw(m_FBOShader);

		gui::BeginDraw();

		camUBO.camMatrix = m_Camera.GetProjectionViewMatrix();
		m_CameraUniformBuffer->Bind(&camUBO, sizeof(camUBO), 0);
		renderBindBufferSet(m_PreStencilShader, m_CameraUniformBuffer, 0);

		float change = std::abs(std::sin(getTimeSeconds()));

		gui::DrawColor(0.9f, 0.3f, 0.7f, 0.8f);
		gui::DrawRect(50.0f, 260.0f, 2.0f, 120.0f);
		gui::DrawColor(0.2f, 0.3f, 0.7f, 0.5f);
		gui::DrawRect(100.0f, 200.0f, 600.0f, 400.0f);
		gui::DrawColor(0.7f, 0.2f, 0.4f, 0.8f);
		gui::DrawTriangle(100.0f, 100.0f, 300.0f, 100.0f, 200.0f, 200.0f);

		gui::SubmitDraw();

		renderPassEnd();
		renderDrawSubmit();
	}

	void ChonpsEditor::OnEvent(Event& e)
	{
		EventType eType = e.GetEventType();

		if (eType == EventType::WindowMoved || eType == EventType::WindowResize || eType == EventType::WindowFramebufferResize)
		{
			CHONPS_TRACE("({0}, {1})", m_Window->GetWidth(), m_Window->GetHeight());
			OnUpdate();
			m_Window->OnUpdate();
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&ChonpsEditor::MouseScrolled, this, std::placeholders::_1));
	}

	void ChonpsEditor::OnGuiRender()
	{
		float dt = Application::GetApp().GetTimestep() * 1000;

		float currentFrame = getTimeSeconds();
		m_FrameCount++;
		if (currentFrame - m_PreviousFrame >= 1.0f)
		{
			m_FPS = m_FrameCount;
			m_PreviousFrame = currentFrame;
			m_FrameCount = 0;
			CHONPS_TRACE("FPS: {0} | ms: {1}", m_FPS, dt);
		}

	}
}