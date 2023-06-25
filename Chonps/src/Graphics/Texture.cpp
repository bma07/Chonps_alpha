#include "cepch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/Vulkan/VulkanTexture.h"

namespace Chonps
{
	void textureBinding(uint32_t textureBinding, uint32_t samplerBinding)
	{
		GraphicsAPI GraphicsAPI = getGraphicsAPI();

		if (GraphicsAPI == GraphicsAPI::Vulkan)
			vkSpec::vkImplTextureBinding(textureBinding, samplerBinding);
	}

	void Texture::Binding(uint32_t textureBinding, uint32_t samplerBinding)
	{
		GraphicsAPI GraphicsAPI = getGraphicsAPI();

		if (GraphicsAPI == GraphicsAPI::Vulkan)
			vkSpec::vkImplTextureBinding(textureBinding, samplerBinding);
	}

	std::shared_ptr<Texture> createTextureSp(const std::string& filepath, TexType texType, TexFormat texFormat, TexFilterPair texFilter, TexWrap texWrap)
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
				return std::make_shared<OpenGLTexture>(filepath, texType, texFormat, texFilter, texWrap);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanTexture>(filepath, texType, texFormat, texFilter, texWrap);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}

	std::shared_ptr<Texture> createTextureSp(uint32_t width, uint32_t height, void* data, uint32_t size)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTextureSp(width, height, data, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return std::make_shared<OpenGLTexture>(width, height, data, size);
			}

			case GraphicsAPI::Vulkan:
			{
				return std::make_shared<VulkanTexture>(width, height, data, size);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}

	Texture* createTexture(const std::string& filepath, TexType texType, TexFormat texFormat, TexFilterPair texFilter, TexWrap texWrap)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTexture(filepath) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLTexture(filepath, texType, texFormat, texFilter, texWrap);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanTexture(filepath, texType, texFormat, texFilter, texWrap);
			}

			case GraphicsAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: TEXTURE: Could not create Texture!");
		return nullptr;
	}

	Texture* createTexture(uint32_t width, uint32_t height, void* data, uint32_t size)
	{
		switch (getGraphicsAPI())
		{
			case GraphicsAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createTexture(width, height, data, size) - No graphics API selected beforehand!");
				break;
			}

			case GraphicsAPI::OpenGL:
			{
				return new OpenGLTexture(width, height, data, size);
			}

			case GraphicsAPI::Vulkan:
			{
				return new VulkanTexture(width, height, data, size);
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