#ifndef CORE_H
#define CORE_H


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#define CHONPS_PLATFORM_WINDOWS
	#ifdef CHONPS_PLATFORM_WINDOWS
		#ifdef CHONPS_SHARED_LIBRARY
			#define CHONPS_API __declspec(dllexport)
			#define CHONPS_API_IMPORT __declspec(dllimport)
		#endif
	#endif

#elif __APPLE__
	#define CHONPS_PLATFORM_APPLE

#elif __linux__
	#define CHONPS_PLATFORM_LINUX

#else
	#error "Chonps does not support the current platform."
#endif


#define BIT(x) (x << 1)

#endif