#include "cepch.h"
#include "Shader.h"

#include "Graphics/RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Chonps
{
	Shader* createShader(const std::string& vertex, const std::string& fragment, const std::string& name /*= Shader*/)
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: SHADER: createShader(vertex, fragment, name) - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLShader(name, vertex, fragment);
				break;
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
		CHONPS_CORE_ERROR("ERROR: SHADER: Could not create Shader!");
		return nullptr;
	}

	Shader* createShader(const std::string& filepath)
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: SHADER: createShader(filepath) - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLShader(filepath);
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
		CHONPS_CORE_ERROR("ERROR: SHADER: Could not create Shader!");
		return nullptr;
	}

}