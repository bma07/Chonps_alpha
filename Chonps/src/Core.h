#ifndef CONFIG_H
#define CONFIG_H


// Platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#undef CHONPS_PLATFORM_WINDOWS
	#define CHONPS_PLATFORM_WINDOWS
	#ifdef CHONPS_PLATFORM_WINDOWS
		#ifdef CHONPS_SHARED_LIBRARY
			#define CHONPS_API __declspec(dllexport)
			#define CHONPS_API_IMPORT __declspec(dllimport)
		#else 
			#define CHONPS_API
		#endif
	#endif

#elif __APPLE__
	#define CHONPS_PLATFORM_APPLE

#elif __linux__
	#define CHONPS_PLATFORM_LINUX

#else
	#error "Chonps does not support the current platform."
#endif

// Window Plaform API
#ifdef CHONPS_USE_WIN32
	#define CHONPS_WIN32_API
#else
	#define CHONPS_GLFW_API
#endif

// Graphics Platform API
#ifdef CHONPS_USE_VULKAN
	#define CHONPS_VULKAN_API
#elif CHONPS_DIRECTX_API
	#define CHONPS_DIRECTX_API
#else
	#define CHONPS_OPENGL_API
#endif


// Debug
#ifdef CHONPS_DEBUG
	#define CHONPS_ENABLE_ASSERTS 
#endif

#ifdef CHONPS_ENABLE_ASSERTS
	#define CHONPS_ASSERT(x, ...) { if(!(x)) { CHONPS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define CHONPS_CORE_ASSERT(x, ...) { if(!(x)) { CHONPS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define CHONPS_ASSERT(x, ...)
	#define CHONPS_CORE_ASSERT(x, ...)
#endif


#endif