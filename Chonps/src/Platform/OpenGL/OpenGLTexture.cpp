#include "cepch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Chonps
{
	OpenGLTexture::OpenGLTexture(const std::string& filepath, TexF texFilter /*= TexF::Linear*/, TexW texWrap /*= TexW::Repeat*/)
		: Texture(filepath), m_TexFilter(texFilter), m_TexWrap(texWrap)
	{
		int channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &channels, 0);
		stbi_set_flip_vertically_on_load(false);
		if (!data) CHONPS_CORE_ERROR("ERROR: TEXTURE: Failed to load texture data: {0}", filepath);

		// Get color channel format
		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (channels == 1)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RED;
		}
		CHONPS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

		// Get texture filtering format
		GLenum filterFormat = 0;
		if (m_TexFilter == TexF::Linear) filterFormat = GL_LINEAR;
		else if (m_TexFilter == TexF::Nearest) filterFormat = GL_NEAREST;
		CHONPS_CORE_ASSERT(filterFormat, "Format not supported!");

		// Get texture wrap mode format
		GLenum wrapFormat = 0;
		if (m_TexWrap == TexW::Repeat) wrapFormat = GL_REPEAT;
		else if (m_TexWrap == TexW::MirroredRepeat) wrapFormat = GL_MIRRORED_REPEAT;
		else if (m_TexWrap == TexW::ClampEdge) wrapFormat = GL_CLAMP_TO_EDGE;
		CHONPS_CORE_ASSERT(wrapFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, filterFormat);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, filterFormat);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, wrapFormat);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, wrapFormat);

		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	void OpenGLTexture::TexUnit(Shader* shader, const char* uniform, uint32_t unit)
	{
		glUniform1i(glGetUniformLocation(shader->GetID(), uniform), unit);
		m_Unit = unit;
	}

	void OpenGLTexture::SetTexType(TexT textureType)
	{
		m_TexType = textureType;
	}

	void OpenGLTexture::Bind(uint32_t unit) const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_ID);
		//glBindTextureUnit(slot, m_ID);
	}

	void OpenGLTexture::Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + m_Unit);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void OpenGLTexture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::Delete()
	{
		glDeleteTextures(1, &m_ID);
	}
}