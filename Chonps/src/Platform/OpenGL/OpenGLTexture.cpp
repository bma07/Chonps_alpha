#include "cepch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "Graphics/RendererBackends.h"

namespace Chonps
{
	OpenGLTexture::OpenGLTexture(const std::string& filepath, TexType texType /*= TexT::Diffuse*/, TexFilterPair texFilter /*= { TexFilter::Linear, TexFilter::Nearest }*/, TexWrap texWrap /*= TexW::Repeat*/)
		: Texture(filepath, texType, texFilter, texWrap), m_TexType(texType), m_TexFilter(texFilter), m_TexWrap(texWrap)
	{
		int channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &channels, 0);
		stbi_set_flip_vertically_on_load(false);
		if (!data) CHONPS_CORE_ERROR("ERROR: TEXTURE: Failed to load texture data: {0}", filepath);

		bool gammaCorrect = renderGetGammaCorrection();

		// Get color channel format
		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			gammaCorrect && (m_TexType != TexT::Specular && m_TexType != TexT::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			gammaCorrect && (m_TexType != TexT::Specular && m_TexType != TexT::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (channels == 1)
		{
			gammaCorrect && (m_TexType != TexT::Specular && m_TexType != TexT::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB8;
			dataFormat = GL_RED;
		}
		CHONPS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

		// Get texture filter mode
		GLenum magFilter = GL_NEAREST, minFilter = GL_LINEAR;
		switch (m_TexFilter.min)
		{
			case TexFilter::Linear: magFilter = GL_LINEAR; break;
			case TexFilter::Nearest: magFilter = GL_NEAREST; break;
			case TexFilter::Nearest_Mipmap_Nearest: magFilter = GL_NEAREST_MIPMAP_NEAREST; break;
			case TexFilter::Nearest_Mipmap_Linear: magFilter = GL_NEAREST_MIPMAP_LINEAR; break;
			case TexFilter::Linear_Mipmap_Nearest: magFilter = GL_LINEAR_MIPMAP_NEAREST; break;
			case TexFilter::Linear_Mipmap_Linear: magFilter = GL_LINEAR_MIPMAP_LINEAR; break;
		}
		switch (m_TexFilter.mag)
		{
			case TexFilter::Linear: magFilter = GL_LINEAR; break;
			case TexFilter::Nearest: magFilter = GL_NEAREST; break;
			default: CHONPS_CORE_WARN("WARNING: TEXTURE: Use of texture filter unsupported in mag filter\n Mag filter only accepts Linear or Nearest"); break;
		}

		// Get texture wrap mode format
		GLenum wrapFormat = GL_REPEAT;
		if (m_TexWrap == TexW::Repeat) wrapFormat = GL_REPEAT;
		else if (m_TexWrap == TexW::MirroredRepeat) wrapFormat = GL_MIRRORED_REPEAT;
		else if (m_TexWrap == TexW::ClampEdge) wrapFormat = GL_CLAMP_TO_EDGE;
		CHONPS_CORE_ASSERT(wrapFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, magFilter);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, wrapFormat);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, wrapFormat);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, void* data, uint32_t size)
		: Texture(width, height, data, size), m_Width(width), m_Height(height)
	{
		bool gammaCorrect = renderGetGammaCorrection();

		// Get color channel format
		GLenum internalFormat = 0, dataFormat = 0;
		gammaCorrect ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;

		CHONPS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		m_TexType = TexType::Diffuse;
		m_TexFilter = { TexFilter::Linear, TexFilter::Linear };
		m_TexWrap = TexWrap::Repeat;
	}

	void OpenGLTexture::TexUnit(Shader* shader, const char* uniform, uint32_t unit)
	{
		glUniform1i(glGetUniformLocation(shader->GetID(), uniform), unit);
		m_Unit = unit;
	}

	void OpenGLTexture::Bind(uint32_t unit) const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_ID);
		//glBindTextureUnit(unit, m_ID);
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