#include "cepch.h"
#include "RendererAPI.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Chonps
{
	static API s_API = API::None;

	bool renderInit(API api /*= API::OpenGL*/)
	{
		s_API = api;
		setRendererAPI();

		if (api == API::None)
		{
			CHONPS_CORE_WARN("No graphics API selected!");
			return false;
		}
		CHONPS_CORE_INFO("Rendering Context Initialized: {0}", getGraphicsContextName());
		return true;
	}

	API getGraphicsContext()
	{
		return s_API;
	}

	std::string getGraphicsContextName()
	{
		switch (getGraphicsContext())
		{
			case Chonps::API::None: return "None";
			case Chonps::API::OpenGL: return "OpenGL";
			case Chonps::API::Vulkan: return "Vulkan";
			case Chonps::API::DirectX: return "DirectX";
		}

		CHONPS_CORE_ERROR("Cannot find the graphics API selected!");
		return "null";
	}

	RendererAPI* createRendererAPI()
	{
		switch (getGraphicsContext())
		{
		case API::None:
		{
			CHONPS_CORE_WARN("WANRING: RENDERER_API: createRendererAPI() - No graphics API selected beforehand!");
			break;
		}

		case API::OpenGL:
		{
			return new OpenGLRendererAPI();
		}

		case API::Vulkan:
		{
			break;
		}

		case API::DirectX:
		{
			break;
		}
		}
		CHONPS_CORE_ERROR("ERROR: RENDERER_API: Could not get rendering API!");
		return nullptr;
	}


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