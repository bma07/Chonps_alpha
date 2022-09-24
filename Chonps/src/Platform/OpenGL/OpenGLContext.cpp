#include "cepch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Chonps
{
	static bool s_GladInit = false;

	void gladInit(GLFWwindow* window)
	{
		glfwMakeContextCurrent(window);

		if (!s_GladInit)
		{
			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			CHONPS_CORE_ASSERT(status, "Failed to initialize Glad!");

			CHONPS_CORE_INFO("OpenGL Info:");
			CHONPS_CORE_INFO("	Vendor: {0}", (const char*)glGetString(GL_VENDOR));
			CHONPS_CORE_INFO("	Renderer: {0}", (const char*)glGetString(GL_RENDERER));
			CHONPS_CORE_INFO("	Version: {0}", (const char*)glGetString(GL_VERSION));

			s_GladInit = true;
		}
	}
}