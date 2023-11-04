#ifndef HG_CHONPS_UNIFORM_BUFFER_H
#define HG_CHONPS_UNIFORM_BUFFER_H

namespace Chonps
{
	enum class BufferType
	{
		None = 0,
		Uniform, Storage,
	};

	enum class ShaderStage
	{
		Vertex, Fragment, All
	};

	struct BufferLayout
	{
		void* layoutData;
		uint32_t setIndex;
	};

	struct PushConstantRange
	{
		uint32_t size, offset;
		ShaderStage shaderStage;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer(uint32_t binding, uint32_t size, BufferType bufferType) {}

		virtual void Bind(const void* data, uint32_t size, uint32_t offset) = 0;
		virtual void Delete() = 0;

		virtual uint32_t id() = 0;
	};

	struct BufferBindingInfo
	{
		UniformBuffer* uniformBuffer;
		uint32_t binding;
		ShaderStage shaderStage = ShaderStage::All;
		BufferType bufferType = BufferType::None;
	};

	struct BufferBindingCreateLayoutsInfo
	{
		BufferBindingInfo* pBufferBindings;
		uint32_t  bindingCount;
	};

	std::shared_ptr<UniformBuffer> createUniformBufferSp(uint32_t binding, uint32_t size, BufferType bufferType = BufferType::None);
	UniformBuffer* createUniformBuffer(uint32_t binding, uint32_t size, BufferType bufferType = BufferType::None);

	BufferLayout createBufferLayout(BufferBindingCreateLayoutsInfo* bufferBindings, uint32_t setIndex);
}

#endif