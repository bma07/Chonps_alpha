#ifndef CHONPS_FRAME_BUFFER_H
#define CHONPS_FRAME_BUFFER_H

#include "Texture.h"

namespace Chonps
{
	enum class FrameBufferTextureFormat
	{
		None = 0,
		RGB, RGBA, RGBA8, RGBA16F, RGBA32F, RED_INT,
		DEPTH_COMPONENT,
		DEPTH24STENCIL8
	};
	typedef FrameBufferTextureFormat FBTexFormat;

	struct FrameBufferTextureSpecification
	{
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat texFormat, TexWrap texWrap = TexWrap::ClampEdge, TexFilter texFilter = TexFilter::Nearest) 
			: TextureFormat(texFormat), TextureWrap(texWrap), TextureFilter(texFilter) {}

		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
		TexWrap TextureWrap;
		TexFilter TextureFilter;
	};
	typedef FrameBufferTextureSpecification FBTexSpec;

	struct FrameBufferTextureAttachment
	{
		FrameBufferTextureAttachment() = default;
		FrameBufferTextureAttachment(const std::initializer_list<FrameBufferTextureSpecification>& attachment)
			: TextureAttachments(attachment) {}

		std::vector<FrameBufferTextureSpecification> TextureAttachments;
	};
	typedef FrameBufferTextureAttachment FBTexAttachment;

	struct FrameBufferSpecification
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		FrameBufferTextureAttachment Attachments;
		uint32_t Samples = 0;
	};
	typedef FrameBufferSpecification FBSpec;
	
	class FrameBuffer
	{
	public:
		FrameBuffer(FrameBufferSpecification frameBufferSpecification) {}

		virtual void Draw(uint32_t index = 0) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		
		virtual void CreateMsaaSampleAmount(uint32_t samples) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		// Begin and End are the same as Bind and Unbind
		virtual void Begin() const = 0;
		virtual void End() const = 0;

		virtual void BindTex(uint32_t slot = 0) = 0;

		virtual void Delete() = 0;

		virtual uint32_t GetID() const = 0;
		virtual uint32_t GetTexID(uint32_t index = 0) const = 0;

		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;
	};
	typedef FrameBuffer FBO;

	std::shared_ptr<FrameBuffer> createFrameBufferSp(FrameBufferSpecification frameBufferSpecification);

	FrameBuffer* createFrameBuffer(FrameBufferSpecification frameBufferSpecification);
}

#endif