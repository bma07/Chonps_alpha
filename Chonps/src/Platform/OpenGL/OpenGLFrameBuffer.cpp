#include "cepch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Chonps
{
	float screen[] =
	{
		// Coords    // texCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	GLenum Multisampled(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	GLenum GetTexWrap(FrameBufferTextureSpecification frameBufferTexture)
	{
		switch (frameBufferTexture.TextureWrap)
		{
			case TexWrap::ClampEdge: return GL_CLAMP_TO_EDGE;
			case TexWrap::Repeat: return GL_REPEAT;
			case TexWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
			case TexWrap::Default: return GL_CLAMP_TO_EDGE;
		}

		return 0;
	}

	GLenum GetTexFilter(FrameBufferTextureSpecification frameBufferTexture)
	{
		switch (frameBufferTexture.TextureFilter)
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

	void CreateTextures(bool multisampled, uint32_t* TexID, uint32_t count)
	{
		glCreateTextures(Multisampled(multisampled), count, TexID);
	}

	void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum dataFormat, uint32_t width, uint32_t height, FrameBufferTextureSpecification FBTexspec, int index)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTexFilter(FBTexspec));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTexFilter(FBTexspec));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GetTexWrap(FBTexspec));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetTexWrap(FBTexspec));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetTexWrap(FBTexspec));
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, Multisampled(multisampled), id, 0);
	}

	void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height, FrameBufferTextureSpecification FBTexspec)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTexFilter(FBTexspec));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTexFilter(FBTexspec));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GetTexWrap(FBTexspec));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetTexWrap(FBTexspec));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetTexWrap(FBTexspec));
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, Multisampled(multisampled), id, 0);
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(FrameBufferSpecification frameBufferSpecification)
		: FrameBuffer(frameBufferSpecification)
	{
		m_FrameBufferSpecification = frameBufferSpecification;

		for (auto spec : m_FrameBufferSpecification.Attachments.TextureAttachments)
		{
			if (spec.TextureFormat != FrameBufferTextureFormat::DEPTH24STENCIL8 && spec.TextureFormat != FrameBufferTextureFormat::DEPTH_COMPONENT)
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		UpdateFrameBuffer();
	}

	void OpenGLFrameBuffer::UpdateFrameBuffer()
	{
		if (m_FBO)
		{
			glDeleteFramebuffers(1, &m_FBO);
			glDeleteTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());
			glDeleteTextures(1, &m_RBO);

			m_ColorAttachments.clear();
			m_RBO = 0;
		}

		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		bool multisample = m_FrameBufferSpecification.Samples >= 1;

		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			CreateTextures(multisample, m_ColorAttachments.data(), static_cast<uint32_t>(m_ColorAttachments.size()));

			for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				glBindTexture(Multisampled(multisample), m_ColorAttachments[i]);

				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
					case FrameBufferTextureFormat::RGB:
					{
						AttachColorTexture(m_ColorAttachments[i], m_FrameBufferSpecification.Samples, GL_RGB8, GL_RGB, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height, m_ColorAttachmentSpecifications[i], i);
						break;
					}
					case FrameBufferTextureFormat::RGBA8:
					{
						AttachColorTexture(m_ColorAttachments[i], m_FrameBufferSpecification.Samples, GL_RGBA8, GL_RGBA, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height, m_ColorAttachmentSpecifications[i], i);
						break;
					}
					case FrameBufferTextureFormat::RGBA16F:
					{
						AttachColorTexture(m_ColorAttachments[i], m_FrameBufferSpecification.Samples, GL_RGBA16F, GL_RGBA, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height, m_ColorAttachmentSpecifications[i], i);
						break;
					}
					case FrameBufferTextureFormat::RGBA32F:
					{
						AttachColorTexture(m_ColorAttachments[i], m_FrameBufferSpecification.Samples, GL_RGBA32F, GL_RGBA, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height, m_ColorAttachmentSpecifications[i], i);
						break;
					}
					case FrameBufferTextureFormat::RED_INT:
					{
						AttachColorTexture(m_ColorAttachments[i], m_FrameBufferSpecification.Samples, GL_R32I, GL_RED_INTEGER, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height, m_ColorAttachmentSpecifications[i], i);
						break;
					}
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
		{
			CreateTextures(multisample, &m_RBO, 1);
			glBindTexture(Multisampled(multisample), m_RBO);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
				case FrameBufferTextureFormat::DEPTH24STENCIL8:
				{
					AttachDepthTexture(m_RBO, m_FrameBufferSpecification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height, m_DepthAttachmentSpecification);
					break;
				}
				case FrameBufferTextureFormat::DEPTH_COMPONENT:
				{
					AttachDepthTexture(m_RBO, m_FrameBufferSpecification.Samples, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height, m_DepthAttachmentSpecification);
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

		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			CHONPS_CORE_ERROR("ERROR: FBO: Post FrameBuffer is not complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		/*	uint32_t screenVBO;
			glGenVertexArrays(1, &m_VAO);
			glGenBuffers(1, &screenVBO);
			glBindVertexArray(m_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(screen), &screen, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDeleteBuffers(1, &screenVBO);*/
	}

	void OpenGLFrameBuffer::Draw(uint32_t index /*= 0*/)
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[index]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_FrameBufferSpecification.Width = width;
		m_FrameBufferSpecification.Height = height;

		UpdateFrameBuffer();
	}

	void OpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glViewport(0, 0, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height);
	}

	void OpenGLFrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height);
	}

	void OpenGLFrameBuffer::Begin() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glViewport(0, 0, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height);
	}

	void OpenGLFrameBuffer::End() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_FrameBufferSpecification.Width, m_FrameBufferSpecification.Height);
	}

	void OpenGLFrameBuffer::BindTex(uint32_t slot /*= 0*/)
	{
		glBindTexture(Multisampled(m_FrameBufferSpecification.Samples), m_ColorAttachments[slot]);
	}

	void OpenGLFrameBuffer::Delete()
	{
		glDeleteFramebuffers(1, &m_FBO);
		glDeleteRenderbuffers(1, &m_RBO);
	}
	uint32_t OpenGLFrameBuffer::GetTexID(uint32_t index /*= 0*/) const
	{
		if (index < m_ColorAttachments.size()) return m_ColorAttachments[index];
		CHONPS_CORE_ERROR("ERROR: FBO: Texture ID index out of bounds! ID must be within range: {0} | Index given: {1}", m_ColorAttachments.size(), index);
		return 0;
	}
}