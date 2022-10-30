#include "cepch.h"
#include "Cubemap.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLCubemap.h"

namespace Chonps
{
	const char* Cubemap::m_CubemapProjectionUniformName = "cubemapProj";
	const char* Cubemap::m_CubemapViewUniformName = "cubemapView";

	Cubemap* createCubemap(const std::string cubeMapFaces[6])
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createCubemap(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLCubemap(cubeMapFaces);
				break;
			}

			case API::Vulkan:
			{
				break;
			}

			case API::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: CUBEMAP: Could not create Cubemap!");
		return nullptr;
	}

	Cubemap* createCubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz)
	{
		switch (getGraphicsContext())
		{
			case API::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createCubemap(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}

			case API::OpenGL:
			{
				return new OpenGLCubemap(posx, negx, posy, negy, posz, negz);
				break;
			}

			case API::Vulkan:
			{
				break;
			}

			case API::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: CUBEMAP: Could not create Cubemap!");
		return nullptr;
	}
}