#include "cepch.h"
#include "OBJModelLoader.h"

#include "Core/File.h"

namespace Chonps
{
	struct OBJmaterial
	{
		float Ns;
		glm::vec3 Ka;
		glm::vec3 Kd;
		glm::vec3 Ks;
		glm::vec3 Ke;
		float Ni;
		float d;

		Texture* map_Kd = nullptr;
		Texture* map_Ks = nullptr;

		bool HasMaterials = false;
	};

	std::vector<Mesh> loadOBJModel(const std::string& filepath)
	{
		std::vector<Mesh> Meshes;

		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture*> textures;


		std::vector<glm::vec3> pos;
		std::vector<glm::vec3> color;
		std::vector<glm::vec2> texUVs;
		std::vector<glm::vec3> normals;

		std::string data;
		if (!get_file_contents(filepath.c_str(), &data))
		{
			CHONPS_CORE_ERROR("ERROR: MODEL: OBJ: Cannot open Obj file: {0}", filepath);
			return Meshes;
		}

		// Get Vertices
		const char* dataType = "v ";
		size_t dataTypeLength = strlen(dataType);
		size_t dataVertices = data.find(dataType, 0);
		while (dataVertices != std::string::npos)
		{
			// Find vertex position in data
			size_t eol = data.find_first_of("\r\n", dataVertices);
			std::string getvec = data.substr(dataVertices + dataTypeLength, eol - dataVertices - dataTypeLength);
			size_t offset = 0;
			size_t count = getvec.find(" ", offset + 1);

			// Get x value
			std::string vecx = getvec.substr(offset, count);
			float vx = std::stof(vecx);
			offset = getvec.find(" ", offset + 1);
			count = getvec.find(" ", offset + 1);

			// Get y value
			std::string vecy = getvec.substr(offset, count - offset);
			float vy = std::stof(vecy);
			offset = getvec.find(" ", offset + 1);

			// Get z value
			std::string vecz = getvec.substr(offset);
			float vz = std::stof(vecz);

			glm::vec3 position(vx, vy, vz);
			pos.push_back(position);

			size_t nextLinePos = data.find_first_not_of("\r\n", eol);
			dataVertices = data.find(dataType, nextLinePos);
		}

		// Get Normals
		dataType = "vn ";
		dataTypeLength = strlen(dataType);
		dataVertices = data.find(dataType, 0);
		while (dataVertices != std::string::npos)
		{
			// Find vertex position in data
			size_t eol = data.find_first_of("\r\n", dataVertices);
			std::string getvec = data.substr(dataVertices + dataTypeLength, eol - dataVertices - dataTypeLength);
			size_t offset = 0;
			size_t count = getvec.find(" ", offset + 1);

			// Get x value
			std::string vecx = getvec.substr(offset, count);
			float vx = std::stof(vecx);
			offset = getvec.find(" ", offset + 1);
			count = getvec.find(" ", offset + 1);

			// Get y value
			std::string vecy = getvec.substr(offset, count - offset);
			float vy = std::stof(vecy);
			offset = getvec.find(" ", offset + 1);

			// Get z value
			std::string vecz = getvec.substr(offset);
			float vz = std::stof(vecz);

			glm::vec3 norm(vx, vy, vz);
			normals.push_back(norm);

			size_t nextLinePos = data.find_first_not_of("\r\n", eol);
			dataVertices = data.find(dataType, nextLinePos);
		}

		// Get Tex Coord
		dataType = "vt ";
		dataTypeLength = strlen(dataType);
		dataVertices = data.find(dataType, 0);
		while (dataVertices != std::string::npos)
		{
			// Find vertex position in data
			size_t eol = data.find_first_of("\r\n", dataVertices);
			std::string getvec = data.substr(dataVertices + dataTypeLength, eol - dataVertices - dataTypeLength);
			size_t offset = 0;
			size_t count = getvec.find(" ", offset + 1);

			// Get x value
			std::string vecx = getvec.substr(offset, count);
			float vx = std::stof(vecx);
			offset = getvec.find(" ", offset + 1);

			// Get y value
			std::string vecy = getvec.substr(offset);
			float vy = std::stof(vecy);

			glm::vec2 texCoord(vx, vy);
			texUVs.push_back(texCoord);

			size_t nextLinePos = data.find_first_not_of("\r\n", eol);
			dataVertices = data.find(dataType, nextLinePos);
		}

		std::unordered_map<std::string, OBJmaterial> materials;
		std::vector<std::string> materialNames;

		// Get material data
		dataType = "mtllib ";
		dataTypeLength = strlen(dataType);
		dataVertices = data.find(dataType, 0);
		if (dataVertices != std::string::npos)
		{
			size_t eol = data.find_first_of("\r\n", dataVertices);
			std::string mtlfilename = data.substr(dataVertices + dataTypeLength, eol - dataVertices - dataTypeLength);

			std::string mtlfilepath = mtlfilename;
			size_t directory = filepath.find_last_of("/\\");
			if (directory != std::string::npos)
				mtlfilepath = filepath.substr(0, directory + 1) + mtlfilename;

			std::string mtldata;
			if (get_file_contents(mtlfilepath.c_str(), &mtldata))
			{
				const char* material = "newmtl ";
				size_t materialLength = std::strlen(material);
				size_t materialPos = mtldata.find(material, 0);
				while (materialPos != std::string::npos)
				{
					OBJmaterial objMat; // Create obj material to hold material data

					// Get name of Material
					size_t materialeol = mtldata.find_first_of("\r\n", materialPos);
					std::string materialName = mtldata.substr(materialPos + materialLength, materialeol - materialPos - materialLength);

					size_t nextLinePos = mtldata.find_first_not_of("\r\n", materialeol);
					size_t nextnewmtlpos = mtldata.find(material, nextLinePos);
					std::string materialSource = mtldata.substr(materialeol, nextnewmtlpos);


					// Find color data
					const char* mtldataType = "Kd ";
					size_t mtldataTypeLength = std::strlen(mtldataType);
					size_t mtldataTypePos = materialSource.find(mtldataType, 0);
					if (mtldataTypePos != std::string::npos)
					{

						size_t mtleol = materialSource.find_first_of("\r\n", mtldataTypePos);
						std::string getcolor = materialSource.substr(mtldataTypePos + mtldataTypeLength, mtleol - mtldataTypePos - mtldataTypeLength);
						size_t offset = 0;
						size_t count = getcolor.find(" ", offset + 1);

						// Get r value
						std::string colr = getcolor.substr(offset, count);
						float r = std::stof(colr);
						offset = getcolor.find(" ", offset + 1);
						count = getcolor.find(" ", offset + 1);

						// Get g value
						std::string colg = getcolor.substr(offset, count - offset);
						float g = std::stof(colg);
						offset = getcolor.find(" ", offset + 1);

						// Get b value
						std::string colb = getcolor.substr(offset);
						float b = std::stof(colb);

						objMat.Kd = glm::vec3(r, g, b);
					}

					// Get textures
					mtldataType = "map_Kd ";
					mtldataTypeLength = std::strlen(mtldataType);
					mtldataTypePos = materialSource.find(mtldataType, 0);
					if (mtldataTypePos != std::string::npos)
					{
						size_t mtleol = materialSource.find_first_of("\r\n", mtldataTypePos);
						std::string getTexturePath = materialSource.substr(mtldataTypePos + mtldataTypeLength, mtleol - mtldataTypePos - mtldataTypeLength);

						Texture* tex = createTexture(getTexturePath, TexType::Diffuse);
						objMat.map_Kd = tex;

						size_t nextLineTexPos = data.find_first_not_of("\r\n", materialeol);
						mtldataTypePos = data.find(materialSource, nextLineTexPos);

						objMat.HasMaterials = true;
					}

					mtldataType = "map_Ks ";
					mtldataTypeLength = std::strlen(mtldataType);
					mtldataTypePos = materialSource.find(mtldataType, 0);
					if (mtldataTypePos != std::string::npos)
					{
						size_t mtleol = materialSource.find_first_of("\r\n", mtldataTypePos);
						std::string getTexturePath = materialSource.substr(mtldataTypePos + mtldataTypeLength, mtleol - mtldataTypePos - mtldataTypeLength);

						Texture* tex = createTexture(getTexturePath, TexType::Specular);
						objMat.map_Ks = tex;

						size_t nextLineTexPos = data.find_first_not_of("\r\n", materialeol);
						mtldataTypePos = data.find(materialSource, nextLineTexPos);

						objMat.HasMaterials = true;
					}

					if (!objMat.HasMaterials)
					{
						uint32_t whiteTextureData = 0xffffffff;
						Texture* tex = createTexture(1, 1, 4, &whiteTextureData);
						objMat.map_Kd = tex;
					}

					materials[materialName] = objMat;

					// Find next newmtl keyword
					materialPos = mtldata.find(material, nextLinePos);
				}
			}
			else
				CHONPS_CORE_ERROR("ERROR: MODEL: OBJ: Cannot open Obj mtl file: {0}", mtlfilepath);
		}
		else
		{
			CHONPS_CORE_WARN("WARNING: MODEL: OBJ: No Obj mtl file can be found; Assuming no mtl file was created");
		}

		int indexValue = 0;

		// Get material data
		dataType = "usemtl ";
		dataTypeLength = strlen(dataType);
		dataVertices = data.find(dataType, 0);
		bool NoMat = dataVertices == std::string::npos ? true : false;
		if (NoMat) // If obj file does not have any materials
		{
			// Group vectors
			const char* dataFaceType = "f ";
			size_t dataFaceTypeLength = strlen(dataFaceType);
			size_t dataFaceVertices = data.find(dataFaceType, 0);
			while (dataFaceVertices != std::string::npos)
			{
				// Find vertex position in data
				size_t eol = data.find_first_of("\r\n", dataFaceVertices);
				std::string getface = data.substr(dataFaceVertices + dataFaceTypeLength, eol - dataFaceVertices - dataFaceTypeLength);
				size_t count = getface.find(" ", 0);
				size_t offset = 0;
				for (int i = 0; i < 3; i++)
				{
					std::string face = getface.substr(offset, count - offset);

					// Get pos value
					size_t indexPosOffset = face.find("/", 0);
					std::string position = face.substr(0, indexPosOffset);
					int posIndex = std::stoi(position);

					// Get texCoord value
					size_t indexTexOffset = face.find("/", indexPosOffset + 1);
					std::string texCoord = face.substr(indexPosOffset + 1, indexTexOffset - indexPosOffset - 1);
					int texIndex = std::stoi(texCoord);

					// Get Normal value
					std::string normal = face.substr(indexTexOffset + 1);
					int normIndex = std::stoi(normal);

					// Add Indices
					indices.push_back(indexValue++);

					vertex Vertex = { pos[posIndex - 1], glm::vec3(0.8f), texUVs[texIndex - 1], normals[normIndex - 1] };
					vertices.push_back(Vertex);

					offset += face.length() + 1;
					count = getface.find(" ", offset);

					if (i == 2 && count != std::string::npos)
						CHONPS_CORE_ERROR("ERROR: MODEL: OBJ: Index length for drawing faces [f] is more than 3\nChonps can only currently draw triangle faces!");
				}

				size_t nextLinePos = data.find_first_not_of("\r\n", eol);
				dataFaceVertices = data.find(dataFaceType, nextLinePos);
			}

			Meshes.emplace_back(Mesh(vertices, indices, textures));
			vertices.clear();
			indices.clear();
			indexValue = 0;
		}
		else // obj file has materials
		{
			while (dataVertices != std::string::npos)
			{
				size_t mtleol = data.find_first_of("\r\n", dataVertices);
				std::string materialName = data.substr(dataVertices + dataTypeLength, mtleol - dataVertices - dataTypeLength);

				size_t newmtlLinePos = data.find_first_not_of("\r\n", mtleol);
				dataVertices = data.find(dataType, newmtlLinePos);
				std::string materialSource = data.substr(newmtlLinePos, dataVertices - (newmtlLinePos == std::string::npos ? data.size() - 1 : newmtlLinePos));

				// Group vectors
				const char* dataFaceType = "f ";
				size_t dataFaceTypeLength = strlen(dataFaceType);
				size_t dataFaceVertices = materialSource.find(dataFaceType, 0);
				while (dataFaceVertices != std::string::npos)
				{
					// Find vertex position in data
					size_t eol = materialSource.find_first_of("\r\n", dataFaceVertices);
					std::string getface = materialSource.substr(dataFaceVertices + dataFaceTypeLength, eol - dataFaceVertices - dataFaceTypeLength);
					size_t count = getface.find(" ", 0);
					size_t offset = 0;
					for (int i = 0; i < 3; i++)
					{
						std::string face = getface.substr(offset, count - offset);

						// Get pos value
						size_t indexPosOffset = face.find("/", 0);
						std::string position = face.substr(0, indexPosOffset);
						int posIndex = std::stoi(position);

						// Get texCoord value
						size_t indexTexOffset = face.find("/", indexPosOffset + 1);
						std::string texCoord = face.substr(indexPosOffset + 1, indexTexOffset - indexPosOffset - 1);
						int texIndex = std::stoi(texCoord);

						// Get Normal value
						std::string normal = face.substr(indexTexOffset + 1);
						int normIndex = std::stoi(normal);

						// Add Indices
						indices.push_back(indexValue++);

						glm::vec3 colors = glm::vec3(1.0f);
						for (auto mat : materials)
						{
							if (materialName == mat.first)
								colors = mat.second.Kd;
						}

						vertex Vertex = { pos[posIndex - 1], colors, texUVs[texIndex - 1], normals[normIndex - 1] };
						vertices.push_back(Vertex);

						offset += face.length() + 1;
						count = getface.find(" ", offset);

						if (i == 2 && count != std::string::npos)
							CHONPS_CORE_ERROR("ERROR: MODEL: OBJ: Index length for drawing faces [f] is more than 3\nChonps can only currently draw triangle faces!");
					}

					size_t nextLinePos = materialSource.find_first_not_of("\r\n", eol);
					dataFaceVertices = materialSource.find(dataFaceType, nextLinePos);
				}

				for (auto mat : materials)
				{
					if (materialName == mat.first)
					{
						if (mat.second.map_Kd != nullptr)
							textures.emplace_back(mat.second.map_Kd);
						if (mat.second.map_Ks != nullptr)
							textures.emplace_back(mat.second.map_Ks);
					}
				}

				Meshes.emplace_back(Mesh(vertices, indices, textures));
				vertices.clear();
				indices.clear();
				textures.clear();
				indexValue = 0;
			}
		}

		return Meshes;
	}
}