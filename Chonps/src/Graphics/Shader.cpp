#include "cepch.h"
#include "Shader.h"

#include "Renderer.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Chonps
{
	Shader* createShader(const std::string& vertex, const std::string& fragment, const std::string& name /*= Shader*/)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: SHADER: createShader(vertex, fragment, name) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLShader(name, vertex, fragment);
				break;
			}

			case RenderAPI::Vulkan:
			{
				break;
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: SHADER: Could not create Shader!");
		return nullptr;
	}

	Shader* createShader(const std::string& filepath)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: SHADER: createShader(filepath) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLShader(filepath);
			}

			case RenderAPI::Vulkan:
			{
				break;
			}

			case RenderAPI::DirectX:
				{
					break;
				}
			}
		CHONPS_CORE_ERROR("ERROR: SHADER: Could not create Shader!");
		return nullptr;
	}

	// Uniform Upload functions - corresponding symbol with type:
	void uploadUniform1f(uint32_t shader, const char* uniform, float x)
	{
		getRendererAPI()->UploadUniform1f(shader, uniform, x);
	}

	void uploadUniform2f(uint32_t shader, const char* uniform, float x, float y)
	{
		getRendererAPI()->UploadUniform2f(shader, uniform, x, y);
	}

	void uploadUniform3f(uint32_t shader, const char* uniform, float x, float y, float z)
	{
		getRendererAPI()->UploadUniform3f(shader, uniform, x, y, z);
	}

	void uploadUniform4f(uint32_t shader, const char* uniform, float x, float y, float z, float w)
	{
		getRendererAPI()->UploadUniform4f(shader, uniform, x, y, z, w);
	}

	void uploadUniform1i(uint32_t shader, const char* uniform, int x)
	{
		getRendererAPI()->UploadUniform1i(shader, uniform, x);
	}

	void uploadUniform2i(uint32_t shader, const char* uniform, int x, int y)
	{
		getRendererAPI()->UploadUniform2i(shader, uniform, x, y);
	}

	void uploadUniform3i(uint32_t shader, const char* uniform, int x, int y, int z)
	{
		getRendererAPI()->UploadUniform3i(shader, uniform, x, y, z);
	}

	void uploadUniform4i(uint32_t shader, const char* uniform, int x, int y, int z, int w)
	{
		getRendererAPI()->UploadUniform4i(shader, uniform, x, y, z, w);
	}

	void uploadUniform1ui(uint32_t shader, const char* uniform, unsigned int x)
	{
		getRendererAPI()->UploadUniform1ui(shader, uniform, x);
	}

	void uploadUniform2ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y)
	{
		getRendererAPI()->UploadUniform2ui(shader, uniform, x, y);
	}

	void uploadUniform3ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z)
	{
		getRendererAPI()->UploadUniform3ui(shader, uniform, x, y, z);
	}

	void uploadUniform4ui(uint32_t shader, const char* uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w)
	{
		getRendererAPI()->UploadUniform4ui(shader, uniform, x, y, z, w);
	}

	void uploadUniform1fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{
		getRendererAPI()->UploadUniform1fv(shader, uniform, count, v);
	}

	void uploadUniform2fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{
		getRendererAPI()->UploadUniform2fv(shader, uniform, count, v);
	}

	void uploadUniform3fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{
		getRendererAPI()->UploadUniform3fv(shader, uniform, count, v);
	}

	void uploadUniform4fv(uint32_t shader, const char* uniform, unsigned int count, const float* v)
	{
		getRendererAPI()->UploadUniform4fv(shader, uniform, count, v);
	}

	void uploadUniform1iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{
		getRendererAPI()->UploadUniform1iv(shader, uniform, count, v);
	}

	void uploadUniform2iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{
		getRendererAPI()->UploadUniform2iv(shader, uniform, count, v);
	}

	void uploadUniform3iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{
		getRendererAPI()->UploadUniform3iv(shader, uniform, count, v);
	}

	void uploadUniform4iv(uint32_t shader, const char* uniform, unsigned int count, const int* v)
	{
		getRendererAPI()->UploadUniform4iv(shader, uniform, count, v);
	}

	void uploadUniform1uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{
		getRendererAPI()->UploadUniform1uiv(shader, uniform, count, v);
	}

	void uploadUniform2uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{
		getRendererAPI()->UploadUniform2uiv(shader, uniform, count, v);
	}

	void uploadUniform3uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{
		getRendererAPI()->UploadUniform3uiv(shader, uniform, count, v);
	}

	void uploadUniform4uiv(uint32_t shader, const char* uniform, unsigned int count, const unsigned int* v)
	{
		getRendererAPI()->UploadUniform4uiv(shader, uniform, count, v);
	}

	void uploadUniform2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		getRendererAPI()->UploadUniform2mfv(shader, uniform, count, transpose, v);
	}

	void uploadUniform3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		getRendererAPI()->UploadUniform3mfv(shader, uniform, count, transpose, v);
	}

	void uploadUniform4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		getRendererAPI()->UploadUniform4mfv(shader, uniform, count, transpose, v);
	}

	void uploadUniform2x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		getRendererAPI()->UploadUniform2x3mfv(shader, uniform, count, transpose, v);
	}

	void uploadUniform3x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		getRendererAPI()->UploadUniform3x2mfv(shader, uniform, count, transpose, v);
	}

	void uploadUniform2x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		getRendererAPI()->UploadUniform2x4mfv(shader, uniform, count, transpose, v);
	}

	void uploadUniform4x2mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		getRendererAPI()->UploadUniform4x2mfv(shader, uniform, count, transpose, v);
	}

	void uploadUniform3x4mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		getRendererAPI()->UploadUniform3x4mfv(shader, uniform, count, transpose, v);
	}

	void uploadUniform4x3mfv(uint32_t shader, const char* uniform, unsigned int count, bool transpose, const float* v)
	{
		getRendererAPI()->UploadUniform4x3mfv(shader, uniform, count, transpose, v);
	}
}