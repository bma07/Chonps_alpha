#ifndef CHONPS_CUBEMAP_H
#define CHONPS_CUBEMAP_H

#include "Shader.h"
#include "Camera.h"

namespace Chonps
{
	class Cubemap
	{
	public:
		// Note: Order of cubemap texture must be in this index order: right, left, top, bottom, front, back
		Cubemap(const std::string cubeMapFaces[6]) {}
		Cubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz) {}

		virtual void Use(Shader* shader, Camera camera) = 0;

		static void ChangeCubemapProjectionUniformName(const char* name) { m_CubemapProjectionUniformName = name; }
		static void ChangeCubeMapViewUniformName(const char* name) { m_CubemapViewUniformName = name; }

	protected:
		static const char* m_CubemapProjectionUniformName;
		static const char* m_CubemapViewUniformName;
	};

	std::shared_ptr<Cubemap> createCubemapSp(const std::string cubeMapFaces[6]);
	std::shared_ptr<Cubemap> createCubemapSp(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz);

	Cubemap* createCubemap(const std::string cubeMapFaces[6]);
	Cubemap* createCubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz);
}

#endif