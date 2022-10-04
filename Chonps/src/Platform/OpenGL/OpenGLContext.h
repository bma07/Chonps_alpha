#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include "Core.h"

struct GLFWwindow;

namespace Chonps
{
	void gladInit(GLFWwindow* window, unsigned int width, unsigned int height);
}



#endif