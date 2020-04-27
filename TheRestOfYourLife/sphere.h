#pragma once

#include "rtweekend.h"
#include "hittable.h"


class sphere : public hittable 
{
    public:
        sphere() {}
        sphere(vec3 cen, double r, shared_ptr<material> m) 
            :   center(cen), radius(r), mat_ptr(m) {}

        virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
        virtual bool bounding_box(double time0, double time01, aabb& output_box) const;

        vec3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

// Sphere hit function derived from sphere equation + solving a quadratic equation with known formulas...
bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const 
{
    vec3 oc = r.origin() - center;
    double a = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c = oc.length_squared() - radius*radius;
    double discriminant = half_b * half_b - a*c;

    if (discriminant > 0) 
    {
        double root = sqrt(discriminant);

        double temp = (-half_b - root) / a;
        if (temp < t_max && temp > t_min) 
        {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;

            // get uv coordinates (expects things on the unit sphere (divided by radius) centered at the origin (minus center))
            get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) 
        {
            rec.t = temp;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
            return true;
        }
    }
    return false;
}

// This sphere does not move over time, so time variables can be ignored
bool sphere::bounding_box(double time0, double time1, aabb& output_box) const
{
    output_box = aabb(
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius)
    );
    return true;
}
