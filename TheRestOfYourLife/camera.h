#pragma once

#include "rtweekend.h"


class camera 
{
    public:
        camera() : camera(vec3(0, 0, -1), vec3(0, 0, 0), vec3(0, 1, 0), 40, 1, 0, 10) {}

        // vfov is top to bottom in degrees
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, double vfov, double aspect, double aperture, double focus_dist,
            double t0 = 0, double t1 = 0) 
        {
            origin = lookfrom;
            lens_radius = aperture / 2;
            time0 = t0;
            time1 = t1;

            double theta = degrees_to_radians(vfov);
            double half_height = tan(theta/2);
            double half_width = aspect * half_height;
            
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w,u);
            lower_left_corner = origin - half_width*focus_dist*u - half_height*focus_dist*v - focus_dist*w;
            horizontal = 2*half_width*focus_dist*u;
            vertical = 2*half_height*focus_dist*v;
        }

        ray get_ray(double s, double t) 
        {
            vec3 rd = lens_radius * random_in_unit_disc();
            vec3 offset = u * rd.x() + v * rd.y();
            // Randomly determine the ray between shutter open / close times
            return ray(
                origin + offset, 
                lower_left_corner + s*horizontal + t*vertical - origin - offset,
                random_double(time0, time1));
        }

        vec3 origin;
        vec3 u;
        vec3 v;
        vec3 w;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        double lens_radius;
        double time0, time1; // shutter open/close times
};
