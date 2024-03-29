#include "cepch.h"
#include "Shader.h"

#include "Renderer.h"
#include "RendererAPI.h"
#include "Core/File.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Chonps
{
	Shader* createShader(const std::string& vertexFile, const std::string& fragmentFile, PipelineLayoutInfo* pipelineInfo)
	{
		std::string vertexSrc = get_file_contents(vertexFile.c_str());
		std::string fragmentSrc = get_file_contents(fragmentFile.c_str());

		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Shader, "createShader(vertex, fragment, pipelineInfo) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return new OpenGLShader(vertexSrc, fragmentSrc, pipelineInfo); }

			case GraphicsContext::Vulkan: { return new VulkanShader(vertexSrc, fragmentSrc, pipelineInfo); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Shader, "Could not create Shader!");
		return nullptr;
	}

	std::shared_ptr<Shader> createShaderSp(const std::string& vertexFile, const std::string& fragmentFile, PipelineLayoutInfo* pipelineInfo)
	{
		std::string vertexSrc = get_file_contents(vertexFile.c_str());
		std::string fragmentSrc = get_file_contents(fragmentFile.c_str());

		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Shader, "createShaderSp(vertex, fragment, pipelineInfo) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return std::make_shared<OpenGLShader>(vertexSrc, fragmentSrc, pipelineInfo); }

			case GraphicsContext::Vulkan: { return std::make_shared<VulkanShader>(vertexSrc, fragmentSrc, pipelineInfo); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Shader, "Could not create Shader!");
		return nullptr;
	}

	Shader* createShaderSrc(const std::string& vertexSrc, const std::string& fragmentSrc, PipelineLayoutInfo* pipelineInfo)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Shader, "createShader(vertex, fragment, pipelineInfo) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return new OpenGLShader(vertexSrc, fragmentSrc, pipelineInfo); }

			case GraphicsContext::Vulkan: { return new VulkanShader(vertexSrc, fragmentSrc, pipelineInfo); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Shader, "Could not create Shader!");
		return nullptr;
	}

	std::shared_ptr<Shader> createShaderSrcSp(const std::string& vertexSrc, const std::string& fragmentSrc, PipelineLayoutInfo* pipelineInfo)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Shader, "createShaderSp(vertex, fragment, pipelineInfo) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL: { return std::make_shared<OpenGLShader>(vertexSrc, fragmentSrc, pipelineInfo); }

			case GraphicsContext::Vulkan: { return std::make_shared<VulkanShader>(vertexSrc, fragmentSrc, pipelineInfo); }

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Shader, "Could not create Shader!");
		return nullptr;
	}

	namespace vks
	{
		void setUsePushConstant(bool use)
		{
			vks::vkImplSetUsePushConstant(use);
		}

		void setPushConstantRange(PushConstantRange* pushConstant, uint32_t count)
		{
			vks::vkImplSetPushConstantRange(pushConstant, count);
		}

		void renderPushConstant(Shader* shader, ShaderStage shaderStage, uint32_t size, uint32_t offset, void* pValues)
		{
			vks::vkImplRenderPushConstant(shader, shaderStage, size, offset, pValues);
		}
	}

	namespace ogls
	{
		void uploadUniform1f(uint32_t shader, const char* uniform, float x)
		{
			oglImplUploadUniform1f(shader, uniform, x);
		}

		void uploadUniform2f(uint32_t shader, const char* uniform, float x, float y)
		{
			oglImplUploadUniform2f(shader, uniform, x, y);
		}

		void uploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z)
		{
			oglImplUploadUniform3f(shader, uniform, x, y, z);
		}

		void uploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w)
		{
			oglImplUploadUniform4f(shader, uniform, x, y, z, w);
		}

		void uploadUniform1i(uint32_t shader, const char* uniform, int x)
		{
			oglImplUploadUniform1i(shader, uniform, x);
		}

		void uploadUniform2i(uint32_t shader, const char* uniform, int x, int y)
		{
			oglImplUploadUniform2i(shader, uniform, x, y);
		}

		void uploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z)
		{
			oglImplUploadUniform3i(shader, uniform, x, y, z);
		}

		void uploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w)
		{
			oglImplUploadUniform4i(shader, uniform, x, y, z, w);
		}

		void uploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x)
		{
			oglImplUploadUniform1ui(shader, uniform, x);
		}

		void uploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y)
		{
			oglImplUploadUniform2ui(shader, uniform, x, y);
		}

		void uploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z)
		{
			oglImplUploadUniform3ui(shader, uniform, x, y, z);
		}

		void uploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w)
		{
			oglImplUploadUniform4ui(shader, uniform, x, y, z, w);
		}

		void uploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
		{
			oglImplUploadUniform1fv(shader, uniform, count, v);
		}

		void uploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
		{
			oglImplUploadUniform2fv(shader, uniform, count, v);
		}

		void uploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
		{
			oglImplUploadUniform3fv(shader, uniform, count, v);
		}

		void uploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
		{
			oglImplUploadUniform4fv(shader, uniform, count, v);
		}

		void uploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
		{
			oglImplUploadUniform1iv(shader, uniform, count, v);
		}

		void uploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
		{
			oglImplUploadUniform2iv(shader, uniform, count, v);
		}

		void uploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
		{
			oglImplUploadUniform3iv(shader, uniform, count, v);
		}

		void uploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
		{
			oglImplUploadUniform4iv(shader, uniform, count, v);
		}

		void uploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
		{
			oglImplUploadUniform1uiv(shader, uniform, count, v);
		}

		void uploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
		{
			oglImplUploadUniform2uiv(shader, uniform, count, v);
		}

		void uploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
		{
			oglImplUploadUniform3uiv(shader, uniform, count, v);
		}

		void uploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
		{
			oglImplUploadUniform4uiv(shader, uniform, count, v);
		}

		void uploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			oglImplUploadUniform2mfv(shader, uniform, count, transpose, v);
		}

		void uploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			oglImplUploadUniform3mfv(shader, uniform, count, transpose, v);
		}

		void uploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			oglImplUploadUniform4mfv(shader, uniform, count, transpose, v);
		}

		void uploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			oglImplUploadUniform2x3mfv(shader, uniform, count, transpose, v);
		}

		void uploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			oglImplUploadUniform3x2mfv(shader, uniform, count, transpose, v);
		}

		void uploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			oglImplUploadUniform2x4mfv(shader, uniform, count, transpose, v);
		}

		void uploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			oglImplUploadUniform4x2mfv(shader, uniform, count, transpose, v);
		}

		void uploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			oglImplUploadUniform3x4mfv(shader, uniform, count, transpose, v);
		}

		void uploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
		{
			oglImplUploadUniform4x3mfv(shader, uniform, count, transpose, v);
		}
	}
}