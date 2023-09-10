#include "cepch.h"
#include "Cubemap.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLCubemap.h"
#include "Platform/Vulkan/VulkanCubemap.h"

namespace Chonps
{
	const char* Cubemap::m_CubemapProjectionUniformName = "cubemapProj";
	const char* Cubemap::m_CubemapViewUniformName = "cubemapView";

	std::shared_ptr<Cubemap> createCubemapSp(CubemapCreateInfo createInfo)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createCubemapSp(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}
			case GraphicsAPI::OpenGL: { return std::make_shared<OpenGLCubemap>(createInfo); }
			case GraphicsAPI::Vulkan: { return std::make_shared<VulkanCubemap>(createInfo); }
			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: CUBEMAP: Could not create Cubemap!");
		return nullptr;
	}

	Cubemap* createCubemap(CubemapCreateInfo createInfo)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createCubemapSp(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}
			case GraphicsAPI::OpenGL: { return new OpenGLCubemap(createInfo); }
			case GraphicsAPI::Vulkan: { return new VulkanCubemap(createInfo); }
			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: CUBEMAP: Could not create Cubemap!");
		return nullptr;
	}
}