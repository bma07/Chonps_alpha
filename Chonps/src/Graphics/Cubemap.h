#ifndef HG_CHONPS_CUBEMAP_H
#define HG_CHONPS_CUBEMAP_H

#include "Shader.h"
#include "Camera.h"

namespace Chonps
{
	struct CubemapCreateInfo
	{
		std::string posx, negx, posy, negy, posz, negz;
		Shader** pShaders;
		uint32_t shaderCount;
	};

	class Cubemap
	{
	public:
		// Note: Order of cubemap texture must be in this index order: right, left, top, bottom, front, back
		Cubemap(CubemapCreateInfo cubemapInfo) {}

		virtual void Draw() = 0;
		virtual void Delete() = 0;

		static void ChangeCubemapProjectionUniformName(const char* name) { m_CubemapProjectionUniformName = name; }
		static void ChangeCubeMapViewUniformName(const char* name) { m_CubemapViewUniformName = name; }

	protected:
		static const char* m_CubemapProjectionUniformName;
		static const char* m_CubemapViewUniformName;
	};

	std::shared_ptr<Cubemap> createCubemapSp(CubemapCreateInfo createInfo);
	Cubemap* createCubemap(CubemapCreateInfo createInfo);
}

#endif