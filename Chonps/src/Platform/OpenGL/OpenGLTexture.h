#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Graphics/Texture.h"

namespace Chonps
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& filepath, TexF texFilter = TexF::Linear, TexW texWrap = TexW::Repeat);

		virtual void TexUnit(Shader* shader, const char* uniform, uint32_t unit) override;
		virtual void SetTexType(TexT textureType) override;

		virtual void Bind(uint32_t unit) const override;
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual int GetWidth() const override { return m_Width; }
		virtual int GetHeight() const override { return m_Height; }

		virtual uint32_t GetID() const override { return m_ID; }
		virtual uint32_t GetUnit() const override { return m_Unit; }
		virtual TexType GetTexType() const override { return m_TexType; }

	private:
		uint32_t m_ID;
		uint32_t m_Unit;

		int m_Width;
		int m_Height;

		TexT m_TexType = TexT::NA;   // Tex Type
		TexF m_TexFilter; // Tex Filter
		TexW m_TexWrap;   // Tex Wrapper
	};
}

#endif