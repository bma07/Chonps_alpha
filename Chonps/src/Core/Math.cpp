#include "cepch.h"
#include "Math.h"

#include <cmath>

namespace Chonps
{
	double radians(const double degrees)
	{
		return degrees * CHONPS_RAD;
	}

	double degrees(const double radians)
	{
		return radians * (180 / CHONPS_PI);
	}

	constexpr float Q_rsqrt(float number) noexcept
	{
		static_assert(std::numeric_limits<float>::is_iec559); // (enable only on IEEE 754)

		const float n = std::bit_cast<float>(
			0x5f3759df - (std::bit_cast<std::uint32_t>(number) >> 1));
		return n * (1.5f - (number * 0.5f * n * n));
	}

	constexpr int floor(double num)
	{
		return int(num);
	}

	constexpr int ceil(double num)
	{
		int inum = (int)num;
		num == float(inum) ? inum : inum += 1;
		return inum;
	}

	int randomi(int min, int max)
	{
		return (rand() % (max - min + 1)) + min;
	}

	int random(int max)
	{
		return rand() % max;
	}

	float randomf(float min, float max)
	{
		return min + ((float)rand() / (float)RAND_MAX) * (max - min);
	}

	double hypotenuse(double l1, double l2)
	{
		return std::sqrt(std::pow(l1, 2) + std::pow(l2, 2));
	}

	double hleg(double leg, double hyp)
	{
		return std::sqrt(std::pow(hyp, 2) - std::pow(leg, 2));
	}

	double circle_area(double radius)
	{
		return CHONPS_PI * std::pow(radius, 2);
	}

	double circle_circumference(double radius)
	{
		return CHONPS_PI * radius * 2;
	}

	double cone_area(double radius, double height)
	{
		return CHONPS_PI * radius * (radius + std::sqrt(std::pow(height, 2) + std::pow(radius, 2)));
	}
	double sphere_area(double radius)
	{
		return 4 * CHONPS_PI * std::pow(radius, 2);
	}
	unsigned int factorial(unsigned int n)
	{
		if (n == 0 || n == 1)
			return 1;

		unsigned int result = 1;

		for (unsigned int i = 2; i <= n; i++)
			result *= i;

		return result;
	}

	pair<double, double> midpoint(double x1, double y1, double x2, double y2)
	{
		return { (x1 + x2) / 2, (y1 + y2) / 2};
	}

	double distance(double x1, double y1, double x2, double y2)
	{
		return std::sqrt(std::pow((x1 - x2), 2) + std::pow((y1 - y2), 2));
	}

	unsigned int permutation(unsigned int n, unsigned int r)
	{
		if (r > n)
		{
			CHONPS_CORE_LOG_ERROR(Arithmetic, "Permutation: r ({0}) greater than n ({1}). r must not be greater than n", r, n);
			return 0;
		}

		return factorial(n) / factorial(n - r);
	}

	unsigned int combination(unsigned int n, unsigned int r)
	{
		if (r > n)
		{
			CHONPS_CORE_LOG_ERROR(Arithmetic, "Combination: r ({0}) greater than n ({1}). r must not be greater than n", r, n);
			return 0;
		}

		return factorial(n) / (factorial(r) * factorial(n - r));
	}

	double derivative(std::function<double(double)> func, double x, double delta)
	{
		// Calculate the derivative using the central difference formula
		double fxph = func(x + delta);
		double fxmh = func(x - delta);
		return (fxph - fxmh) / (2.0 * delta);
	}
}