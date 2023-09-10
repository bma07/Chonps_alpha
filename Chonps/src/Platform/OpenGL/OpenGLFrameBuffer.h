#ifndef HG_CHONPS_OPENGL_FRAME_BUFFER_H
#define HG_CHONPS_OPENGL_FRAME_BUFFER_H

#include "Graphics/FrameBuffer.h"

namespace Chonps
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(FrameBufferSpecificationInfo frameBufferSpecificationInfo);

		virtual void Draw(uint32_t index = 0) override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Begin() override;
		virtual void End() override;

		virtual void BindTex(uint32_t slot = 0) override;

		virtual void Delete() override;

		virtual uint32_t id() const override { return m_FBO; }
		virtual uint32_t GetTexID(uint32_t index = 0) const override;

		virtual unsigned int GetWidth() override { return m_FrameBufferSpecificationInfo.width; }
		virtual unsigned int GetHeight() override { return m_FrameBufferSpecificationInfo.height; }

	private:
		void UpdateFrameBuffer();

		uint32_t m_FBO, m_PostFBO, m_VAO;
		uint32_t m_VPx, m_VPy, m_VPWidth, m_VPHeight;
		
		FrameBufferSpecificationInfo m_FrameBufferSpecificationInfo;

		std::vector<FrameBufferColorAttachment> m_ColorAttachmentSpecifications;
		std::vector<uint32_t> m_ColorAttachments, m_PreColorAttachments;
		FrameBufferDepthAttachment m_DepthAttachmentSpecification = { 0, FrameBufferDepthFormat::None };
		uint32_t m_PreDepthAttachment, m_DepthAttachment;
		bool m_MultiSampled = false;
	};
}

#endif