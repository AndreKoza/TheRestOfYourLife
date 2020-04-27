#pragma once

#include "rtweekend.h"
#include "texture.h"


struct hit_record;

// Reflectivity varies with angle -> this is an approximation for that (haven't looked into that)
double schlick(double cosine, double ref_idx)
{
    double r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

class material
{
    public:
        virtual vec3 emitted(double u, double v, const vec3& p) const
        {
            return Color::black;
        }

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};


class lambertian : public material
{
    public:
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
        {
            vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            scattered = ray(rec.p, target-rec.p, r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }


    private:
        shared_ptr<texture> albedo;
};


class metal : public material
{
    public:
        metal(const vec3& a, double f = 0) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
        {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            // See definition of dot product: If dot product > 0 -> angle is sharp (spitzer Winkel)
            return (dot(scattered.direction(), rec.normal) > 0);
        }

        vec3 albedo;
        double fuzz;
};


class dielectric : public material
{
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
        {
            attenuation = vec3(1.0, 1.0, 1.0);
            double etai_over_etat = rec.front_face ? (1.0 / ref_idx) : (ref_idx);

            vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = min(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            if (etai_over_etat * sin_theta > 1.0)
            {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }

            double reflect_prob = schlick(cos_theta, etai_over_etat);
            if (random_double() < reflect_prob)
            {
                vec3 reflected = reflect(unit_direction, rec.normal);
                scattered = ray(rec.p, reflected);
                return true;
            }

            vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
            scattered = ray(rec.p, refracted);
            return true;
        }

        double ref_idx;
};

class diffuse_light : public material
{
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
        {
            return false;
        }

        virtual vec3 emitted(double u, double v, const vec3& p) const
        {
            return emit->value(u, v, p);
        }


    private:
        shared_ptr<texture> emit;
};

class isotropic : public material
{
    public:
        isotropic(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
        {
            scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    private:
        shared_ptr<texture> albedo;
};
