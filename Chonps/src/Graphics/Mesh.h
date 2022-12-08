#ifndef CHONPS_MESH_H
#define CHONPS_MESH_H

#include "Shader.h"
#include "Camera.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Renderer.h"
#include "Core/DataTypes.h"

namespace Chonps
{
	class Mesh
	{
	public:
		Mesh(std::vector<vertextb>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*> textures = std::vector<Texture*>());

		void Draw(Shader* shader, glm::mat4 matrix = glm::mat4(1.0f));
		void AttachShader(Shader* shader);
		void Delete();

		glm::mat4& GetMeshMatrix() { return m_MeshMatrix; }
		void SetMeshMatrix(glm::mat4 matrix) { m_MeshMatrix = matrix; }

		static void ChangeMeshMatrixUniformName(const char* name) { s_MatrixUniform = name; }
		static void ChangeTexUnitName(const char* name) { s_TexUnitName = name; }

	private:
		std::vector<vertextb> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Texture*> m_Textures;

		glm::mat4 m_MeshMatrix = glm::mat4(1.0f);

		static const char* s_MatrixUniform;
		static const char* s_TexUnitName;

		std::shared_ptr<VertexArray> m_VAO;

		bool m_HasTexture = false;
		bool m_TextureAttached = false;
	};
}

#endif