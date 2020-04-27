#pragma once

#include "rtweekend.h"

class aabb
{
	public:
		aabb() {}

		// AABB can just be defined by min and max vector.
		aabb(const vec3& a, const vec3& b) { _min = a; _max = b; }

		vec3 min() const { return _min; }
		vec3 max() const { return _max; }

		// tmin and tmax are the min / max allowed values of rays from the ray equation
		// Implementation of ray-slab intersection
		bool hit(const ray& r, double tmin, double tmax) const
		{
			// iterate over the three axes
			for (int a = 0; a < 3; ++a)
			{
				/*
				Compute the intervals while taking care of division by zero / NaNs etc.
				Below is a more efficient version of:
				auto t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a],
                                (_max[a] - r.origin()[a]) / r.direction()[a]);
                auto t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a],
                                (_max[a] - r.origin()[a]) / r.direction()[a]);
                tmin = ffmax(t0, tmin);
                tmax = ffmin(t1, tmax);
				
				*/
				auto invD = 1.0f / r.direction()[a];
				auto t0 = (_min[a] - r.origin()[a]) * invD;
				auto t1 = (_max[a] - r.origin()[a]) * invD;
				if (invD < 0.0f)
					std::swap(t0, t1);
				
				/* Overlap function of intervals (f,F),(d,D) and (e,E).
					bool overlap(d, D, e, E, f, F)
						f = max(d, e)
						F = min(D, E)
						return (f < F)
				*/
				tmin = t0 > tmin ? t0 : tmin;
				tmax = t1 < tmax ? t1 : tmax;

				if (tmax <= tmin)
					return false;
			}
			return true;
		}

		vec3 _min;
		vec3 _max;
};

// Compute the surrounding box of two aabbs: Just take the min value of x,y,z and
// max value of x,y,z and construct the aabb out of that.
aabb surrounding_box(const aabb& box0, const aabb& box1)
{
	vec3 small 
		(min(box0.min().x(), box1.min().x()),
		 min(box0.min().y(), box1.min().y()),
		 min(box0.min().z(), box1.min().z()));
	vec3 big   
		(max(box0.max().x(), box1.max().x()),
		 max(box0.max().y(), box1.max().y()),
		 max(box0.max().z(), box1.max().z()));
	return aabb(small, big);
}
