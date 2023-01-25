#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Graphics/Texture.h"

namespace Chonps
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& filepath, TexType texType = TexType::Diffuse, TexFilterPair texFilter = { TexFilter::Linear, TexFilter::Nearest }, TexWrap texWrap = TexWrap::Repeat);
		OpenGLTexture(uint32_t width, uint32_t height, void* data, uint32_t size);

		virtual void TexUnit(Shader* shader, const char* uniform, uint32_t unit) override;

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
		uint32_t m_Unit = uint32_t();

		int m_Width;
		int m_Height;

		TexType m_TexType = TexType::NA;   // Tex Type
		TexFilterPair m_TexFilter; // Tex Filter
		TexWrap m_TexWrap;   // Tex Wrapper
	};
}

#endif