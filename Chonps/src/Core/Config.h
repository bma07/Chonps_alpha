#ifndef HG_CHONPS_CONFIG_H
#define HG_CHONPS_CONFIG_H

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


#ifdef _MSC_VER
	#define CHONPS_ASSERT_BREAK __debugbreak()
	#pragma warning (disable : 4267)
	#pragma warning (disable : 4244)
#else
	#define CHONPS_ASSERT_BREAK assert(false);
#endif


// Debug
#ifdef CHONPS_DEBUG
	#define CHONPS_ENABLE_ASSERTS 
#endif

#ifdef CHONPS_ENABLE_ASSERTS
	#define CHONPS_ASSERT(x, ...) { if(!(x)) { CHONPS_ERROR("Assertion Failed: {0}", __VA_ARGS__); CHONPS_ASSERT_BREAK; } }
	#define CHONPS_CORE_ASSERT(x, ...) { if(!(x)) { CHONPS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); CHONPS_ASSERT_BREAK; } }
#else	
	#define CHONPS_ASSERT(x, ...) { if(!(x)) { CHONPS_ERROR("ERROR: {0}", __VA_ARGS__); } }
	#define CHONPS_CORE_ASSERT(x, ...) { if(!(x)) { CHONPS_CORE_ERROR("ERROR: {0}", __VA_ARGS__); } }
#endif

// Warnings
#ifdef CHONPS_DEBUG

#endif

// Specification
#ifndef CHONPS_FORCE_GRAPHICS_API_NAMESPACES
	#define CHONPS_GRAPHICS_API_NAMESPACES_NOT_REQUIRED
#endif


#define CHONPS_TRUE 1
#define CHONPS_FALSE 0

#define CHONPS_UINT8_MAX  0xff
#define CHONPS_UINT16_MAX 0xffff
#define CHONPS_UINT32_MAX 0xffffffff
#define CHONPS_UINT64_MAX 0xffffffffffffffff

#define ALIGN(x) alignas(x)
#define BIT(x) (1 << x)
#define SCAST(t, x) static_cast<t>(x)
#define CASTUI(x) SCAST(uint32_t, x)
#define CASTF(x) SCAST(float, x)

namespace Chonps
{
	template <typename T>
	using Shared = std::shared_ptr<T>;
	template <typename T, typename... Args>
	constexpr Shared<T> CreateSharedPtr(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Unique = std::unique_ptr<T>;
	template <typename T, typename... Args>
	constexpr Unique<T> CreateUniquePtr(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}


#endif