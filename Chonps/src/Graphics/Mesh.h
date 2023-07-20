#ifndef CHONPS_MESH_H
#define CHONPS_MESH_H

#include "Shader.h"
#include "Camera.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Material.h"
#include "Renderer.h"
#include "Core/DataTypes.h"

namespace Chonps
{
	struct Mesh
	{
		Mesh() = default;

		Mesh(std::vector<vertex>& _vertices, std::vector<uint32_t>& _indices, std::vector<Texture*> _textures = std::vector<Texture*>());
		Mesh(VertexArray* _vertexArray, std::vector<Texture*> _textures);
		Mesh(VertexArray* _vertexArray, Material _material);

		void AttachMaterial(Material material);
		void Delete();

		std::vector<vertex>		vertices;
		std::vector<uint32_t>	indices;
		VertexArray*			vertexArray;
		Material				material;
		glm::mat4				matrix = glm::mat4(1.0f);

		uint32_t id() { return m_ID; }

	private:
		std::vector<Texture*> m_Textures;
		uint32_t m_ID;
	};
}

#endif