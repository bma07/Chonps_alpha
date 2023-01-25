#ifndef CHONPS_CONFIG_H
#define CHONPS_CONFIG_H

#include <memory>

// Platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#ifndef CHONPS_PLATFORM_WINDOWS
		#define CHONPS_PLATFORM_WINDOWS
	#endif
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
#elif CHONPS_USE_DIRECTX
	#define CHONPS_DIRECTX_API
#else
	#define CHONPS_OPENGL_API
#endif


// Debug
#ifdef CHONPS_DEBUG
	#define CHONPS_ENABLE_ASSERTS 
#endif

#ifdef CHONPS_ENABLE_ASSERTS
	#define CHONPS_ASSERT(x, ...) { if(!(x)) { CHONPS_ERROR("Assertion Failed: {0}", __VA_ARGS__); assert(false); } }
	#define CHONPS_CORE_ASSERT(x, ...) { if(!(x)) { CHONPS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); assert(false); } }
#else
	#define CHONPS_ASSERT(x, ...)
	#define CHONPS_CORE_ASSERT(x, ...)
#endif

#define CHONPS_TRUE 1
#define CHONPS_FALSE 0

#define CHONPS_PI 3.1415926535897932384626433832795028841971693993751
#define CHONPS_RAD (3.1415926535897932384626433832795028841971693993751 / 180)

namespace Chonps
{
	template <typename T>
	using Shared = std::shared_ptr<T>;
	template <typename T, typename... Args>
	constexpr Shared<T> createSharedPtr(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Unique = std::unique_ptr<T>;
	template <typename T, typename... Args>
	constexpr Unique<T> createUniquePtr(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}


#endif