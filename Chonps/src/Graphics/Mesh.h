#ifndef CHONPS_MESH_H
#define CHONPS_MESH_H

#include "Shader.h"
#include "Camera.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "DataTypes.h"
#include "Renderer.h"

namespace Chonps
{
	class Mesh
	{
	public:
		Mesh(std::vector<vertex>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*> textures = std::vector<Texture*>());

		void Draw(Shader* shader, mat4 matrix = mat4(1.0f));
		void Delete();

		static void ChangeMeshMatrixUniformName(const char* name) { s_MatrixUniform = name; }
		static void ChangeTexUnitName(const char* name) { s_TexUnitName = name; }

	private:
		std::vector<vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Texture*> m_Textures;

		static const char* s_MatrixUniform;
		static const char* s_TexUnitName;

		VertexArray* m_VAO;
	};
}

#endif