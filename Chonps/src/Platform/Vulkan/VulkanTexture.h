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

		virtual void Delete() override;

		virtual int GetWidth() const override { return m_Width; }
		virtual int GetHeight() const override { return m_Height; }

		virtual uint32_t id() override { return m_ID; };
		virtual TexType GetTexType() const { return m_TexType; }

		VulkanTextureData* getNativeTextureData() { return &m_TexData; }

	private:
		uint32_t m_ID;
		int m_Width, m_Height;
		TexType m_TexType = TexType::None;
		TexFilterPair m_TexFilter;
		TexWrap m_TexWrap;

		VulkanTextureData m_TexData;

		bool m_Deleted = false;
	};

	class VulkanTextureLayout : public TextureLayout
	{
	public:
		VulkanTextureLayout(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex);

		virtual void insert(Texture* texture, uint32_t slot) override;
		virtual void erase(uint32_t slot) override;

		virtual void Bind(Shader* shader) const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual std::map<uint32_t, Texture*> GetTextures() override { return m_Textures; }

	private:
		uint32_t m_SetIndex;
		std::map<uint32_t, Texture*> m_Textures;
		std::vector<VkDescriptorSet> m_TextureDescriptorSet;
		std::vector<VkDescriptorImageInfo> m_ImageInfo;
	};
}


#endif