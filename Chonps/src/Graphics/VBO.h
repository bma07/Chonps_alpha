#ifndef VBO_H
#define VBO_H

#include "DataTypes.h"

#include <vector>

namespace Chonps
{
	enum class ShaderDataType
	{
		None = 0,
		Float, Float1, Float2, Float3, Float4,
		Mat2, Mat3, Mat4,
		Int, Int1, Int2, Int3, Int4,
		Bool
	};

	typedef ShaderDataType SDT;

	uint32_t getShaderDataTypeSize(ShaderDataType type);
	uint32_t getShaderDataTypeComponent(ShaderDataType type);

	class VBO
	{
	public:
		// VBO constructor for static arrays
		VBO(float* vertices, size_t size) {}
		// VBO constructor for dynamic array
		VBO(std::vector<vertex>& vertices) {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;

	};

	VBO* createVBO(float* vertices, uint32_t size);
	VBO* createVertexVBO(std::vector<vertex>& vertices);
}

#endif