#pragma once

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

class bvh_node : public hittable
{
	public:
		bvh_node();

		bvh_node(hittable_list& list, double time0, double time1)
			: bvh_node(list.objects, 0, list.objects.size(), time0, time1)
		{}

		bvh_node(
			std::vector<shared_ptr<hittable>>& objects,
			size_t start, size_t end, double time0, double time1);

		virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
		virtual bool bounding_box(double time0, double time1, aabb& output_box) const;

	public:
		// Children of node are generic hittable: Can be other nodes or leaves (spheres, etc...)
		shared_ptr<hittable> left;
		shared_ptr<hittable> right;
		aabb box;
};

// Returns true if min value of a's boxes is less than min value of b's boxes for given axis
inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis)
{
	aabb box_a;
	aabb box_b;

	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.min().e[axis] < box_b.min().e[axis];
}

bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
{
	return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
{
	return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
{
	return box_compare(a, b, 2);
}

// Constructs BVH: start and end arguments are needed for recursion arguments
// Goal: Division should be done well: Two children of a node should have smaller bounding boxes
// than their parent's bounding box (only for speed, not needed for correctness!)
bvh_node::bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end, double time0, double time1)
{
	// Randomly choose an axis and define comparator
	int axis = random_int(0, 2);
	auto comparator = (axis == 0) ? box_x_compare
					: (axis == 1) ? box_y_compare
								  : box_z_compare;

	// How many objects are in objects
	size_t object_span = end - start;

	// For simplicity: If there is only one element, duplicate it in each subtree.
	// Can be optimized but this way there are always two children
	if (object_span == 1)
	{
		left  = objects[start];
		right = objects[start];
	}
	// If two elements: Put one in each subtree and end recursion
	else if (object_span == 2)
	{
		if (comparator(objects[start], objects[start + 1]))
		{
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			left = objects[start + 1];
			right = objects[start];
		}
	}
	// Sort the primitives and construct tree recursively
	else
	{
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		auto mid = start + object_span / 2;
		left = make_shared<bvh_node>(objects, start, mid, time0, time1);
		right = make_shared<bvh_node>(objects, mid, end, time0, time1);
	}

	aabb box_left;
	aabb box_right;

	if (!left->bounding_box(time0, time1, box_left)
		|| !right->bounding_box(time0, time1, box_right))
	{
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}

	box = surrounding_box(box_left, box_right);
}

// Just return the box which is calculated during construction.
bool bvh_node::bounding_box(double time0, double time1, aabb& output_box) const
{
	output_box = box;
	return true;
}

// Check whether the box for the node is hit, and if so, check the children and sort out any details
bool bvh_node::hit(const ray& r, double tmin, double tmax, hit_record& rec) const
{
	if (!box.hit(r, tmin, tmax))
		return false;

	bool hit_left = left->hit(r, tmin, tmax, rec);
	bool hit_right = right->hit(r, tmin, hit_left ? rec.t : tmax, rec);

	return hit_left || hit_right;
}

// Alternative implementation, according to github issue should be faster. Could not verify...
//bool bvh_node::hit(const ray& r, double tmin, double tmax, hit_record& rec) const
//{
//	if (box.hit(r, tmin, tmax)) 
//	{
//		if (left->hit(r, tmin, tmax, rec)) 
//		{
//			right->hit(r, tmin, rec.t, rec);
//			return true;
//		}
//		else 
//		{
//			return right->hit(r, tmin, tmax, rec);
//		}
//	}
//	return false;
//}
