#include "cepch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "Graphics/RendererBackends.h"

namespace Chonps
{
	void getDataFormats(GLenum& dataFormat, GLenum& internalFormat, TexFormat texFormat, TexType texType, uint32_t channels, bool gammaCorrect)
	{
		switch (texFormat)
		{
			case Chonps::TexFormat::None:
			{
				switch (channels)
				{
				case 1:
					internalFormat = GL_RED;
					dataFormat = GL_RED;
					break;
				case 3:
					gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB8;
					dataFormat = GL_RGB;
					break;
				case 4:
					gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
					dataFormat = GL_RGBA;
					break;
				default:
					CHONPS_CORE_LOG_ERROR(OpenGL:Texture, "Unable to find Texture Format with given channel: {0}", channels);
					break;
				}
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
				CHONPS_CORE_LOG_WARN(OpenGL:Texture, "RGB64F format is unsupported on OpenGL! Reverting texture to RGB8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGB64I:
			{
				CHONPS_CORE_LOG_WARN(OpenGL:Texture, "RGB64I format is unsupported on OpenGL! Reverting texture to RGB8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8 : internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			case Chonps::TexFormat::RGB64UI:
			{
				CHONPS_CORE_LOG_WARN(OpenGL:Texture, "RGB64UI format is unsupported on OpenGL! Reverting texture to RGB8 format.");
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
				CHONPS_CORE_LOG_WARN(OpenGL:Texture, "RGBA64F format is unsupported on OpenGL! Reverting texture to RGBA8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
			case Chonps::TexFormat::RGBA64I:
			{
				CHONPS_CORE_LOG_WARN(OpenGL:Texture, "RGBA64I format is unsupported on OpenGL! Reverting texture to RGBA8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
			case Chonps::TexFormat::RGBA64UI:
			{
				CHONPS_CORE_LOG_WARN(OpenGL:Texture, "RGBA64UI format is unsupported on OpenGL! Reverting texture to RGBA8 format.");
				gammaCorrect && (texType != TexType::Specular && texType != TexType::Normal) ? internalFormat = GL_SRGB8_ALPHA8 : internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
		}
		CHONPS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");
	}

	OpenGLTexture::OpenGLTexture(const std::string& filepath , TexType texType, TexFilterPair texFilter, TexWrap texWrap)
		: Texture(filepath, texType, texFilter, texWrap), m_TexType(texType), m_TexFilter(texFilter), m_TexWrap(texWrap)
	{
		int channels;
		stbi_uc* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &channels, STBI_rgb_alpha);
		if (!data) CHONPS_CORE_LOG_ERROR(OpenGL:Texture, "Failed to load texture data: {0}", filepath);

		bool gammaCorrect = renderGetGammaCorrection();

		// Get color channel format
		GLenum internalFormat = GL_RGBA8, dataFormat = GL_RGBA;
		getDataFormats(dataFormat, internalFormat, TexFormat::RGBA8, texType, channels, gammaCorrect);

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
		default: CHONPS_CORE_LOG_WARN(OpenGL:Texture, "Use of texture filter unsupported in mag filter\n Mag filter only accepts Linear or Nearest"); break;
		}

		// Get texture wrap mode format
		GLenum wrapFormat = GL_REPEAT;
		if (m_TexWrap == TexWrap::Repeat) wrapFormat = GL_REPEAT;
		else if (m_TexWrap == TexWrap::MirroredRepeat) wrapFormat = GL_MIRRORED_REPEAT;
		else if (m_TexWrap == TexWrap::ClampToEdge) wrapFormat = GL_CLAMP_TO_EDGE;
		CHONPS_CORE_ASSERT(wrapFormat, "Format not supported!");

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, magFilter);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, wrapFormat);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, wrapFormat);

		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, uint32_t channels, const void* data, TexType texType, TexFilterPair texFilter, TexWrap texWrap)
		: Texture(width, height, channels, data, texType, texFilter, texWrap),
		m_Width(width), m_Height(height),
		m_TexType(texType),
		m_TexFilter(texFilter),
		m_TexWrap(texWrap)
	{
		bool gammaCorrect = renderGetGammaCorrection();

		// Get color channel format
		GLenum internalFormat = GL_RGBA8, dataFormat = GL_RGBA;
		getDataFormats(dataFormat, internalFormat, TexFormat::Auto, texType, channels, gammaCorrect);

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
		default: CHONPS_CORE_LOG_WARN(OpenGL:Texture, "Use of texture filter unsupported in mag filter\n Mag filter only accepts Linear or Nearest"); break;
		}
		// Get texture wrap mode format
		GLenum wrapFormat = GL_REPEAT;
		if (m_TexWrap == TexWrap::Repeat) wrapFormat = GL_REPEAT;
		else if (m_TexWrap == TexWrap::MirroredRepeat) wrapFormat = GL_MIRRORED_REPEAT;
		else if (m_TexWrap == TexWrap::ClampToEdge) wrapFormat = GL_CLAMP_TO_EDGE;
		CHONPS_CORE_ASSERT(wrapFormat, "Format not supported!");

		CHONPS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFormat);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFormat);
	}

	void OpenGLTexture::Delete()
	{
		glDeleteTextures(1, &m_ID);
	}

	OpenGLTextureLayout::OpenGLTextureLayout(TextureCreateInfo* pTextures, uint32_t textureCount, uint32_t setIndex)
		: TextureLayout(pTextures, textureCount, setIndex)
	{
		RendererBackends* backends = getRendererBackends();

		CHONPS_CORE_ASSERT(textureCount < backends->maxTextureBindingSlots, "cannot have more textures than the max texture binding slot!");

		for (uint32_t i = 0; i < textureCount; i++)
		{
			TextureCreateInfo textureInfo = pTextures[i];
			
			CHONPS_CORE_ASSERT(textureInfo.slot < backends->maxTextureBindingSlots, "cannot have texture slot that is above the max texture binding slot!");

			if (m_Textures.find(textureInfo.slot) == m_Textures.end())
			{
				m_Textures[textureInfo.slot] = std::move(textureInfo.texture);
			}
			else CHONPS_CORE_LOG_WARN(OpenGL:TextureLayout, "Texture with slot {0} already exists! Ignoring second texture slot", textureInfo.slot);
		}

		for (uint32_t i = 0; i < backends->maxTextureBindingSlots; i++)
			m_TextureSlots.push_back(i);
	}

	void OpenGLTextureLayout::insert(Texture* texture, uint32_t slot)
	{
		if (m_Textures.find(slot) == m_Textures.end())
		{
			m_Textures[slot] = std::move(texture);
		}
		else CHONPS_CORE_LOG_WARN(OpenGL:TextureLayout, "Texture with slot {0} already exists! Ignoring second texture slot", slot);
	}

	void OpenGLTextureLayout::erase(uint32_t slot)
	{
		if (m_Textures.find(slot) == m_Textures.end())
			CHONPS_CORE_LOG_WARN(OpenGL:TextureLayout, "No texture was found at slot {0}! texture cannot be removed because it does not exist!", slot);
		else
			m_Textures.erase(slot);
	}

	void OpenGLTextureLayout::Bind(Shader* shader) const
	{
		glUniform1iv(glGetUniformLocation(shader->id(), getRendererBackends()->ogls.textureLayoutUniformName), m_Textures.size(), m_TextureSlots.data());

		for (auto& texture : m_Textures)
		{
			glBindTextureUnit(texture.first, texture.second->id());
		}
	}

	void OpenGLTextureLayout::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTextureLayout::Delete()
	{
		for (auto& texture : m_Textures)
		{
			OpenGLTexture* oglTexture = static_cast<OpenGLTexture*>(texture.second);
			glDeleteTextures(1, oglTexture->getNativeID());
		}
	}
}