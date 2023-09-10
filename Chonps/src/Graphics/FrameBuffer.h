#ifndef HG_CHONPS_FRAME_BUFFER_H
#define HG_CHONPS_FRAME_BUFFER_H

#include "Pipeline.h"
#include "Texture.h"
#include "Shader.h"

namespace Chonps
{
	enum class FrameBufferColorFormat
	{
		None = 0,
		Auto = FrameBufferColorFormat::None,
		RGB, RGBA, RGBA8, RGBA16F, RGBA32F, RED_INT,
	};

	enum class FrameBufferDepthFormat
	{
		None = 0,
		Auto = FrameBufferDepthFormat::None,
		DEPTH_COMPONENT,
		DEPTH24STENCIL8
	};

	struct FrameBufferColorAttachment
	{
		FrameBufferColorAttachment() = default;
		FrameBufferColorAttachment(uint32_t attachIndex, FrameBufferColorFormat texFormat)
			: index(attachIndex), format(texFormat) {}

		uint32_t index;
		FrameBufferColorFormat format = FrameBufferColorFormat::None;
	};

	struct FrameBufferDepthAttachment
	{
		FrameBufferDepthAttachment() = default;
		FrameBufferDepthAttachment(uint32_t attachIndex, FrameBufferDepthFormat texFormat)
			: index(attachIndex), format(texFormat) {}
		
		uint32_t index;
		FrameBufferDepthFormat format = FrameBufferDepthFormat::None;
	};

	struct FrameBufferSpecificationInfo
	{
		uint32_t width = 0;
		uint32_t height = 0;
		FrameBufferColorAttachment* colorAttachments;
		uint32_t colorAttachmentCount;
		FrameBufferDepthAttachment* depthAttachment;
		Sample samples = Sample::SampleCount_1_Bit;
		TexWrap textureWrap = TexWrap::ClampToEdge;
		TexFilter textureFilter = TexFilter::Nearest;
		Shader** pShaders;
		uint32_t shaderCount;
	};
	
	class FrameBuffer
	{
	public:
		FrameBuffer(FrameBufferSpecificationInfo frameBufferSpecificationInfo) {}

		virtual void Draw(uint32_t index = 0) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		// Begin and End are the same as Bind and Unbind
		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void BindTex(uint32_t slot = 0) = 0;

		virtual void Delete() = 0;

		virtual uint32_t id() const = 0;
		virtual uint32_t GetTexID(uint32_t index = 0) const = 0;

		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;
	};
	typedef FrameBuffer FBO;

	std::shared_ptr<FrameBuffer> createFrameBufferSp(FrameBufferSpecificationInfo frameBufferSpecificationInfo);

	FrameBuffer* createFrameBuffer(FrameBufferSpecificationInfo frameBufferSpecificationInfo);

	RenderPass retrieveRenderPass(FrameBuffer* frameBuffer);
}

#endif