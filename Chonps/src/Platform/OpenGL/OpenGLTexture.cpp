#include "cepch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "Graphics/RendererBackends.h"

namespace Chonps
{
	void getDataFormats(GLenum& dataFormat, GLenum& internalFormat, TexFormat texFormat, TexType texType, bool gammaCorrect)
	{
		switch (texFormat)
		{
			case Chonps::TexFormat::None:
			{
				CHONPS_CORE_ERROR("ERROR: TEXTURE: Unable to find Texture Format!");
				break;
			}
			case Chonps::TexFormat::RGB8:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGB16:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB16;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGB32F:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB32F;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGB32I:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB32I;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGB32UI:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB32UI;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGB64F:
			{
				CHONPS_CORE_WARN("WARNING: TEXTURE: RGB64F format is unsupported on OpenGL! Reverting texture to RGB8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGB64I:
			{
				CHONPS_CORE_WARN("WARNING: TEXTURE: RGB64I format is unsupported on OpenGL! Reverting texture to RGB8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGB64UI:
			{
				CHONPS_CORE_WARN("WARNING: TEXTURE: RGB64UI format is unsupported on OpenGL! Reverting texture to RGB8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGBA8:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
			case Chonps::TexFormat::RGBA16:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA16;
				dataFormat = GL_RGBA;
				break;
			}
			case Chonps::TexFormat::RGBA32F:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA32F;
				dataFormat = GL_RGBA;
				break;
			}
			case Chonps::TexFormat::RGBA32I:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA32I;
				dataFormat = GL_RGBA;
				break;
			}
			case Chonps::TexFormat::RGBA32UI:
			{
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA32UI;
				dataFormat = GL_RGBA;
				break;
			}
			case Chonps::TexFormat::RGBA64F:
			{
				CHONPS_CORE_WARN("WARNING: TEXTURE: RGBA64F format is unsupported on OpenGL! Reverting texture to RGBA8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
			case Chonps::TexFormat::RGBA64I:
			{
				CHONPS_CORE_WARN("WARNING: TEXTURE: RGBA64I format is unsupported on OpenGL! Reverting texture to RGBA8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
			case Chonps::TexFormat::RGBA64UI:
			{
				CHONPS_CORE_WARN("WARNING: TEXTURE: RGBA64UI format is unsupported on OpenGL! Reverting texture to RGBA8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
		}
		CHONPS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");
	}

	OpenGLTexture::OpenGLTexture(const std::string& filepath, TexType texType, TexFilterPair texFilter, TexWrap texWrap)
		: Texture(filepath, texType, texFilter, texWrap), m_TexType(texType), m_TexFilter(texFilter), m_TexWrap(texWrap), m_Unit(0)
	{
		int channels;
		stbi_uc* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &channels, STBI_rgb_alpha);
		if (!data) CHONPS_CORE_ERROR("ERROR: TEXTURE: Failed to load texture data: {0}", filepath);

		bool gammaCorrect = renderGetGammaCorrection();

		// Get color channel format
		GLenum internalFormat = GL_RGBA8, dataFormat = GL_RGBA;
		getDataFormats(dataFormat, internalFormat, TexFormat::RGBA8, texType, gammaCorrect);

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
		if (m_TexWrap == TexWrap::Repeat) wrapFormat = GL_REPEAT;
		else if (m_TexWrap == TexWrap::MirroredRepeat) wrapFormat = GL_MIRRORED_REPEAT;
		else if (m_TexWrap == TexWrap::ClampToEdge) wrapFormat = GL_CLAMP_TO_EDGE;
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

	OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, const void* data, TexType texType, TexFilterPair texFilter, TexWrap texWrap)
		: Texture(width, height, data, texType, texFilter, texWrap), 
		m_Width(width), m_Height(height), 
		m_TexType(texType), 
		m_TexFilter(texFilter), 
		m_TexWrap(texWrap), 
		m_Unit(0)
	{
		bool gammaCorrect = renderGetGammaCorrection();

		// Get color channel format
		GLenum internalFormat = GL_RGBA8, dataFormat = GL_RGBA;
		getDataFormats(dataFormat, internalFormat, TexFormat::RGBA8, texType, gammaCorrect);

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
		if (m_TexWrap == TexWrap::Repeat) wrapFormat = GL_REPEAT;
		else if (m_TexWrap == TexWrap::MirroredRepeat) wrapFormat = GL_MIRRORED_REPEAT;
		else if (m_TexWrap == TexWrap::ClampToEdge) wrapFormat = GL_CLAMP_TO_EDGE;
		CHONPS_CORE_ASSERT(wrapFormat, "Format not supported!");

		CHONPS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, internalFormat, width, height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, magFilter);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, wrapFormat);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, wrapFormat);

		glTextureSubImage2D(m_ID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

		m_TexType = TexType::Diffuse;
		m_TexFilter = { TexFilter::Nearest, TexFilter::Nearest };
		m_TexWrap = TexWrap::Repeat;
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