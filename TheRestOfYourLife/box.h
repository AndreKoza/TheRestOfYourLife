#pragma once

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "aarect.h"

class box : public hittable
{
	public:
		box(const vec3& p0, const vec3& p1, shared_ptr<material> mat);

		virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;

		virtual bool bounding_box(double time0, double time1, aabb& output_box) const
		{
			output_box = aabb(box_min, box_max);
			return true;
		}

	private:
		vec3 box_min;
		vec3 box_max;
		hittable_list sides;
};

box::box(const vec3& p0, const vec3& p1, shared_ptr<material> mat)
{
	box_min = p0;
	box_max = p1;

	sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat));
	sides.add(make_shared<flip_face>(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat)));

	sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat));
	sides.add(make_shared<flip_face>(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat)));

	sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat));
	sides.add(make_shared<flip_face>(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat)));
}

bool box::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	return sides.hit(r, t_min, t_max, rec);
}
