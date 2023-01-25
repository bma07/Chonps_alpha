#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include "Core/Config.h"

struct GLFWwindow;

namespace Chonps
{
	void gladInit(GLFWwindow* window, unsigned int width, unsigned int height);

	void gladUpdateViewPort(GLFWwindow* window, unsigned int width, unsigned int height);
}



#endif