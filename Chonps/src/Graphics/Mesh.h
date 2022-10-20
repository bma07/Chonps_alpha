#ifndef MESH_H
#define MESH_H

#include "Shader.h"
#include "Camera.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
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

		VAO* m_VAO;
	};
}

#endif