#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Graphics/Texture.h"

namespace Chonps
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& filepath);

		virtual void TexUnit(Shader* shader, const char* uniform, uint32_t unit) override;

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		virtual uint32_t GetID() const override { return m_ID; }
		virtual uint32_t GetUnit() const override { return m_Unit; }
		virtual TexType GetTexType() const override { return m_TexType; }

	private:
		uint32_t m_ID;
		uint32_t m_Unit;
		TexType m_TexType;
	};
}

#endif