#include "cepch.h"
#include "OpenGLCubemap.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

namespace Chonps
{
	float cubemapVertices[] =
	{
		//   Coordinates
		-1.0f, -1.0f,  1.0f, //        7--------6
		 1.0f, -1.0f,  1.0f, //       /|       /|  
		 1.0f, -1.0f, -1.0f, //      4--------5 |  
		-1.0f, -1.0f, -1.0f, //      | |      | |  y  z   <-- positive z facing backwards
		-1.0f,  1.0f,  1.0f, //      | 3------|-2  | /		
		 1.0f,  1.0f,  1.0f, //      |/       |/   |/
		 1.0f,  1.0f, -1.0f, //      0--------1    o----x
		-1.0f,  1.0f, -1.0f  //		Note: OpenGL cube maps uses the left hand rule
	};

	uint32_t cubemapIndices[] =
	{
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3
	};

	OpenGLCubemap::OpenGLCubemap(const std::string cubeMapFaces[6])
		: Cubemap(cubeMapFaces)
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		uint32_t cmVBO, cmEBO;
		glGenBuffers(1, &cmVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cmVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);
		glGenBuffers(1, &cmEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cmEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubemapIndices), cubemapIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		

		// Creates a cube map texture
		glGenTextures(1, &m_CubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapTexture);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// loads the texture for each side of the cube
		for (unsigned int i = 0; i < 6; i++)
		{
			int width, height, channels;
			unsigned char* data = stbi_load(cubeMapFaces[i].c_str(), &width, &height, &channels, 0);
			if (data)
			{
				// Get color channel format
				GLenum internalFormat = 0, dataFormat = 0;
				if (channels == 4)
				{
					internalFormat = GL_RGBA8;
					dataFormat = GL_RGBA;
				}
				else if (channels == 3)
				{
					internalFormat = GL_RGB8;
					dataFormat = GL_RGB;
				}
				else if (channels == 1)
				{
					internalFormat = GL_RGB8;
					dataFormat = GL_RED;
				}
				CHONPS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				CHONPS_CORE_ERROR("ERROR: CUBEMAP: TEXTURE: Failed to load texture: {0}", cubeMapFaces[i]);
				stbi_image_free(data);
			}
		}
	}
	OpenGLCubemap::OpenGLCubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz)
		: Cubemap(posx, negx, posy, negy, posz, negz)
	{
		const std::string cubemapFaces[6] = { posx, negx, posy, negy, posz, negz };

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		uint32_t cmVBO, cmEBO;
		glGenBuffers(1, &cmVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cmVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);
		glGenBuffers(1, &cmEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cmEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubemapIndices), cubemapIndices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		// Creates a cube map texture
		glGenTextures(1, &m_CubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapTexture);
		glTextureParameteri(m_CubemapTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_CubemapTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_CubemapTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_CubemapTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_CubemapTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// loads the texture for each side of the cube
		for (unsigned int i = 0; i < 6; i++)
		{
			int width, height, channels;
			unsigned char* data = stbi_load((cubemapFaces[i]).c_str(), &width, &height, &channels, 0);
			if (data)
			{
				// Get color channel format
				GLenum internalFormat = 0, dataFormat = 0;
				if (channels == 4)
				{
					internalFormat = GL_RGBA8;
					dataFormat = GL_RGBA;
				}
				else if (channels == 3)
				{
					internalFormat = GL_RGB8;
					dataFormat = GL_RGB;
				}
				else if (channels == 1)
				{
					internalFormat = GL_RGB8;
					dataFormat = GL_RED;
				}
				CHONPS_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				CHONPS_CORE_ERROR("ERROR: CUBEMAP: TEXTURE: Failed to load texture: {0}", cubemapFaces[i]);
				stbi_image_free(data);
			}
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	void OpenGLCubemap::Use(Shader* shader, Camera camera)
	{
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_CULL_FACE);
		shader->Bind();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::mat4(mat3(glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetOrientation(), camera.GetUpVector())));
		projection = glm::perspective(camera.GetFOVrad(), (float)camera.GetWidth() / (float)camera.GetHeight(), camera.GetNearPlane(), camera.GetFarPlane());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), m_CubemapViewUniformName), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), m_CubemapProjectionUniformName), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(m_VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
	}
}