#pragma once

#include "rtweekend.h"


#include <iomanip>


void pi_main()
{
	int inside_circle = 0;
	int inside_circle_stratified = 0;
	int sqrt_N = 10000;
	for (int i = 0; i < sqrt_N; ++i)
	{
		for (int j = 0; j < sqrt_N; ++j)
		{
			auto x = random_double(-1, 1);
			auto y = random_double(-1, 1);
			if (x * x + y * y < 1)
				++inside_circle;

			x = 2 * ((i + random_double()) / sqrt_N) - 1;
			y = 2 * ((j + random_double()) / sqrt_N) - 1;
			if (x * x + y * y < 1)
				++inside_circle_stratified;
		}
	}
	auto N = std::pow(sqrt_N, 2);
	std::cout << std::fixed << std::setprecision(12);
	std::cout	<< "Regular    Estimate of Pi = " << 4 * double(inside_circle) / N << '\n'
				<< "Stratified Estimate of Pi = " << 4 * double(inside_circle_stratified) / N << '\n';
}
