#ifndef OPENGL_FRAME_BUFFER_H
#define OPENGL_FRAME_BUFFER_H

#include "Graphics/FrameBuffer.h"

namespace Chonps
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(FrameBufferSpecification frameBufferSpecification);

		virtual void Draw(uint32_t index = 0) override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		void UpdateFrameBuffer();

		virtual void CreateMsaaSampleAmount(uint32_t samples) override { m_FrameBufferSpecification.Samples = samples; }

		virtual void Bind() const override;
		virtual void Unbind() const override;

		// Begin and End are the same as Bind and Unbind
		virtual void Begin() const override;
		virtual void End() const override;

		virtual void BindTex(uint32_t slot = 0) override;

		virtual void Delete() override;

		virtual uint32_t GetID() const override { return m_FBO; }
		virtual uint32_t GetTexID(uint32_t index = 0) const override;

		virtual unsigned int GetWidth() override { return m_FrameBufferSpecification.Width; }
		virtual unsigned int GetHeight() override { return m_FrameBufferSpecification.Height; }

	private:
		uint32_t m_FBO;
		uint32_t m_RBO;
		
		FrameBufferSpecification m_FrameBufferSpecification;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecifications;
		std::vector<uint32_t> m_ColorAttachments;
		FrameBufferTextureSpecification m_DepthAttachmentSpecification = FrameBufferTextureFormat::None;
	};
}

#endif