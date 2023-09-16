#include "cepch.h"
#include "Cubemap.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLCubemap.h"
#include "Platform/Vulkan/VulkanCubemap.h"

namespace Chonps
{
	const char* Cubemap::m_CubemapProjectionUniformName = "cubemapProj";
	const char* Cubemap::m_CubemapViewUniformName = "cubemapView";

	std::shared_ptr<Cubemap> createCubemapSp(CubemapCreateInfo createInfo, uint32_t setIndex)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(Cubemap:Texture, "createCubemapSp(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}
			case GraphicsAPI::OpenGL: { return std::make_shared<OpenGLCubemap>(createInfo, setIndex); }
			case GraphicsAPI::Vulkan: { return std::make_shared<VulkanCubemap>(createInfo, setIndex); }
			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Cubemap, "Could not create Cubemap!");
		return nullptr;
	}

	Cubemap* createCubemap(CubemapCreateInfo createInfo, uint32_t setIndex)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_LOG_WARN(Cubemap:Texture, "createCubemapSp(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}
			case GraphicsAPI::OpenGL: { return new OpenGLCubemap(createInfo, setIndex); }
			case GraphicsAPI::Vulkan: { return new VulkanCubemap(createInfo, setIndex); }
			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Cubemap, "Could not create Cubemap!");
		return nullptr;
	}
}