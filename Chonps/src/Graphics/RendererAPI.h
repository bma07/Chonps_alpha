#ifndef RENDERER_API_H
#define RENDERER_API_H

namespace Chonps
{
	enum class API
	{
		None = 0,
		OpenGL,
		Vulkan,
		DirectX
	};

	bool renderInit(API api = API::OpenGL);

	API getGraphicsContext();

	std::string getGraphicsContextName();
}

#endif