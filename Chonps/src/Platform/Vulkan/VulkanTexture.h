#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H

#include "Graphics/Texture.h"
#include <vulkan/vulkan.h>

namespace Chonps
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string& filepath, TexType texType, TexFormat texFormat, TexFilterPair texFilter, TexWrap texWrap);
		VulkanTexture(uint32_t width, uint32_t height, void* data, uint32_t size);

		virtual void Bind(uint32_t unit) const override;
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual void TexUnit(Shader* shader, const char* uniform, uint32_t unit) override;
		virtual void TexUnit(uint32_t unit) override;

		virtual int GetWidth() const override { return m_Width; }
		virtual int GetHeight() const override { return m_Height; }

		virtual uint32_t GetID() const override { return m_ID; };
		virtual uint32_t GetUnit() const override { return m_Unit; }
		virtual TexType GetTexType() const { return m_TexType; }

	private:
		uint32_t m_ID, m_Unit = 0;
		int m_Width, m_Height;
		TexType m_TexType = TexType::NA;
		TexFormat m_TexFormat;
		TexFilterPair m_TexFilter;
		TexWrap m_TexWrap;

		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;
		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler;
	};

	namespace vkSpec
	{
		void vkImplTextureBinding(uint32_t textureBinding, uint32_t samplerBinding);
	}
}


#endif