#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Graphics/Texture.h"

namespace Chonps
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& filepath, TexType texType, TexFormat texFormat, TexFilterPair texFilter, TexWrap texWrap);
		OpenGLTexture(uint32_t width, uint32_t height, void* data, uint32_t size);

		virtual void Bind(uint32_t unit) const override;
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual void TexUnit(Shader* shader, const char* uniform, uint32_t unit) override;
		virtual void TexUnit(uint32_t unit) override { m_Unit = unit; }

		virtual int GetWidth() const override { return m_Width; }
		virtual int GetHeight() const override { return m_Height; }

		virtual uint32_t GetID() const override { return m_ID; }
		virtual uint32_t GetUnit() const override { return m_Unit; }
		virtual TexType GetTexType() const override { return m_TexType; }

	private:
		uint32_t m_ID, m_Unit;
		int m_Width, m_Height;

		TexType m_TexType = TexType::NA;
		TexFormat m_TexFormat;
		TexFilterPair m_TexFilter;
		TexWrap m_TexWrap;
	};
}

#endif