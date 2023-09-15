#ifndef HG_CHONPS_CUBEMAP_H
#define HG_CHONPS_CUBEMAP_H

#include "Shader.h"
#include "Camera.h"

namespace Chonps
{
	struct CubemapCreateInfo
	{
		std::string posx, negx, posy, negy, posz, negz;
	};

	class Cubemap
	{
	public:
		// Note: Order of cubemap texture must be in this index order: right, left, top, bottom, front, back
		Cubemap(CubemapCreateInfo cubemapInfo, uint32_t setIndex) {}

		virtual void Bind(Shader* shader) = 0;
		virtual void Delete() = 0;

		static void ChangeCubemapProjectionUniformName(const char* name) { m_CubemapProjectionUniformName = name; }
		static void ChangeCubeMapViewUniformName(const char* name) { m_CubemapViewUniformName = name; }

	protected:
		static const char* m_CubemapProjectionUniformName;
		static const char* m_CubemapViewUniformName;
	};

	std::shared_ptr<Cubemap> createCubemapSp(CubemapCreateInfo createInfo, uint32_t setIndex);
	Cubemap* createCubemap(CubemapCreateInfo createInfo, uint32_t setIndex);
}

#endif