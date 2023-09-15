#include "cepch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Chonps
{
	float screenOpenGLImpl[] =
	{
		// Coords    // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
	};

	GLenum getColorFormat(FrameBufferColorFormat texFormat)
	{
		switch (texFormat)
		{
			case FrameBufferColorFormat::None:
			{
				return GL_RGBA8;
			}
			case FrameBufferColorFormat::RGB:
			{
				return GL_RGB;
			}
			case FrameBufferColorFormat::RGBA:
			{
				return GL_RGBA;
			}
			case FrameBufferColorFormat::RGBA8:
			{
				return GL_RGBA8;
			}
			case FrameBufferColorFormat::RGBA16F:
			{
				return GL_RGBA16F;
			}
			case FrameBufferColorFormat::RGBA32F:
			{
				return GL_RGBA32F;
			}
			case FrameBufferColorFormat::RED_INT:
			{
				return GL_R32I;
			}
			default:
			{
				CHONPS_CORE_WARN("WARNING: FRAMEBUFFER: No options matched FrameBufferColorFormat that are available for color format, defaulting to standard texture format!");
				return GL_RGBA8;
			}
		}
	}

	GLenum getDataFormat(FrameBufferColorFormat texFormat)
	{
		switch (texFormat)
		{
			case FrameBufferColorFormat::None:
			{
				return GL_RGBA;
			}
			case FrameBufferColorFormat::RGB:
			{
				return GL_RGB;
			}
			case FrameBufferColorFormat::RGBA:
			{
				return GL_RGBA;
			}
			case FrameBufferColorFormat::RGBA8:
			{
				return GL_RGBA;
			}
			case FrameBufferColorFormat::RGBA16F:
			{
				return GL_RGBA;
			}
			case FrameBufferColorFormat::RGBA32F:
			{
				return GL_RGBA;
			}
			case FrameBufferColorFormat::RED_INT:
			{
				return GL_RGBA;
			}
			default:
			{
				CHONPS_CORE_WARN("WARNING: FRAMEBUFFER: No options matched FrameBufferColorFormat that are available for color format, defaulting to standard texture format!");
				return GL_RGBA;
			}
		}
	}

	GLenum Multisampled(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	GLenum GetTexWrap(TexWrap textureWrap)
	{
		switch (textureWrap)
		{
			case TexWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
			case TexWrap::Repeat: return GL_REPEAT;
			case TexWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
			case TexWrap::Default: return GL_CLAMP_TO_EDGE;
		}

		return 0;
	}

	GLenum GetTexFilter(TexFilter textureFilter)
	{
		switch (textureFilter)
		{
			case TexFilter::Linear: return GL_LINEAR;
			case TexFilter::Nearest: return GL_NEAREST;
			default: 
			{
				CHONPS_CORE_WARN("WARNING: FRAMEBUFFER: Cannot create texture with unsupported texture filter\nFramebuffer only supports Linear or Nearest");
				return GL_NEAREST;
			}
		}

		return 0;
	}

	uint32_t getSampleCount(Sample samples)
	{
		switch (samples)
		{
			case Sample::SampleCount_1_Bit: { return 1; }
			case Sample::SampleCount_2_Bit: { return 2; }
			case Sample::SampleCount_4_Bit: { return 4; }
			case Sample::SampleCount_8_Bit: { return 8; }
			case Sample::SampleCount_16_Bit: { return 16; }
			case Sample::SampleCount_32_Bit: { return 32; }
			case Sample::SampleCount_64_Bit: { return 64; }
			case Sample::SampleCount_Max_Bit: { return 0x7FFFFFFF; }
			default:
			{
				CHONPS_CORE_WARN("WARNING: FRAMEBUFFER: Could not find sample value, returning default sample value!");
				return 1;
			}
		}
	}

	void CreateTextures(bool multisampled, uint32_t* TexID, uint32_t count)
	{
		glCreateTextures(Multisampled(multisampled), count, TexID);
	}

	void AttachColorTexture(uint32_t id, bool multisampled, FrameBufferSpecificationInfo specification, GLenum internalFormat, GLenum dataFormat, int index)
	{
		if (multisampled)
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, getSampleCount(specification.samples), internalFormat, specification.width, specification.height, GL_TRUE);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, specification.width, specification.height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);

		GLenum texType = Multisampled(multisampled);

		glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GetTexFilter(specification.textureFilter));
		glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GetTexFilter(specification.textureFilter));
		glTexParameteri(texType, GL_TEXTURE_WRAP_R, GetTexWrap(specification.textureWrap));
		glTexParameteri(texType, GL_TEXTURE_WRAP_S, GetTexWrap(specification.textureWrap));
		glTexParameteri(texType, GL_TEXTURE_WRAP_T, GetTexWrap(specification.textureWrap));

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texType, id, 0);
	}

	void AttachDepthTexture(uint32_t id, bool multisampled, FrameBufferSpecificationInfo specification, GLenum format, GLenum attachmentType)
	{
		if (multisampled)
			glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, getSampleCount(specification.samples), format, specification.width, specification.height, GL_TRUE);
		else
			glTexStorage2D(GL_TEXTURE_2D, 1, format, specification.width, specification.height);

		GLenum texType = Multisampled(multisampled);

		glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GetTexFilter(specification.textureFilter));
		glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GetTexFilter(specification.textureFilter));
		glTexParameteri(texType, GL_TEXTURE_WRAP_R, GetTexWrap(specification.textureWrap));
		glTexParameteri(texType, GL_TEXTURE_WRAP_S, GetTexWrap(specification.textureWrap));
		glTexParameteri(texType, GL_TEXTURE_WRAP_T, GetTexWrap(specification.textureWrap));

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, texType, id, 0);
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(FrameBufferSpecificationInfo frameBufferSpecificationInfo, uint32_t setIndex)
		: FrameBuffer(frameBufferSpecificationInfo, setIndex)
	{
		m_FrameBufferSpecificationInfo = frameBufferSpecificationInfo;

		m_ColorAttachmentSpecifications = std::vector<FrameBufferColorAttachment>(m_FrameBufferSpecificationInfo.colorAttachments, m_FrameBufferSpecificationInfo.colorAttachments + m_FrameBufferSpecificationInfo.colorAttachmentCount);
		m_DepthAttachmentSpecification = *m_FrameBufferSpecificationInfo.depthAttachment;

		m_VPx = 0;
		m_VPy = 0;
		m_VPWidth = frameBufferSpecificationInfo.width;
		m_VPHeight = frameBufferSpecificationInfo.height;

		UpdateFrameBuffer();

		uint32_t screenVBO;
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &screenVBO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenOpenGLImpl), &screenOpenGLImpl, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &screenVBO);
	}

	void OpenGLFrameBuffer::UpdateFrameBuffer()
	{
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		m_MultiSampled = m_FrameBufferSpecificationInfo.samples != Sample::SampleCount_1_Bit;

		// Attachments
		if (!m_ColorAttachmentSpecifications.empty())
		{
			m_PreColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			CreateTextures(m_MultiSampled, m_PreColorAttachments.data(), static_cast<uint32_t>(m_PreColorAttachments.size()));

			for (uint32_t i = 0; i < m_PreColorAttachments.size(); i++)
			{
				glBindTexture(Multisampled(m_MultiSampled), m_PreColorAttachments[i]);

				FrameBufferColorAttachment colorAttachment = m_ColorAttachmentSpecifications[i];
				GLenum colorFormat = getColorFormat(colorAttachment.format);
				GLenum dataFormat = getDataFormat(colorAttachment.format);

				AttachColorTexture(m_PreColorAttachments[i], m_MultiSampled, m_FrameBufferSpecificationInfo, colorFormat, dataFormat, i);
			}
		}

		if (m_DepthAttachmentSpecification.format != FrameBufferDepthFormat::None)
		{
			CreateTextures(m_MultiSampled, &m_PreDepthAttachment, 1);
			glBindTexture(Multisampled(m_MultiSampled), m_PreDepthAttachment);
			
			switch (m_DepthAttachmentSpecification.format)
			{
				case FrameBufferDepthFormat::DEPTH24STENCIL8:
				{
					AttachDepthTexture(m_PreDepthAttachment, m_MultiSampled, m_FrameBufferSpecificationInfo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
					break;
				}
				case FrameBufferDepthFormat::DEPTH_COMPONENT:
				{
					AttachDepthTexture(m_PreDepthAttachment, m_MultiSampled, m_FrameBufferSpecificationInfo, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
					break;
				}
			}
		}

		if (!m_MultiSampled)
		{
			if (m_PreColorAttachments.size() > 1)
			{
				CHONPS_CORE_ASSERT(m_PreColorAttachments.size() <= 4, "Amount of Color Attachments more than four");
				GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glDrawBuffers(static_cast<GLsizei>(m_PreColorAttachments.size()), buffers);
			}
			else if (m_PreColorAttachments.empty())
			{
				// Only depth-pass
				glDrawBuffer(GL_NONE);
			}
		}

		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			CHONPS_CORE_ERROR("ERROR: FRAMEBUFFER: FrameBuffer is not complete! FrameBuffer error: {0}", status);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (m_MultiSampled)
		{
			glGenFramebuffers(1, &m_PostFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, m_PostFBO);

			m_ColorAttachments.resize(m_PreColorAttachments.size());
			CreateTextures(false, m_ColorAttachments.data(), static_cast<uint32_t>(m_ColorAttachments.size()));

			for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);

				FrameBufferColorAttachment colorAttachment = m_ColorAttachmentSpecifications[i];
				GLenum colorFormat = getColorFormat(colorAttachment.format);
				GLenum dataFormat = getDataFormat(colorAttachment.format);

				AttachColorTexture(m_ColorAttachments[i], false, m_FrameBufferSpecificationInfo, colorFormat, dataFormat, i);
			}

			if (m_DepthAttachmentSpecification.format != FrameBufferDepthFormat::None)
			{
				CreateTextures(false, &m_DepthAttachment, 1);
				glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
				switch (m_DepthAttachmentSpecification.format)
				{
					case FrameBufferDepthFormat::DEPTH24STENCIL8:
					{
						AttachDepthTexture(m_DepthAttachment, false, m_FrameBufferSpecificationInfo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
						break;
					}
					case FrameBufferDepthFormat::DEPTH_COMPONENT:
					{
						AttachDepthTexture(m_DepthAttachment, false, m_FrameBufferSpecificationInfo, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
						break;
					}
				}
			}

			if (m_ColorAttachments.size() > 1)
			{
				CHONPS_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Amount of Color Attachments more than four");
				GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glDrawBuffers(static_cast<GLsizei>(m_ColorAttachments.size()), buffers);
			}
			else if (m_ColorAttachments.empty())
			{
				// Only depth-pass
				glDrawBuffer(GL_NONE);
			}

			// Error checking framebuffer
			auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
				CHONPS_CORE_ERROR("ERROR: FRAMEBUFFER: FrameBuffer is not complete! Post-Processing Framebuffer error: {0}", status);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			m_ColorAttachments = m_PreColorAttachments;
			m_DepthAttachment = m_PreDepthAttachment;
		}
	}

	void OpenGLFrameBuffer::Draw(Shader* shader, uint32_t index /*= 0*/)
	{
		glBindVertexArray(m_VAO);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[index]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (m_FrameBufferSpecificationInfo.width != width || m_FrameBufferSpecificationInfo.height != height)
		{
			m_FrameBufferSpecificationInfo.width = width;
			m_FrameBufferSpecificationInfo.height = height;

			glDeleteFramebuffers(1, &m_FBO);
			if (m_MultiSampled) glDeleteFramebuffers(1, &m_PostFBO);
			glDeleteTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());
			glDeleteTextures(static_cast<GLsizei>(m_PreColorAttachments.size()), m_PreColorAttachments.data());
			glDeleteTextures(1, &m_PreDepthAttachment);
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_PreColorAttachments.clear();
			m_DepthAttachment = 0;
			m_PreDepthAttachment = 0;

			UpdateFrameBuffer();
			Viewport(0, 0, width, height);
		}
	}

	void OpenGLFrameBuffer::Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		m_VPx = x;
		m_VPy = y;
		m_VPWidth = width < m_FrameBufferSpecificationInfo.width ? width : m_FrameBufferSpecificationInfo.width;
		m_VPHeight = height < m_FrameBufferSpecificationInfo.height ? height : m_FrameBufferSpecificationInfo.height;
	}

	void OpenGLFrameBuffer::Bind()
	{
		glViewport(m_VPx, m_VPy, m_VPWidth, m_VPHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	void OpenGLFrameBuffer::Unbind()
	{
		if (m_MultiSampled)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_PostFBO);
			glBlitFramebuffer(0, 0, m_FrameBufferSpecificationInfo.width, m_FrameBufferSpecificationInfo.height, 0, 0, m_FrameBufferSpecificationInfo.width, m_FrameBufferSpecificationInfo.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Begin()
	{
		glViewport(m_VPx, m_VPy, m_VPWidth, m_VPHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	void OpenGLFrameBuffer::End()
	{
		if (m_MultiSampled)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_PostFBO);
			glBlitFramebuffer(0, 0, m_FrameBufferSpecificationInfo.width, m_FrameBufferSpecificationInfo.height, 0, 0, m_FrameBufferSpecificationInfo.width, m_FrameBufferSpecificationInfo.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::BindTex(uint32_t slot /*= 0*/)
	{
		glBindTexture(Multisampled(m_FrameBufferSpecificationInfo.samples != Sample::SampleCount_1_Bit), m_ColorAttachments[slot]);
	}

	void OpenGLFrameBuffer::Delete()
	{
		glDeleteFramebuffers(1, &m_FBO);
		if (m_MultiSampled) glDeleteFramebuffers(1, &m_PostFBO);
		glDeleteTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());
		glDeleteTextures(static_cast<GLsizei>(m_PreColorAttachments.size()), m_PreColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
		glDeleteVertexArrays(1, &m_VAO);
	}

	uint32_t OpenGLFrameBuffer::GetTexID(uint32_t index /*= 0*/) const
	{
		if (index < m_ColorAttachments.size()) return m_ColorAttachments[index];
		CHONPS_CORE_ERROR("ERROR: FBO: Texture ID index out of bounds! ID must be within range: {0} | Index given: {1}", m_ColorAttachments.size(), index);
		return 0;
	}
}