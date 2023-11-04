#include "cepch.h"
#include "OpenGLShader.h"

#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Chonps
{
	static PipelineSpecification s_StandardPipelineSpecification;

	GLenum get_shader_type(std::string type)
	{
		std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c) { return std::tolower(c); });
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		CHONPS_CORE_LOG_ERROR(OpenGL:Shader, "Unkown shader type: {0}", type);
		return 0;
	}

	ColorBlendAttachment getColorBlendAttachment()
	{
		ColorBlendAttachment colorAttachment{};
		colorAttachment.blendEnable = true;
		colorAttachment.colorWriteMask = { true, true, true, true };
		colorAttachment.colorBlendOp = ColorBlendOperation::Add;
		colorAttachment.srcColorBlendFactor = ColorBlendFactor::SrcAlpha;
		colorAttachment.dstColorBlendFactor = ColorBlendFactor::OneMinusSrcAlpha;
		colorAttachment.alphaBlendOp = ColorBlendOperation::Add;
		colorAttachment.srcAlphaBlendFactor = ColorBlendFactor::SrcAlpha;
		colorAttachment.dstAlphaBlendFactor = ColorBlendFactor::OneMinusSrcAlpha;

		return colorAttachment;
	}

	void ogls::initStandardOglPipelineSpecification()
	{
		// Input Assembly
		s_StandardPipelineSpecification.inputAssembly.topology = RenderTopologyType::Triangle;
		s_StandardPipelineSpecification.inputAssembly.primitiveRestartEnable = false;

		// Viewport
		s_StandardPipelineSpecification.viewport.x = 0.0f;
		s_StandardPipelineSpecification.viewport.y = 0.0f;
		s_StandardPipelineSpecification.viewport.width = 0.0f;
		s_StandardPipelineSpecification.viewport.height = 0.0f;
		s_StandardPipelineSpecification.viewport.minDepth = 0.0f;
		s_StandardPipelineSpecification.viewport.maxDepth = 1.0f;

		// Scissor
		s_StandardPipelineSpecification.viewport.scissorsOffset = { 0, 0 };

		// Rasterizer
		s_StandardPipelineSpecification.rasterizer.depthClampEnable = false;
		s_StandardPipelineSpecification.rasterizer.rasterizerDiscardEnable = false;
		s_StandardPipelineSpecification.rasterizer.lineWidth = 1.0f;
		s_StandardPipelineSpecification.rasterizer.cullMode = RenderCullFaceMode::Back;
		s_StandardPipelineSpecification.rasterizer.frontFace = RenderCullFrontFace::CounterClockwise;
		s_StandardPipelineSpecification.rasterizer.depthBiasEnable = false;
		s_StandardPipelineSpecification.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		s_StandardPipelineSpecification.rasterizer.depthBiasClamp = 0.0f; // Optional
		s_StandardPipelineSpecification.rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		// MultiSampling
		s_StandardPipelineSpecification.multisampling.sampleShadingEnable = false;
		s_StandardPipelineSpecification.multisampling.rasterizationSamples = Sample::SampleCount_1_Bit;
		s_StandardPipelineSpecification.multisampling.minSampleShading = 1.0f; // Optional
		s_StandardPipelineSpecification.multisampling.sampleMask = nullptr; // Optional
		s_StandardPipelineSpecification.multisampling.alphaToCoverageEnable = false; // Optional
		s_StandardPipelineSpecification.multisampling.alphaToOneEnable = false; // Optional

		// Color Attachments
		s_StandardPipelineSpecification.colorBlend.colorBlendAttachmentCount = 0;
		s_StandardPipelineSpecification.colorBlend.pColorBlendAttachments = nullptr;

		// Color Blend
		s_StandardPipelineSpecification.colorBlend.logicOpEnable = false;
		s_StandardPipelineSpecification.colorBlend.blendConstants[0] = 0.0f; // Optional
		s_StandardPipelineSpecification.colorBlend.blendConstants[1] = 0.0f; // Optional
		s_StandardPipelineSpecification.colorBlend.blendConstants[2] = 0.0f; // Optional
		s_StandardPipelineSpecification.colorBlend.blendConstants[3] = 0.0f; // Optional

		// Depth Stencil
		s_StandardPipelineSpecification.depthstencil.enableDepth = false;
		s_StandardPipelineSpecification.depthstencil.depthOpCompare = CompareOperation::Never;
		s_StandardPipelineSpecification.depthstencil.enableStencil = false;
		s_StandardPipelineSpecification.depthstencil.stencil.compareMask = 0x00;
		s_StandardPipelineSpecification.depthstencil.stencil.writeMask = 0x00;
		s_StandardPipelineSpecification.depthstencil.stencil.reference = 0;
		s_StandardPipelineSpecification.depthstencil.stencil.compareOp = CompareOperation::Never;
		s_StandardPipelineSpecification.depthstencil.stencil.depthFailOp = StencilOperation::Keep;
		s_StandardPipelineSpecification.depthstencil.stencil.failOp = StencilOperation::Keep;
		s_StandardPipelineSpecification.depthstencil.stencil.passOp = StencilOperation::Keep;
	}

	void compileErrors(unsigned int shader, GLenum type, const char* shaderFile = " ")
	{
		GLint hasCompiled;
		char infoLog[1024];
		if (type != GL_PROGRAM)
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				std::string shaderType;
				if (type == GL_VERTEX_SHADER)
					shaderType = "VERTEX";
				else if (type == GL_FRAGMENT_SHADER)
					shaderType = "FRAGMENT";
				glGetShaderInfoLog(shader, 1024, 0, infoLog);
				CHONPS_CORE_LOG_ERROR(OpenGL:Shader, "{0}: {1} - SHADER_COMPILATION_ERROR!\nInfolog: {2}", shaderType, shaderFile, infoLog);
				glDeleteShader(shader);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 1024, 0, infoLog);
				CHONPS_CORE_LOG_ERROR(OpenGL:Shader, "{0}: {1} - SHADER_LINKING_ERROR!\nInfolog: {2}", "PROGRAM", shaderFile, infoLog);
				glDeleteProgram(shader);
			}
		}
	}

	OpenGLShader::OpenGLShader(const std::string& vertex, const std::string& fragment, PipelineLayoutInfo* pipelineInfo)
		: Shader(vertex, fragment, pipelineInfo)
	{
		const char* vertexSource = vertex.c_str();
		const char* fragmentSource = fragment.c_str();

		// Create Vertex Shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, 0);
		glCompileShader(vertexShader);
		compileErrors(vertexShader, GL_VERTEX_SHADER, vertex.c_str());

		// Create Fragment Shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, 0);
		glCompileShader(fragmentShader);
		compileErrors(fragmentShader, GL_FRAGMENT_SHADER, fragment.c_str());

		// Create Shader Program
		m_ID = glCreateProgram();

		// Bind Shaders
		glAttachShader(m_ID, vertexShader);
		glAttachShader(m_ID, fragmentShader);
		glLinkProgram(m_ID);
		compileErrors(m_ID, GL_PROGRAM);

		glDetachShader(m_ID, vertexShader);
		glDetachShader(m_ID, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		BindPipeline(pipelineInfo);
	}

	void OpenGLShader::Bind() const
	{
		if (m_Pipeline.enableCullMode)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(m_Pipeline.cullmode);
			glFrontFace(m_Pipeline.cullModeFrontFace);
		}
		else
			glDisable(GL_CULL_FACE);

		if (m_Pipeline.enableBlending)
		{
			glEnable(GL_BLEND);
			glBlendFunc(m_Pipeline.srcBlendFactor, m_Pipeline.dstBlendFactor);
		}
		else
			glDisable(GL_BLEND);

		if (m_Pipeline.enableDepthTest)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(m_Pipeline.depthCompare);
		}
		else
			glDisable(GL_DEPTH_TEST);

		if (m_Pipeline.enableStencilTest)
		{
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(m_Pipeline.stencilCompareOp, m_Pipeline.stencilReference, m_Pipeline.stencilCompareMask);
			glStencilOp(m_Pipeline.stencilFailOp, m_Pipeline.stencilDepthFailOp, m_Pipeline.stencilPassOp);
			glStencilMask(m_Pipeline.stencilWriteMask);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
			glStencilMask(0x00);
		}

		glUseProgram(m_ID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::Delete() const
	{
		glDeleteProgram(m_ID);
	}

	void OpenGLShader::BindPipeline(PipelineLayoutInfo* pipelineLayout)
	{
		m_LayoutInfo = *pipelineLayout;

		PipelineSpecification pipelineSpec;
		if (pipelineLayout->pipelineSpecification == nullptr)
			pipelineSpec = s_StandardPipelineSpecification;
		else
			pipelineSpec = *pipelineLayout->pipelineSpecification;

		// Blending
		if (pipelineSpec.colorBlend.colorBlendAttachmentCount > 0)
		{
			ColorBlendAttachment colorBlendAttachment = pipelineSpec.colorBlend.pColorBlendAttachments[0];
			m_Pipeline.enableBlending = true;
			m_Pipeline.srcBlendFactor = ogls::getBlendFactorType(colorBlendAttachment.srcColorBlendFactor);
			m_Pipeline.dstBlendFactor = ogls::getBlendFactorType(colorBlendAttachment.dstColorBlendFactor);
		}
		else if(getRendererBackends()->enableColorBlend)
		{
			ColorBlendAttachment colorBlendAttachment = getColorBlendAttachment();
			m_Pipeline.enableBlending = true;
			m_Pipeline.srcBlendFactor = ogls::getBlendFactorType(colorBlendAttachment.srcColorBlendFactor);
			m_Pipeline.dstBlendFactor = ogls::getBlendFactorType(colorBlendAttachment.dstColorBlendFactor);
		}

		// Cull Face
		m_Pipeline.cullmode = ogls::getPipelineCullFaceMode(pipelineSpec.rasterizer.cullMode);
		if (pipelineSpec.rasterizer.cullMode != CullFaceMode::Disable)
			m_Pipeline.enableCullMode = true;

		m_Pipeline.cullModeFrontFace = ogls::getPipelineFrontFace(pipelineSpec.rasterizer.frontFace);

		// Depth Stencil
		m_Pipeline.enableDepthTest = pipelineSpec.depthstencil.enableDepth;
		m_Pipeline.depthCompare = ogls::getPipelineCompareOp(pipelineSpec.depthstencil.depthOpCompare);
		m_Pipeline.enableStencilTest = pipelineSpec.depthstencil.enableStencil;
		m_Pipeline.stencilCompareOp = ogls::getPipelineCompareOp(pipelineSpec.depthstencil.stencil.compareOp);
		m_Pipeline.stencilCompareMask = pipelineSpec.depthstencil.stencil.compareMask;
		m_Pipeline.stencilWriteMask = pipelineSpec.depthstencil.stencil.writeMask;
		m_Pipeline.stencilReference = pipelineSpec.depthstencil.stencil.reference;
		m_Pipeline.stencilPassOp = ogls::getPipelineStencilOp(pipelineSpec.depthstencil.stencil.passOp);
		m_Pipeline.stencilFailOp = ogls::getPipelineStencilOp(pipelineSpec.depthstencil.stencil.failOp);
		m_Pipeline.stencilDepthFailOp = ogls::getPipelineStencilOp(pipelineSpec.depthstencil.stencil.depthFailOp);
	}

	namespace ogls
	{
		PipelineSpecification getStandardOglPipelineSpecification()
		{
			return s_StandardPipelineSpecification;
		}

		void setStandardOglPipelineSpecification(PipelineSpecification pipelineSpecification)
		{
			s_StandardPipelineSpecification = pipelineSpecification;
		}

		void oglImplUploadUniform1f(uint32_t shader, const char* uniform, float x)
		{
			glUniform1f(glGetUniformLocation(shader, uniform), x);
		}

		void oglImplUploadUniform2f(uint32_t shader, const char* uniform, float x, float y)
		{
			glUniform2f(glGetUniformLocation(shader, uniform), x, y);
		}

		void oglImplUploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z)
		{
			glUniform3f(glGetUniformLocation(shader, uniform), x, y, z);
		}

		void oglImplUploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(shader, uniform), x, y, z, w);
		}

		void oglImplUploadUniform1i(uint32_t shader, const char* uniform, int x)
		{
			glUniform1i(glGetUniformLocation(shader, uniform), x);
		}

		void oglImplUploadUniform2i(uint32_t shader, const char* uniform, int x, int y)
		{
			glUniform2i(glGetUniformLocation(shader, uniform), x, y);
		}

		void oglImplUploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z)
		{
			glUniform3i(glGetUniformLocation(shader, uniform), x, y, z);
		}

		void oglImplUploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w)
		{
			glUniform4i(glGetUniformLocation(shader, uniform), x, y, z, w);
		}

		void oglImplUploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x)
		{
			glUniform1ui(glGetUniformLocation(shader, uniform), x);
		}

		void oglImplUploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y)
		{
			glUniform2ui(glGetUniformLocation(shader, uniform), x, y);
		}

		void oglImplUploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z)
		{
			glUniform3ui(glGetUniformLocation(shader, uniform), x, y, z);
		}

		void oglImplUploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w)
		{
			glUniform4ui(glGetUniformLocation(shader, uniform), x, y, z, w);
		}

		void oglImplUploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
		{
			glUniform1fv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
		{
			glUniform2fv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
		{
			glUniform3fv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
		{
			glUniform4fv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
		{
			glUniform1iv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
		{
			glUniform2iv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
		{
			glUniform3iv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
		{
			glUniform4iv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
		{
			glUniform1uiv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
		{
			glUniform2uiv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
		{
			glUniform3uiv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
		{
			glUniform4uiv(glGetUniformLocation(shader, uniform), count, v);
		}

		void oglImplUploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			glUniformMatrix2fv(glGetUniformLocation(shader, uniform), count, transpose, v);
		}

		void oglImplUploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			glUniformMatrix3fv(glGetUniformLocation(shader, uniform), count, transpose, v);
		}

		void oglImplUploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			glUniformMatrix4fv(glGetUniformLocation(shader, uniform), count, transpose, v);
		}

		void oglImplUploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			glUniformMatrix2x3fv(glGetUniformLocation(shader, uniform), count, transpose, v);
		}

		void oglImplUploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			glUniformMatrix3x2fv(glGetUniformLocation(shader, uniform), count, transpose, v);
		}

		void oglImplUploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			glUniformMatrix2x4fv(glGetUniformLocation(shader, uniform), count, transpose, v);
		}

		void oglImplUploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			glUniformMatrix4x2fv(glGetUniformLocation(shader, uniform), count, transpose, v);
		}

		void oglImplUploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			glUniformMatrix3x4fv(glGetUniformLocation(shader, uniform), count, transpose, v);
		}

		void oglImplUploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			glUniformMatrix4x3fv(glGetUniformLocation(shader, uniform), count, transpose, v);
		}
	}
}