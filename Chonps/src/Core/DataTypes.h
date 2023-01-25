#ifndef CHONPS_DATA_TYPES_H
#define CHONPS_DATA_TYPES_H

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Chonps
{
	template <typename P1, typename P2>
	struct pair
	{
		P1 x;
		P2 y;
	};

	// Vertex data types
	struct vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texUV;
		glm::vec3 normal;

		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	struct vertexpc
	{
		glm::vec3 pos;
		glm::vec3 color;
	};
	
	struct vertexc
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec3 normal;
	};

	struct vertext
	{
		glm::vec3 pos;
		glm::vec2 texUV;
		glm::vec3 normal;
	};
}

#endif