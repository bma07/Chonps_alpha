#ifndef HG_CHONPS_VULKAN_TEXTURE_H
#define HG_CHONPS_VULKAN_TEXTURE_H

#include "Graphics/Texture.h"
#include "VulkanRendererAPI.h"

namespace Chonps
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string& filepath, 
			TexType texType, 
			TexFilterPair texFilter, 
			TexWrap texWrap);

		VulkanTexture(uint32_t width, uint32_t height, const void* data, 
			TexType texType = TexType::Diffuse, 
			TexFilterPair texFilter = { TexFilter::Nearest, TexFilter::Nearest }, 
			TexWrap texWrap = TexWrap::Repeat);

		virtual void Bind(uint32_t unit) const override;
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual void TexUnit(uint32_t unit) override;

		virtual int GetWidth() const override { return m_Width; }
		virtual int GetHeight() const override { return m_Height; }

		virtual uint32_t id() const override { return m_ID; };
		virtual uint32_t unit() const override { return m_Unit; }
		virtual TexType GetTexType() const { return m_TexType; }

	private:
		uint32_t m_ID, m_Unit = 0;
		int m_Width, m_Height;
		TexType m_TexType = TexType::None;
		TexFilterPair m_TexFilter;
		TexWrap m_TexWrap;

		VkImage m_TextureImage;
		VmaAllocation m_TextureImageMemory;
		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler;

		bool m_Deleted = false;
	};
}


#endif