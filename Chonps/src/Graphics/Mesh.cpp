#include "cepch.h"
#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>

namespace Chonps
{
	const char* Mesh::s_MatrixUniform = "meshMatrix";
	const char* Mesh::s_TexUnitName = "texUnit";

	Mesh::Mesh(std::vector<vertex>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*> textures /*= std::vector<Texture*>()*/)
		: m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
	{
		m_VAO = createVAO();

		m_VAO->Bind();

		VBO* VBO = createVertexVBO(vertices);
		EBO* EBO = createVertexEBO(indices);

		m_VAO->LinkEBO(EBO);
		m_VAO->LinkVBO(VBO, 0, 3, SDT::Float, sizeof(vertex), (void*)0);
		m_VAO->LinkVBO(VBO, 1, 3, SDT::Float, sizeof(vertex), (void*)(3 * sizeof(float)));
		m_VAO->LinkVBO(VBO, 2, 2, SDT::Float, sizeof(vertex), (void*)(6 * sizeof(float)));
		m_VAO->LinkVBO(VBO, 3, 3, SDT::Float, sizeof(vertex), (void*)(8 * sizeof(float)));
		m_VAO->Unbind();
		VBO->Unbind();
		EBO->Unbind();
	}

	void Mesh::Draw(Shader* shader, mat4 matrix /*= mat4(1.0f)*/)
	{
		shader->Bind();
		m_VAO->Bind();

		for (int i = 0; i < m_Textures.size(); i++)
		{
			m_Textures[i]->TexUnit(shader, Mesh::s_TexUnitName, 0);
			m_Textures[i]->Bind();
		}

		uploadUniform4mfv(shader->GetID(), Mesh::s_MatrixUniform, 1, false, glm::value_ptr(matrix));

		renderDraw(m_VAO);
	}

	void Mesh::Delete()
	{
		for (auto i : m_Textures)
		{
			i->Delete();
			delete i;
		}
	}
}