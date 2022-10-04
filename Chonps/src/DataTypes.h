#ifndef VECTORS_H
#define VECTORS_H

namespace Chonps
{
	// Vector Data Types
	template<typename T>
	struct vec1_t
	{
		vec1_t() {}
		vec1_t(T x)
			: x(x) {}
		T x;
	};
	
	template<typename T>
	struct vec2_t
	{
		vec2_t() {}
		vec2_t(T x, T y)
			: x(x), y(y) {}
		T x, y;
	};

	template<typename T>
	struct vec3_t
	{
		vec3_t() {}
		vec3_t(T x, T y, T z)
			: x(x), y(y), z(z) {}
		T x, y, z;
	};

	template<typename T>
	struct vec4_t
	{
		vec4_t() {}
		vec4_t(T x, T y, T z, T w)
			: x(x), y(y), z(z), w(w) {}
		T x, y, z, w;
	};

	typedef vec1_t<float> vec1;
	typedef vec2_t<float> vec2;
	typedef vec3_t<float> vec3;
	typedef vec4_t<float> vec4;

	// Vertex data type
	struct vertex
	{
		vec3 position;
		vec3 color;
		vec3 normal;
		vec2 texUV;
	};
	
	// Precise data types
	struct vec2i
	{
		vec2i() 
			: x(0), y(0) {}
		vec2i(int x, int y)
			: x(x), y(y) {}
		int x, y;
	};

	struct vec3i
	{
		vec3i()
			: x(0), y(0), z(0) {}
		vec3i(int x, int y, int z)
			: x(x), y(y), z(z) {}
		int x, y, z;
	};

	struct vec4i
	{
		vec4i()
			: x(0), y(0), z(0), w(0) {}
		vec4i(int x, int y, int z, int w)
			: x(x), y(y), z(z), w(w) {}
		int x, y, z, w;
	};

	struct vec2ui
	{
		vec2ui()
			: x(0), y(0) {}
		vec2ui(unsigned int x, unsigned int y)
			: x(x), y(y) {}
		unsigned int x, y;
	};

	struct vec3ui
	{
		vec3ui()
			: x(0), y(0), z(0) {}
		vec3ui(unsigned int x, unsigned int y, unsigned int z)
			: x(x), y(y), z(z) {}
		unsigned int x, y, z;
	};

	struct vec4ui
	{
		vec4ui()
			: x(0), y(0), z(0), w(0) {}
		vec4ui(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
			: x(x), y(y), z(z), w(w) {}
		unsigned int x, y, z, w;
	};

	struct vec2f
	{
		vec2f()
			: x(0.0f), y(0.0f) {}
		vec2f(float x, float y)
			: x(x), y(y) {}
		float x, y;
	};

	struct vec3f
	{
		vec3f()
			: x(0.0f), y(0.0f), z(0.0f) {}
		vec3f(float x, float y, float z)
			: x(x), y(y), z(z) {}
		float x, y, z;
	};

	struct vec4f
	{
		vec4f()
			: x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		vec4f(float x, float y, float z, float w)
			: x(x), y(y), z(z), w(w) {}
		float x, y, z, w;
	};

	struct vec2d
	{
		vec2d()
			: x(0.0), y(0.0) {}
		vec2d(double x, double y)
			: x(x), y(y) {}
		double x, y;
	};

	struct vec3d
	{
		vec3d()
			: x(0.0), y(0.0), z(0.0) {}
		vec3d(double x, double y, double z)
			: x(x), y(y), z(z){}
		double x, y, z;
	};

	struct vec4d
	{
		vec4d()
			: x(0.0), y(0.0), z(0.0), w(0.0) {}
		vec4d(double x, double y, double z, double w)
			: x(x), y(y), z(z), w(w) {}
		double x, y, z, w;
	};

	struct vec2b
	{
		vec2b()
			: x(false), y(false) {}
		vec2b(bool x, bool y)
			: x(x), y(y) {}
		bool x, y;
	};

	struct vec3b
	{
		vec3b()
			: x(false), y(false), z(false) {}
		vec3b(bool x, bool y, bool z)
			: x(x), y(y), z(z) {}
		bool x, y, z;
	};

	struct vec4b
	{
		vec4b()
			: x(false), y(false), z(false), w(false) {}
		vec4b(bool x, bool y, bool z, bool w)
			: x(x), y(y), z(z), w(w) {}
		bool x, y, z, w;
	};
}

#endif