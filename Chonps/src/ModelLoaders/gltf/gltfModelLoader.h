#ifndef HG_CHONPS_GLFT_MODEL_LOADER_H
#define HG_CHONPS_GLFT_MODEL_LOADER_H

#include "Graphics/Mesh.h"
#include "Core/DataTypes.h"
#include <json/json.h>
using json = nlohmann::json;

namespace Chonps
{
	std::vector<Mesh> loadgltfModel(const std::string& filepath);
	Mesh loadgltfModelBatched(const std::string& filepath);

	class gltfModel
	{
	public:
		gltfModel(const std::string& filepath, bool batched);

		std::vector<Mesh> GetMeshes() { return m_Meshes; }

	private:
		const char* m_File;
		std::vector<unsigned char> m_Data;
		json JSON;
		bool m_Batched = false;

		std::vector<Mesh> m_Meshes;
		std::vector<glm::mat4> m_MatricesMeshes;

		std::vector<unsigned int> m_TexIndex;
		std::vector<Texture*> m_Textures;

		void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));
		void loadMesh(unsigned int meshIndex, glm::mat4 matrix = glm::mat4(1.0f));

		std::vector<unsigned char> getData();
		std::vector<float> getFloats(json accessor);
		glm::vec3 getColors(json material);
		std::vector<uint32_t> getIndices(json accessor);
		std::vector<Texture*> getTextures(json material);
		TexFilterPair GetTexFilter(uint32_t mag, uint32_t min);
		TexWrap GetTexWrap(uint32_t wrap);
	};
}

#endif