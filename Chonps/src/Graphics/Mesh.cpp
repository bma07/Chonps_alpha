#include "cepch.h"
#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>

namespace Chonps
{
	const char* Mesh::s_MatrixUniform = "meshMatrix";
	const char* Mesh::s_TexUnitName = "meshTex";

	void computeTangents(std::vector<vertextb>& vertices, const std::vector<uint32_t>& indices)
	{
		for (int i = 0; i < indices.size(); i += 3)
		{
			// Shortcuts for vertices
			glm::vec3& v0 = vertices[indices[i]].position;
			glm::vec3& v1 = vertices[indices[i + 1]].position;
			glm::vec3& v2 = vertices[indices[i + 2]].position;

			// Shortcuts for UVs
			glm::vec2& uv0 = vertices[indices[i]].texUV;
			glm::vec2& uv1 = vertices[indices[i + 1]].texUV;
			glm::vec2& uv2 = vertices[indices[i + 2]].texUV;

			// Edges of the triangle : position delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			vertices[indices[i]].tangent = tangent;
			vertices[indices[i + 1]].tangent = tangent;
			vertices[indices[i + 2]].tangent = tangent;

			vertices[indices[i]].bitangent = bitangent;
			vertices[indices[i + 1]].bitangent = bitangent;
			vertices[indices[i + 2]].bitangent = bitangent;
		}
	}

	Mesh::Mesh(std::vector<vertextb>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*> textures /*= std::vector<Texture*>()*/)
		: m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
	{
		m_VAO = createVertexArray();
		m_VAO->Bind();

		computeTangents(vertices, indices);

		VertexBuffer* vbo = createVertexBuffer(vertices);
		IndexBuffer* ibo = createIndexBuffer(indices);

		m_VAO->LinkIndexBuffer(ibo);
		m_VAO->LinkVertexBuffer(vbo, 0, 3, SDT::Float, sizeof(vertextb), (void*)0);
		m_VAO->LinkVertexBuffer(vbo, 1, 3, SDT::Float, sizeof(vertextb), (void*)(3 * sizeof(float)));
		m_VAO->LinkVertexBuffer(vbo, 2, 2, SDT::Float, sizeof(vertextb), (void*)(6 * sizeof(float)));
		m_VAO->LinkVertexBuffer(vbo, 3, 3, SDT::Float, sizeof(vertextb), (void*)(8 * sizeof(float)));
		m_VAO->LinkVertexBuffer(vbo, 4, 3, SDT::Float, sizeof(vertextb), (void*)(11 * sizeof(float)));
		m_VAO->LinkVertexBuffer(vbo, 5, 3, SDT::Float, sizeof(vertextb), (void*)(14 * sizeof(float)));
		m_VAO->Unbind();
		vbo->Unbind();
		ibo->Unbind();
	}

	void Mesh::Draw(Shader* shader, glm::mat4 matrix /*= glm::mat4(1.0f)*/)
	{
		
		shader->Bind();
		m_VAO->Bind();

		for (auto texture : m_Textures)
			texture->Bind();

		uploadUniform4mfv(shader->GetID(), Mesh::s_MatrixUniform, 1, false, glm::value_ptr(matrix * m_MeshMatrix));

		renderDraw(m_VAO);

		for (auto texture : m_Textures)
			texture->Unbind();
		m_VAO->Unbind();
	}

	void Mesh::AttachShader(Shader* shader)
	{
		uint32_t texSlot = 0;
		for (auto texture : m_Textures)
		{
			switch (texture->GetTexType())
			{
				case TexType::Diffuse:
				{
					texture->TexUnit(shader, (std::string(Mesh::s_TexUnitName) + "Diffuse").c_str(), texSlot++);
					break;
				}
				case TexType::Specular:
				{
					texture->TexUnit(shader, (std::string(Mesh::s_TexUnitName) + "Specular").c_str(), texSlot++);
					break;
				}
				case TexType::Metallic:
				{
					texture->TexUnit(shader, (std::string(Mesh::s_TexUnitName) + "Metallic").c_str(), texSlot++);
					break;
				}
				case TexType::Roughness:
				{
					texture->TexUnit(shader, (std::string(Mesh::s_TexUnitName) + "Roughness").c_str(), texSlot++);
					break;
				}
				case TexType::MetallicRoughness:
				{
					texture->TexUnit(shader, (std::string(Mesh::s_TexUnitName) + "MetallicRoughness").c_str(), texSlot++);
					break;
				}
				case TexType::Normal:
				{
					texture->TexUnit(shader, (std::string(Mesh::s_TexUnitName) + "Normal").c_str(), texSlot++);
					break;
				}
				case TexType::Occlusion:
				{
					texture->TexUnit(shader, (std::string(Mesh::s_TexUnitName) + "Occlusion").c_str(), texSlot++);
					break;
				}
				case TexType::Emissive:
				{
					texture->TexUnit(shader, (std::string(Mesh::s_TexUnitName) + "Emissive").c_str(), texSlot++);
					break;
				}
			}
		}
	}

	void Mesh::Delete()
	{
		m_VAO->Delete();
		delete m_VAO;

		for (auto i : m_Textures)
		{
			i->Delete();
			delete i;
		}
	}
}