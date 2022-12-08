#include "cepch.h"
#include "Cubemap.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLCubemap.h"

namespace Chonps
{
	const char* Cubemap::m_CubemapProjectionUniformName = "cubemapProj";
	const char* Cubemap::m_CubemapViewUniformName = "cubemapView";

	std::shared_ptr<Cubemap> createCubemap(const std::string cubeMapFaces[6])
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createCubemap(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return std::make_shared<OpenGLCubemap>(cubeMapFaces);
				break;
			}

			case RenderAPI::Vulkan:
			{
				break;
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: CUBEMAP: Could not create Cubemap!");
		return nullptr;
	}

	std::shared_ptr<Cubemap> createCubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createCubemap(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return std::make_shared<OpenGLCubemap>(posx, negx, posy, negy, posz, negz);
				break;
			}

			case RenderAPI::Vulkan:
			{
				break;
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: CUBEMAP: Could not create Cubemap!");
		return nullptr;
	}

	Cubemap* createCubemapRp(const std::string cubeMapFaces[6])
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createCubemap(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLCubemap(cubeMapFaces);
				break;
			}

			case RenderAPI::Vulkan:
			{
				break;
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: CUBEMAP: Could not create Cubemap!");
		return nullptr;
	}

	Cubemap* createCubemapRp(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz)
	{
		switch (getGraphicsContext())
		{
			case RenderAPI::None:
			{
				CHONPS_CORE_WARN("WANRING: TEXTURE: createCubemap(cubeMapFaces) - No graphics API selected beforehand!");
				break;
			}

			case RenderAPI::OpenGL:
			{
				return new OpenGLCubemap(posx, negx, posy, negy, posz, negz);
				break;
			}

			case RenderAPI::Vulkan:
			{
				break;
			}

			case RenderAPI::DirectX:
			{
				break;
			}
		}
		CHONPS_CORE_ERROR("ERROR: CUBEMAP: Could not create Cubemap!");
		return nullptr;
	}
}