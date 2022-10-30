#include "cepch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Chonps
{
	Texture* createTexture(const std::string& filepath, TexT texType /*= TexT::Diffuse*/, TexF texFilter /*= TexF::Linear*/, TexW texWrap /*= TexW::Repeat*/)
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTexture(filepath) - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLTexture(filepath, texType, texFilter, texWrap);
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
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}

	Texture* createTexture(uint32_t width, uint32_t height, void* data, uint32_t size)
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTexture(width, height, data, size) - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLTexture(width, height, data, size);
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
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}
}