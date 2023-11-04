#include "cepch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/Vulkan/VulkanTexture.h"

namespace Chonps
{
	std::shared_ptr<Texture> createTextureSp(const std::string& filepath, 
		TexType texType, 
		TexFilterPair texFilter, 
		TexWrap texWrap)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Texture, "createTextureSp(filepath, texType, texFilter, texWrap) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL:
			{
				return std::make_shared<OpenGLTexture>(filepath, texType, texFilter, texWrap);
			}

			case GraphicsContext::Vulkan:
			{
				return std::make_shared<VulkanTexture>(filepath, texType, texFilter, texWrap);
			}

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Texture, "Could not create Texture!");
		return nullptr;
	}

	std::shared_ptr<Texture> createTextureSp(uint32_t width, uint32_t height, uint32_t channels, const void* data,
		TexType texType, 
		TexFilterPair texFilter, 
		TexWrap texWrap)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Texture, "createTextureSp(width, height, channels, data, texType, texFilter, texWrap) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL:
			{
				return std::make_shared<OpenGLTexture>(width, height, channels, data, texType, texFilter, texWrap);
			}

			case GraphicsContext::Vulkan:
			{
				return std::make_shared<VulkanTexture>(width, height, channels, data, texType, texFilter, texWrap);
			}

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Texture, "Could not create Texture!");
		return nullptr;
	}

	Texture* createTexture(const std::string& filepath, 
		TexType texType, 
		TexFilterPair texFilter, 
		TexWrap texWrap)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Texture, "createTexture(filepath, texType, texFilter, texWrap) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL:
			{
				return new OpenGLTexture(filepath, texType, texFilter, texWrap);
			}

			case GraphicsContext::Vulkan:
			{
				return new VulkanTexture(filepath, texType, texFilter, texWrap);
			}

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Texture, "Could not create Texture!");
		return nullptr;
	}

	Texture* createTexture(uint32_t width, uint32_t height, uint32_t channels, const void* data,
		TexType texType,
		TexFilterPair texFilter, 
		TexWrap texWrap)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Texture, "createTexture(width, height, channels, data, texType, texFilter, texWrap) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL:
			{
				return new OpenGLTexture(width, height, channels, data, texType, texFilter, texWrap);
			}

			case GraphicsContext::Vulkan:
			{
				return new VulkanTexture(width, height, channels, data, texType, texFilter, texWrap);
			}

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Texture, "Could not create Texture!");
		return nullptr;
	}

	TextureLayout* createTextureLayout(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Texture, "createTextureLayout(pTextures, textureCount, setIndex) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL:
			{
				return new OpenGLTextureLayout(pTextures, textureCount, setIndex);
			}

			case GraphicsContext::Vulkan:
			{
				return new VulkanTextureLayout(pTextures, textureCount, setIndex);
			}

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Texture, "Could not create Texture!");
		return nullptr;
	}

	std::shared_ptr<TextureLayout> createTextureLayoutSp(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex)
	{
		switch (getGraphicsContext())
		{
			case GraphicsContext::None:
			{
				CHONPS_CORE_LOG_WARN(Texture, "createTextureLayout(pTextures, textureCount) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsContext::OpenGL:
			{
				return std::make_shared<OpenGLTextureLayout>(pTextures, textureCount, setIndex);
			}

			case GraphicsContext::Vulkan:
			{
				return std::make_shared<VulkanTextureLayout>(pTextures, textureCount, setIndex);
			}

			case GraphicsContext::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_LOG_ERROR(Texture, "Could not create Texture!");
		return nullptr;
	}
}