#include "cepch.h"
#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>

#define CHONPS_TEXTURE_WARN(t1, t2) CHONPS_CORE_WARN("WARNING: MESH: Material Texture with the same texture type included twice! ({0}) and ({1})", (int)t1, (int)t2);

namespace Chonps
{
	void computeTangents(std::vector<vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		for (int i = 0; i < indices.size(); i += 3)
		{
			// Shortcuts for vertices
			glm::vec3& v0 = vertices[indices[i]].pos;
			glm::vec3& v1 = vertices[indices[i + 1]].pos;
			glm::vec3& v2 = vertices[indices[i + 2]].pos;

			// Shortcuts for UVs
			glm::vec2& uv0 = vertices[indices[i]].texUV;
			glm::vec2& uv1 = vertices[indices[i + 1]].texUV;
			glm::vec2& uv2 = vertices[indices[i + 2]].texUV;

			// Edges of the triangle : pos delta
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

	void sortTexturesToMaterial(std::vector<Texture*>& textures, Material& material)
	{
		for (auto& texture : textures)
		{
			switch (texture->GetTexType())
			{
				case TexType::Diffuse:
				{
					if (material.diffuse != nullptr) CHONPS_TEXTURE_WARN(material.diffuse, texture);
					material.diffuse = texture;
					break;
				}
				case TexType::Specular:
				{
					if (material.specular != nullptr) CHONPS_TEXTURE_WARN(material.specular, texture);
					material.specular = texture;
					break;
				}
				case TexType::Metallic:
				{
					if (material.metallic != nullptr) CHONPS_TEXTURE_WARN(material.metallic, texture);
					material.metallic = texture;
					break;
				}
				case TexType::Roughness:
				{
					if (material.roughness != nullptr) CHONPS_TEXTURE_WARN(material.roughness, texture);
					material.roughness = texture;
					break;
				}
				case TexType::MetallicRoughness:
				{
					if (material.metallicRoughness != nullptr) CHONPS_TEXTURE_WARN(material.metallicRoughness, texture);
					material.metallicRoughness = texture;
					break;
				}
				case TexType::Occlusion:
				{
					if (material.ambientOcclusion != nullptr) CHONPS_TEXTURE_WARN(material.ambientOcclusion, texture);
					material.ambientOcclusion = texture;
					break;
				}
				case TexType::Emissive:
				{
					if (material.emission != nullptr) CHONPS_TEXTURE_WARN(material.emission, texture);
					material.emission = texture;
					break;
				}
				case TexType::Normal:
				{
					if (material.normal != nullptr) CHONPS_TEXTURE_WARN(material.normal, texture);
					material.normal = texture;
					break;
				}
				default:
					break;
			}
		}
	}

	Mesh::Mesh(std::vector<vertex>& _vertices, std::vector<uint32_t>& _indices, std::vector<Texture*> _textures /*= std::vector<Texture*>()*/)
		: vertices(_vertices), indices(_indices), m_Textures(_textures)
	{
		vertexArray = createVertexArray();
		vertexArray->Bind();

		computeTangents(_vertices, _indices);

		VertexBuffer* vbo = createVertexBuffer(_vertices);
		IndexBuffer* ibo = createIndexBuffer(_indices);

		vertexArray->LinkIndexBuffer(ibo);
		vertexArray->LinkVertexBuffer(vbo, 0, 3, SDT::Float, sizeof(vertex), (void*)0);
		vertexArray->LinkVertexBuffer(vbo, 1, 3, SDT::Float, sizeof(vertex), (void*)(3 * sizeof(float)));
		vertexArray->LinkVertexBuffer(vbo, 2, 2, SDT::Float, sizeof(vertex), (void*)(6 * sizeof(float)));
		vertexArray->LinkVertexBuffer(vbo, 3, 3, SDT::Float, sizeof(vertex), (void*)(8 * sizeof(float)));
		vertexArray->LinkVertexBuffer(vbo, 4, 3, SDT::Float, sizeof(vertex), (void*)(11 * sizeof(float)));
		vertexArray->LinkVertexBuffer(vbo, 5, 3, SDT::Float, sizeof(vertex), (void*)(14 * sizeof(float)));
		vertexArray->Unbind();
		vbo->Unbind();
		ibo->Unbind();

		vbo->Delete();
		ibo->Delete();

		sortTexturesToMaterial(_textures, material);
	}

	Mesh::Mesh(VertexArray* _vertexArray, std::vector<Texture*> _textures)
		: m_Textures(_textures)
	{
		vertexArray = _vertexArray;
		sortTexturesToMaterial(_textures, material);
	}

	Mesh::Mesh(VertexArray* _vertexArray, Material _material)
	{
		vertexArray = _vertexArray;
		material = _material;
	}

	void Mesh::AttachMaterial(Material _material)
	{
		material = _material;
	}

	void Mesh::Delete()
	{
		vertexArray->Delete();

		for (auto& texture : m_Textures)
			texture->Delete();
	}
}