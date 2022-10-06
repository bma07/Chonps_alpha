#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#define DATA_TYPE_ASSERT(t, i) CHONPS_CORE_WARN("WANRING: DATA_TYPES: {0} - Index out of bounds: {1}", t, i)

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
		unsigned int length() { return 1; }
		T& operator[](unsigned int i)
		{
			switch (i)
			{
				case 0: return x;
			}
			DATA_TYPE_ASSERT("vec1_t", i);
		}
		vec1_t operator+(const vec1_t& vec1) { return { this->x + vec1.x }; }
		vec1_t operator-(const vec1_t& vec1) { return { this->x - vec1.x }; }
		vec1_t operator*(const vec1_t& vec1) { return { this->x * vec1.x }; }
		vec1_t operator/(const vec1_t& vec1) { return { this->x / vec1.x }; }
	};

	template<typename T>
	struct vec2_t
	{
		vec2_t() {}
		vec2_t(T x, T y)
			: x(x), y(y) {}
		vec2_t(T n)
			: x(n), y(n) {}
		T x, y;
		unsigned int length() { return 2; }
		T& operator[](unsigned int i)
		{
			switch (i)
			{
				case 0: return x;
				case 1: return y;
			}
			DATA_TYPE_ASSERT("vec2_t", i);
		}
		vec2_t operator+(const vec2_t& vec2) { return { this->x + vec2.x, this->y + vec2.y }; }
		vec2_t operator-(const vec2_t& vec2) { return { this->x - vec2.x, this->y - vec2.y }; }
		vec2_t operator*(const vec2_t& vec2) { return { this->x * vec2.x, this->y * vec2.y }; }
		vec2_t operator/(const vec2_t& vec2) { return { this->x / vec2.x, this->y / vec2.y }; }
	};

	template<typename T>
	struct vec3_t
	{
		vec3_t() {}
		vec3_t(T x, T y, T z)
			: x(x), y(y), z(z) {}
		vec3_t(T n)
			: x(n), y(n), z(n) {}
		T x, y, z;
		unsigned int length() { return 3; }
		T& operator[](unsigned int i)
		{
			switch (i)
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
			}
			DATA_TYPE_ASSERT("vec3_t", i);
		}
		vec3_t operator+(const vec3_t& vec3) { return { this->x + vec3.x, this->y + vec3.y, this->z + vec3.z }; }
		vec3_t operator-(const vec3_t& vec3) { return { this->x - vec3.x, this->y - vec3.y, this->z - vec3.z }; }
		vec3_t operator*(const vec3_t& vec3) { return { this->x * vec3.x, this->y * vec3.y, this->z * vec3.z }; }
		vec3_t operator/(const vec3_t& vec3) { return { this->x / vec3.x, this->y / vec3.y, this->z / vec3.z }; }
	};

	template<typename T>
	struct vec4_t
	{
		vec4_t() {}
		vec4_t(T x, T y, T z, T w)
			: x(x), y(y), z(z), w(w) {}
		vec4_t(T n)
			: x(n), y(n), z(n), w(n) {}
		T x, y, z, w;
		unsigned int length() { return 4; }
		T& operator[](unsigned int i)
		{
			switch (i)
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
			}
			DATA_TYPE_ASSERT("vec4_t", i);
		}
		vec4_t operator+(const vec4_t& vec4) { return { this->x + vec4.x, this->y + vec4.y, this->z + vec4.z, this->w + vec4.w }; }
		vec4_t operator-(const vec4_t& vec4) { return { this->x - vec4.x, this->y - vec4.y, this->z - vec4.z, this->w - vec4.w }; }
		vec4_t operator*(const vec4_t& vec4) { return { this->x * vec4.x, this->y * vec4.y, this->z * vec4.z, this->w * vec4.w }; }
		vec4_t operator/(const vec4_t& vec4) { return { this->x / vec4.x, this->y / vec4.y, this->z / vec4.z, this->w / vec4.w }; }
	};

	// Standard float Vectors
	typedef vec1_t<float> vec1;
	typedef vec2_t<float> vec2;
	typedef vec3_t<float> vec3;
	typedef vec4_t<float> vec4;

	// Float Vectors
	typedef vec1_t<float> vec1f;
	typedef vec2_t<float> vec2f;
	typedef vec3_t<float> vec3f;
	typedef vec4_t<float> vec4f;

	// Int Vectors
	typedef vec1_t<int> vec1i;
	typedef vec2_t<int> vec2i;
	typedef vec3_t<int> vec3i;
	typedef vec4_t<int> vec4i;

	// Unsigned Int Vectors
	typedef vec1_t<unsigned int> vec1ui;
	typedef vec2_t<unsigned int> vec2ui;
	typedef vec3_t<unsigned int> vec3ui;
	typedef vec4_t<unsigned int> vec4ui;

	// Double Vectors
	typedef vec1_t<double> vec1d;
	typedef vec2_t<double> vec2d;
	typedef vec3_t<double> vec3d;
	typedef vec4_t<double> vec4d;

	// Boolean Vectors
	typedef vec1_t<bool> vec1b;
	typedef vec2_t<bool> vec2b;
	typedef vec3_t<bool> vec3b;
	typedef vec4_t<bool> vec4b;

	// Vertex data type
	struct vertex
	{
		vec3 position;
		vec3 color;
		vec3 normal;
		vec2 texUV;
	};
	

	// Matrix Data Types
	template<typename T>
	struct mat2_t
	{
		mat2_t() {}
		mat2_t(vec2_t<T> x, vec2_t<T> y) 
			: x(x), y(y) {}
		mat2_t(
			T x1, T x2,
			T y1, T y2)
			: x(x1, x2), y(y1, y2) {}
		mat2_t(T n)
			: x(n, 0), y(0, n) {}

		vec2_t<T> x, y;
		unsigned int length() { return 2; }
		T& operator[](unsigned int i)
		{
			switch (i)
			{
				case 0: return x[i];
				case 1: return y[i];
			}
			DATA_TYPE_ASSERT("mat2_t", i);
		}
	};

	template<typename T>
	struct mat3_t
	{
		mat3_t() {}
		mat3_t(vec3_t<T> x, vec3_t<T> y, vec3_t<T> z)
			: x(x), y(y), z(z) {}
		mat3_t(
			T x1, T x2, T x3,
			T y1, T y2, T y3,
			T z1, T z2, T z3)
			: x(x1, x2, x3), y(y1, y2, y3), z(z1, z2, z3) {}
		mat3_t(T n)
			: x(n, 0, 0), y(0, n, 0), z(0, 0, n) {}

		vec3_t<T> x, y, z;
		unsigned int length() { return 3; }
		T& operator[](unsigned int i)
		{
			switch (i)
			{
				case 0: return x[i];
				case 1: return y[i];
				case 2: return z[i];
			}
			DATA_TYPE_ASSERT("mat3_t", i);
		}
	};

	template<typename T>
	struct mat4_t
	{
		mat4_t() {}
		mat4_t(vec4_t<T> x, vec4_t<T> y, vec4_t<T> z, vec4_t<T> w)
			: x(x), y(y), z(z), w(w) {}
		mat4_t(
			T x1, T x2, T x3, T x4,
			T y1, T y2, T y3, T y4, 
			T z1, T z2, T z3, T z4,
			T w1, T w2, T w3, T w4)
			: x(x1, x2, x3), y(y1, y2, y3), z(z1, z2, z3), w(w1, w2, w3, w4) {}
		mat4_t(T n)
			: x(n, 0, 0, 0), y(0, n, 0, 0), z(0, 0, n, 0), w(0, 0, 0, n) {}

		vec4_t<T> x, y, z, w;
		unsigned int length() { return 4; }
		vec4_t<T>& operator[](unsigned int i)
		{
			switch (i)
			{
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
			}
			DATA_TYPE_ASSERT("mat4_t", i);
		}
	};

	// Standard Float Matrices
	typedef mat2_t<float> mat2;
	typedef mat3_t<float> mat3;
	typedef mat4_t<float> mat4;
}

#endif