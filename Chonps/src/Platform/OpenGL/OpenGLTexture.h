#ifndef HG_CHONPS_OPENGL_TEXTURE_H
#define HG_CHONPS_OPENGL_TEXTURE_H

#include "Graphics/Texture.h"

namespace Chonps
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& filepath,
			TexType texType, 
			TexFilterPair texFilter, 
			TexWrap texWrap);

		OpenGLTexture(uint32_t width, uint32_t height, const void* data,
			TexType texType = TexType::Diffuse, 
			TexFilterPair texFilter = { TexFilter::Nearest, TexFilter::Nearest }, 
			TexWrap texWrap = TexWrap::Repeat);

		virtual void Delete() override;

		virtual int GetWidth() const override { return m_Width; }
		virtual int GetHeight() const override { return m_Height; }

		virtual uint32_t id() override { return m_ID; }
		virtual TexType GetTexType() const override { return m_TexType; }

		uint32_t* getNativeID() { return &m_ID; }
	private:
		uint32_t m_ID;
		int m_Width, m_Height;

		TexType m_TexType = TexType::None;
		TexFilterPair m_TexFilter;
		TexWrap m_TexWrap;
	};

	class OpenGLTextureLayout : public TextureLayout
	{
	public:
		OpenGLTextureLayout(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex);

		virtual void insert(Texture* texture, uint32_t slot) override;
		virtual void erase(uint32_t slot) override;

		virtual void Bind(Shader* shader) const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual std::map<uint32_t, Texture*> GetTextures() override { return m_Textures; }

	private:
		std::map<uint32_t, Texture*> m_Textures;
		std::vector<int32_t> m_TextureSlots;
	};
}

#endif