#ifndef HG_CHONPS_MATH_H
#define HG_CHONPS_MATH_H

#include "DataTypes.h"

#define CHONPS_PI		3.14159265358
#define CHONPS_RAD		0.01745329251
#define CHONPS_EULER	2.71828182845
#define CHONPS_PHI		1.61803398875

namespace Chonps
{
	// Convert degrees to radians
	double radians(const double degrees);

	// Convert radians to degrees
	double degrees(const double radians);

	// Fast sqrt algorithm
	constexpr float Q_rsqrt(float number) noexcept;

	constexpr int floor(double num);

	constexpr int ceil(double num);

	// Random integer between min and max value
	int randomi(int min, int max);

	// Random integer from 0 to max value
	int random(int max);

	// Random decimal number between min and max values (ex. between 0 and 1)
	float randomf(float min, float max);
	
	// Find hypotenuse of right triangle using pythagoras theorem
	double hypotenuse(double l1, double l2);

	// Find leg of right triangle using pythagoras theorem given the hypotenuse and one of the legs
	double hleg(double leg, double hyp);

	double circle_area(double radius);

	double circle_circumference(double radius);

	double cone_area(double radius, double height);

	double sphere_area(double radius);

	// Calculates the factorial of the value, only accepts positive integers
	unsigned int factorial(unsigned int n);

	pair<double, double> midpoint(double x1, double y1, double x2, double y2);

	double distance(double x1, double y1, double x2, double y2);

	unsigned int permutation(unsigned int n, unsigned int r);

	unsigned int combination(unsigned int n, unsigned int r);

	// Approximates the deravitave value of the functions at point x using central difference formula
	// func must have parameter of double and return type double
	// x is the value given to func
	// delta is the step value of precision
	double derivative(std::function<double(double)> func, double x, double delta = 0.0001);

	// Checks if a number is within the range of another number by an amount
	template<typename T>
	bool within(T num, T within, T range)
	{
		return num <= (within + range) && num >= (within - range);
	}

	template <typename T>
	constexpr T min(T num1, T num2)
	{
		return num1 < num2 ? num1 : num2;
	}

	template <typename T>
	constexpr T max(T num1, T num2)
	{
		return num1 > num2 ? num1 : num2;
	}

	template <typename T>
	constexpr T abs(T num)
	{
		return num > 0 ? num : num * -1;
	}

	template <typename T>
	constexpr T round(T num)
	{
		return T(double(num + 0.5));
	}
}

#endif