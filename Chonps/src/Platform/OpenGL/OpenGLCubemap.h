#ifndef HG_CHONPS_OPENGL_CUBEMAP_H
#define HG_CHONPS_OPENGL_CUBEMAP_H

#include "Graphics/Cubemap.h"

namespace Chonps
{
	class OpenGLCubemap : public Cubemap
	{
	public:
		// Note: Order of cubemap texture must be in this index order: posx, negx, posy, negy, posz, negz
		OpenGLCubemap(CubemapCreateInfo cubemapInfo);

		virtual void Draw() override;
		virtual void Delete() override;

	private:
		uint32_t m_CubemapTexture;
		uint32_t m_VAO;
	};
}

#endif