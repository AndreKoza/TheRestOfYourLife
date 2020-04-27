#pragma once

#include "rtweekend.h"
#include "hittable.h"
#include "texture.h"
#include "material.h"

class constant_medium : public hittable
{
	public:
		constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
			: boundary(b), neg_inv_density(-1 / d)
		{
			phase_function = make_shared<isotropic>(a);
		}

		virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;

		virtual bool bounding_box(double time0, double time1, aabb& output_box) const
		{
			return boundary->bounding_box(time0, time1, output_box);
		}

	private:
		shared_ptr<hittable> boundary;
		shared_ptr<material> phase_function;
		double neg_inv_density;
};

bool constant_medium::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	// Print occasional samples when debugging. To enable, set enableDebug true.
	const bool enableDebug = false;
	const bool debugging = enableDebug && random_double() < 0.00001;

	hit_record rec1;
	hit_record rec2;

	// Test if medium was hit at all (entry point)
	if (!boundary->hit(r, -infinity, infinity, rec1))
		return false;

	// Test if ray also exited the medium once it hit
	if (!boundary->hit(r, rec1.t + 0.0001, infinity, rec2))
		return false;

	if (debugging) std::cerr << '\nt0=' << rec1.t << ", t1=" << rec2.t << '\n';

	if (rec1.t < t_min) rec1.t = t_min;
	if (rec2.t > t_max) rec2.t = t_max;

	if (rec1.t >= rec2.t)
		return false;

	if (rec1.t < 0)
		rec1.t = 0;

	const auto ray_length = r.direction().length();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;

	/* Rays may scatter at any point. The denser the volume, the more likely that is. The probability
	   is proportional to the optical density of the volume. Compute the distance (where scattering occurs)
	   based on density and random number: */
	const auto hit_distance = neg_inv_density * log(random_double());

	if (hit_distance > distance_inside_boundary) // ... If that distance is outside the volume, then there is no “hit”
		return false;

	rec.t = rec1.t + hit_distance / ray_length;
	rec.p = r.at(rec.t);

	if (debugging)
	{
		std::cerr	<< "hit_distance = " << hit_distance << '\n'
					<< "rec.t = " << rec.t << '\n'
					<< "rec.p = " << rec.p << '\n';
	}

	rec.normal = vec3(1, 0, 0); // arbitrary
	rec.front_face = true;		// also arbitrary
	rec.mat_ptr = phase_function;

	return true;
}
