#include "cepch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Chonps
{
	Texture* createTexture(const std::string& filepath, TexF texFilter /*= TexF::Linear*/, TexW texWrap /*= TexW::Repeat*/)
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
				return new OpenGLTexture(filepath, texFilter, texWrap);
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
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Shader!");
		return nullptr;
	}
}