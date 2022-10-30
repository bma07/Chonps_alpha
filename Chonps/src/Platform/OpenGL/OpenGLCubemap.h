#ifndef OPENGL_CUBEMAP_H
#define OPENGL_CUBEMAP_H

#include "Graphics/Cubemap.h"

namespace Chonps
{
	class OpenGLCubemap : public Cubemap
	{
	public:
		// Note: Order of cubemap texture must be in this index order: posx, negx, posy, negy, posz, negz
		OpenGLCubemap(const std::string cubeMapFaces[6]);
		OpenGLCubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz);

		virtual void Use(Shader* shader, Camera camera) override;

	private:
		uint32_t m_CubemapTexture;
		uint32_t m_VAO;
	};
}

#endif