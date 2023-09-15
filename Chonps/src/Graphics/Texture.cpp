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
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTextureSp(filepath, texType, texFilter, texWrap) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLTexture>(filepath, texType, texFilter, texWrap);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanTexture>(filepath, texType, texFilter, texWrap);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}

	std::shared_ptr<Texture> createTextureSp(uint32_t width, uint32_t height, const void* data, 
		TexType texType, 
		TexFilterPair texFilter, 
		TexWrap texWrap)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTextureSp(width, height, data, texType, texFilter, texWrap) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLTexture>(width, height, data, texType, texFilter, texWrap);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanTexture>(width, height, data, texType, texFilter, texWrap);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}

	Texture* createTexture(const std::string& filepath, 
		TexType texType, 
		TexFilterPair texFilter, 
		TexWrap texWrap)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTexture(filepath, texType, texFilter, texWrap) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLTexture(filepath, texType, texFilter, texWrap);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanTexture(filepath, texType, texFilter, texWrap);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}

	Texture* createTexture(uint32_t width, uint32_t height, const void* data, 
		TexType texType,
		TexFilterPair texFilter, 
		TexWrap texWrap)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTexture(width, height, data, texType, texFilter, texWrap) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLTexture(width, height, data, texType, texFilter, texWrap);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanTexture(width, height, data, texType, texFilter, texWrap);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}

	TextureLayout* createTextureLayout(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTextureLayout(pTextures, textureCount, setIndex) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLTextureLayout(pTextures, textureCount, setIndex);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanTextureLayout(pTextures, textureCount, setIndex);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}

	std::shared_ptr<TextureLayout> createTextureLayoutSp(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTextureLayout(pTextures, textureCount) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLTextureLayout>(pTextures, textureCount, setIndex);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanTextureLayout>(pTextures, textureCount, setIndex);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}
}