#include "cepch.h"
#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>

#define CHONPS_TEXTURE_WARN(t1, t2) CHONPS_CORE_WARN("WARNING: MESH: Material Texture with the same texture type included twice! ({0}) and ({1})", t1, t2);

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
				case TexType::Albedo:
				{
					if (material.albedo != nullptr) CHONPS_TEXTURE_WARN(material.albedo->id(), texture->id());
					material.albedo = texture;
					break;
				}
				case TexType::Diffuse:
				{
					if (material.diffuse != nullptr) CHONPS_TEXTURE_WARN(material.diffuse->id(), texture->id());
					material.diffuse = texture;
					break;
				}
				case TexType::Specular:
				{
					if (material.specular != nullptr) CHONPS_TEXTURE_WARN(material.specular->id(), texture->id());
					material.specular = texture;
					break;
				}
				case TexType::Metallic:
				{
					if (material.metallic != nullptr) CHONPS_TEXTURE_WARN(material.metallic->id(), texture->id());
					material.metallic = texture;
					break;
				}
				case TexType::Roughness:
				{
					if (material.roughness != nullptr) CHONPS_TEXTURE_WARN(material.roughness->id(), texture->id());
					material.roughness = texture;
					break;
				}
				case TexType::MetallicRoughness:
				{
					if (material.metallicRoughness != nullptr) CHONPS_TEXTURE_WARN(material.metallicRoughness->id(), texture->id());
					material.metallicRoughness = texture;
					break;
				}
				case TexType::Occlusion:
				{
					if (material.ambientOcclusion != nullptr) CHONPS_TEXTURE_WARN(material.ambientOcclusion->id(), texture->id());
					material.ambientOcclusion = texture;
					break;
				}
				case TexType::Emissive:
				{
					if (material.emission != nullptr) CHONPS_TEXTURE_WARN(material.emission->id(), texture->id());
					material.emission = texture;
					break;
				}
				case TexType::Normal:
				{
					if (material.normal != nullptr) CHONPS_TEXTURE_WARN(material.normal->id(), texture->id());
					material.normal = texture;
					break;
				}
				default:
					break;
			}
		}
	}

	Mesh::Mesh(std::vector<vertex>& p_vertices, std::vector<uint32_t>& p_indices, std::vector<Texture*> p_textures /*= std::vector<Texture*>()*/)
		: vertices(p_vertices), indices(p_indices), m_Textures(p_textures)
	{
		vertexArray = createVertexArray();

		computeTangents(p_vertices, p_indices);

		m_VertexBuffer = createVertexBuffer(p_vertices);
		m_IndexBuffer = createIndexBuffer(p_indices);
		
		std::vector<VertexLayout> layouts =
		{
			{ 0, 3, SDT::Float, sizeof(vertex), (void*)(offsetof(vertex, pos)) },
			{ 1, 3, SDT::Float, sizeof(vertex), (void*)(offsetof(vertex, color)) },
			{ 2, 2, SDT::Float, sizeof(vertex), (void*)(offsetof(vertex, texUV)) },
			{ 3, 3, SDT::Float, sizeof(vertex), (void*)(offsetof(vertex, normal)) },
			{ 4, 3, SDT::Float, sizeof(vertex), (void*)(offsetof(vertex, tangent)) },
			{ 5, 3, SDT::Float, sizeof(vertex), (void*)(offsetof(vertex, bitangent)) },
		};

		VertexLayoutLinkInfo linkInfo{};
		linkInfo.layoutCount = static_cast<uint32_t>(layouts.size());
		linkInfo.pLayouts = layouts.data();

		vertexArray->LinkBuffers(m_VertexBuffer, m_IndexBuffer, &linkInfo);
		vertexArray->Unbind();
		m_VertexBuffer->Unbind();
		m_IndexBuffer->Unbind();

		sortTexturesToMaterial(p_textures, material);
	}

	Mesh::Mesh(VertexArray* p_vertexArray, std::vector<Texture*> p_textures)
		: m_Textures(p_textures)
	{
		vertexArray = p_vertexArray;
		sortTexturesToMaterial(p_textures, material);
	}

	Mesh::Mesh(VertexArray* p_vertexArray, Material p_material)
	{
		vertexArray = p_vertexArray;
		material = p_material;
	}

	void Mesh::AttachMaterial(Material p_material)
	{
		material = p_material;
	}

	void Mesh::Delete()
	{
		vertexArray->Delete();
		m_VertexBuffer->Delete();
		m_IndexBuffer->Delete();

		for (auto& texture : m_Textures)
			texture->Delete();
	}
}