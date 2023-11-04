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
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Cubemap:Texture, "createCubemapSp(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}
			case GraphicsContext::OpenGL: { return std::make_shared<OpenGLCubemap>(createInfo, setIndex); }
			case GraphicsContext::Vulkan: { return std::make_shared<VulkanCubemap>(createInfo, setIndex); }
			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Cubemap, "Could not create Cubemap!");
		return nullptr;
	}

	Cubemap* createCubemap(CubemapCreateInfo createInfo, uint32_t setIndex)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Cubemap:Texture, "createCubemapSp(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}
			case GraphicsContext::OpenGL: { return new OpenGLCubemap(createInfo, setIndex); }
			case GraphicsContext::Vulkan: { return new VulkanCubemap(createInfo, setIndex); }
			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Cubemap, "Could not create Cubemap!");
		return nullptr;
	}
}