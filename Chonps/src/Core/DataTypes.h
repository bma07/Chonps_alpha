#ifndef CHONPS_DATA_TYPES_H
#define CHONPS_DATA_TYPES_H

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Chonps
{
	// Vector Data Types
	typedef glm::vec1 vec1;
	typedef glm::vec2 vec2;
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;

	typedef glm::bvec1 vec1b;
	typedef glm::bvec2 vec2b;
	typedef glm::bvec3 vec3b;
	typedef glm::bvec4 vec4b;

	typedef glm::ivec1 vec1i;
	typedef glm::ivec2 vec2i;
	typedef glm::ivec3 vec3i;
	typedef glm::ivec4 vec4i;

	typedef glm::fvec1 vec1f;
	typedef glm::fvec2 vec2f;
	typedef glm::fvec3 vec3f;
	typedef glm::fvec4 vec4f;

	typedef glm::dvec1 vec1d;
	typedef glm::dvec2 vec2d;
	typedef glm::dvec3 vec3d;
	typedef glm::dvec4 vec4d;

	// Matrix Data Types
	typedef glm::mat2 mat2;
	typedef glm::mat3 mat3;
	typedef glm::mat4 mat4;
	typedef glm::mat3x2 mat3x2;
	typedef glm::mat2x3 mat2x3;
	typedef glm::mat2x4 mat2x4;
	typedef glm::mat4x2 mat4x2;

	typedef glm::dmat2 mat2d;
	typedef glm::dmat3 mat3d;
	typedef glm::dmat4 mat4d;
	typedef glm::dmat3x2 mat3x2d;
	typedef glm::dmat2x3 mat2x3d;
	typedef glm::dmat2x4 mat2x4d;
	typedef glm::dmat4x2 mat4x2d;

	typedef glm::fmat2 mat2f;
	typedef glm::fmat3 mat3f;
	typedef glm::fmat4 mat4f;
	typedef glm::fmat3x2 mat3x2f;
	typedef glm::fmat2x3 mat2x3f;
	typedef glm::fmat2x4 mat2x4f;
	typedef glm::fmat4x2 mat4x2f;

	typedef glm::quat quat;

	// Vertex data types
	struct vertextb
	{
		vec3 position;
		vec3 color;
		vec2 texUV;
		vec3 normal;

		vec3 tangent;
		vec3 bitangent;
	};

	struct vertex
	{
		vec3 position;
		vec3 color;
		vec2 texUV;
		vec3 normal;
	};
	
	struct vertexc
	{
		vec3 position;
		vec3 color;
		vec3 normal;
	};

	struct vertext
	{
		vec3 position;
		vec2 texUV;
		vec3 normal;
	};
}

#endif