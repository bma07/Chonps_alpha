#include "cepch.h"
#include "Shader.h"

#include "Renderer.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Chonps
{
	std::shared_ptr<Shader> createShaderSp(const std::string& vertex, const std::string& fragment, const std::string& name /*= Shader*/)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: SHADER: createShaderSp(vertex, fragment, name) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLShader>(name, vertex, fragment);
				break;
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanShader>(name, vertex, fragment);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: SHADER: Could not create Shader!");
		return nullptr;
	}

	std::shared_ptr<Shader> createShaderSp(const std::string& filepath)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: SHADER: createShaderSp(filepath) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLShader>(filepath);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanShader>(filepath);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: SHADER: Could not create Shader!");
		return nullptr;
	}

	Shader* createShader(const std::string& vertex, const std::string& fragment, const std::string& name /*= Shader*/)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: SHADER: createShader(vertex, fragment, name) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLShader(name, vertex, fragment);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanShader(name, vertex, fragment);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: SHADER: Could not create Shader!");
		return nullptr;
	}

	Shader* createShader(const std::string& filepath)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: SHADER: createShader(filepath) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLShader(filepath);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanShader(filepath);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: SHADER: Could not create Shader!");
		return nullptr;
	}
}