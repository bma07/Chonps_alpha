#ifndef HG_CHONPS_MESH_H
#define HG_CHONPS_MESH_H

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

		Mesh(std::vector<vertex>& p_vertices, std::vector<uint32_t>& p_indices, std::vector<Texture*> p_textures = std::vector<Texture*>());
		Mesh(VertexArray* p_vertexArray, std::vector<Texture*> p_textures);
		Mesh(VertexArray* p_vertexArray, Material p_material);

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
		VertexBuffer* m_VertexBuffer;
		IndexBuffer* m_IndexBuffer;

		uint32_t m_ID;
	};
}

#endif