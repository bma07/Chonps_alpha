#include "cepch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGLRendererAPI.h"

namespace Chonps
{
	static bool s_GladInit = false;

	void gladInit(GLFWwindow* window, unsigned int width, unsigned int height)
	{
		glfwMakeContextCurrent(window);

		if (!s_GladInit)
		{
			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			CHONPS_CORE_ASSERT(status, "Failed to initialize Glad!");

			CHONPS_CORE_INFO("OpenGL Info:");
			CHONPS_CORE_INFO("\tVendor: {0}", (const char*)glGetString(GL_VENDOR));
			CHONPS_CORE_INFO("\tRenderer: {0}", (const char*)glGetString(GL_RENDERER));
			CHONPS_CORE_INFO("\tVersion: {0}", (const char*)glGetString(GL_VERSION));

			s_GladInit = true;
		}
		glViewport(0, 0, width, height);

		setOglCurrentWindow(window);
	}

	void gladUpdateViewPort(GLFWwindow* window, unsigned int width, unsigned int height)
	{
		glfwMakeContextCurrent(window);
		glViewport(0, 0, width, height);
	}
}