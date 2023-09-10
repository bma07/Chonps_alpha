#include "cepch.h"
#include "OpenGLCubemap.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

namespace Chonps
{
	float oglImplCubemapVertices[] =
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

	uint32_t oglImplCubemapIndices[] =
	{
		// Right
		6, 2, 1,
		6, 1, 5,
		// Left
		4, 0, 3,
		4, 3, 7,
		// Top
		5, 4, 7,
		5, 7, 6,
		// Bottom
		3, 0, 1,
		3, 1, 2,
		// Front
		5, 1, 0,
		5, 0, 4,
		// Back
		7, 3, 2,
		7, 2, 6
	};

	OpenGLCubemap::OpenGLCubemap(CubemapCreateInfo cubemapInfo)
		: Cubemap(cubemapInfo)
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		uint32_t cmVBO, cmIBO;
		glGenBuffers(1, &cmVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cmVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(oglImplCubemapVertices), oglImplCubemapVertices, GL_STATIC_DRAW);
		glGenBuffers(1, &cmIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cmIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(oglImplCubemapIndices), oglImplCubemapIndices, GL_STATIC_DRAW);

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

		std::string cubeMapFaces[6] = { cubemapInfo.posx, cubemapInfo.negx, cubemapInfo.posy, cubemapInfo.negy, cubemapInfo.posz, cubemapInfo.negz };

		stbi_set_flip_vertically_on_load(false);
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

	void OpenGLCubemap::Draw()
	{
		glBindVertexArray(m_VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void OpenGLCubemap::Delete()
	{
		glDeleteTextures(1, &m_CubemapTexture);
	}
}