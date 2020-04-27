#pragma once

#include "rtweekend.h"
#include "hittable.h"

class moving_sphere : public hittable
{
	public:
		moving_sphere() {}
		moving_sphere(vec3 cen0, vec3 cen1, double t0, double t1, double r, shared_ptr<material> m)
			: center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m)
		{}

		virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const;

		vec3 center(double time) const;


	public:
		vec3 center0; // center at time0
		vec3 center1; // center at time1
		double time0;
		double time1;
		double radius;
		shared_ptr<material> mat_ptr;
};

// Center moves linearly from center0 at time0 to center1 at time1
vec3 moving_sphere::center(double time) const
{
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    vec3 oc = r.origin() - center(r.time());
    double a = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c = oc.length_squared() - radius * radius;
    double discriminant = half_b * half_b - a * c;

    if (discriminant > 0)
    {
        double root = sqrt(discriminant);

        double temp = (-half_b - root) / a;
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;

            // get uv coordinates (expects things on the unit sphere (divided by radius) centered at the origin (minus center))
            get_sphere_uv((rec.p - center(r.time())) / radius, rec.u, rec.v);
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.p - center(r.time())) / radius, rec.u, rec.v);
            return true;
        }
    }
    return false;
}

// For moving sphere, we can take the box of the sphere at time0, and the box of the sphere at time1,
// and compute the box of those two boxes:
bool moving_sphere::bounding_box(double time0, double time1, aabb& output_box) const
{
    aabb box0(
        center(time0) - vec3(radius, radius, radius),
        center(time0) + vec3(radius, radius, radius)
    );
    aabb box1(
        center(time1) - vec3(radius, radius, radius),
        center(time1) + vec3(radius, radius, radius)
    );
    output_box = surrounding_box(box0, box1);
    return true;
}
