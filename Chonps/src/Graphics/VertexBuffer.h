#ifndef HG_CHONPS_VERTEX_BUFFER_H
#define HG_CHONPS_VERTEX_BUFFER_H

#include "Core/DataTypes.h"

#include <vector>

namespace Chonps
{
	enum class ShaderDataType
	{
		None = 0,
		Float,
		Mat2, Mat3, Mat4,
		Int,
		Bool
	};
	typedef ShaderDataType SDT;

	enum class BufferState
	{
		Static,
		Dynamic, DynamicResize,
	};

	uint32_t getShaderDataTypeSize(ShaderDataType type);
	uint32_t getShaderDataTypeComponent(ShaderDataType type);

	class VertexBuffer
	{
	public:
		VertexBuffer(float* vertices, uint32_t size, BufferState bufferState = BufferState::Static) {}
		VertexBuffer(std::vector<vertex>& vertices, BufferState bufferState = BufferState::Static) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

		virtual uint32_t GetCount() const = 0;
		virtual float* GetVertices() const = 0;
		virtual uint32_t GetSize() const = 0;

		virtual void Subdata(float* vertices, uint32_t size) = 0;
		virtual void Subdata(std::vector<vertex>& vertices) = 0;

		virtual uint32_t id() const = 0;
	};

	std::shared_ptr<VertexBuffer> createVertexBufferSp(float* vertices, uint32_t size, BufferState bufferState = BufferState::Static);
	std::shared_ptr<VertexBuffer> createVertexBufferSp(std::vector<vertex>& vertices, BufferState bufferState = BufferState::Static);

	VertexBuffer* createVertexBuffer(float* vertices, uint32_t size, BufferState bufferState = BufferState::Static);
	VertexBuffer* createVertexBuffer(std::vector<vertex>& vertices, BufferState bufferState = BufferState::Static);
}

#endif