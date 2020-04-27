#pragma once

#include "rtweekend.h"
#include "perlin.h"

class texture
{
	public:
		virtual vec3 value(double u, double v, const vec3& p) const = 0;
};

class noise_texture : public texture
{
	public:
		noise_texture() {}
		noise_texture(double sc) : scale(sc) {}

		virtual vec3 value(double u, double v, const vec3& p) const
		{
			/*
			The output of the perlin interpretation can return negative values. 
			These negative values will be passed to the sqrt() function of our gamma 
			function and get turned into NaNs. We will cast the perlin output back to between 0 and 1.

			Regarding sine: The basic idea is to make color proportional to something like a sine function, 
			and use turbulence to adjust the phase (so it shifts x in sin(x)) which makes the stripes undulate.
			Commenting out straight noise and turbulence, and giving a marble-like effect is:
			*/
			// scale * p.z() is dependent on view direction. For final scene from book two, should be changed to p.x()
			// return vec3(1, 1, 1) * 0.5 * (1.0 + sin(scale*p.z() + 10*noise.turb(p)));

			// "Pure" Perlin noise (no marble-like texture):
			 return vec3(1, 1, 1) * noise.turb(scale * p);
			// return vec3(1, 1, 1) * 0.5 * (1 + noise.turb(scale * p));
		}

	private:
		perlin noise;
		double scale; // acts as noise frequency / multiplicator
};

class constant_texture : public texture
{
	public:
		constant_texture(vec3 c) : color(c) {}

		// value for constant_texture just retuns the color and ignores u, v and p
		virtual vec3 value(double u, double v, const vec3& p) const
		{
			return color;
		}


	private:
		vec3 color;
};

class checker_texture : public texture
{
	public:
		checker_texture(shared_ptr<texture> t0, shared_ptr<texture> t1)
			: even(t0), odd(t1)
		{}

		/*	We can create a checker texture by noting that the sign of sine and cosine just alternates 
			in a regular way and if we multiply trig functions in all three dimensions, the sign of that 
			product forms a 3D checker pattern. */
		virtual vec3 value(double u, double v, const vec3& p) const
		{
			auto sines = sin(10 * p.x())* sin(10 * p.y())* sin(10 * p.z());
			if (sines < 0)
				return odd->value(u, v, p);
			else
				return even->value(u, v, p);
		}


	private:
		shared_ptr<texture> even;
		shared_ptr<texture> odd;
};

class image_texture : public texture
{
	public:
		image_texture(unsigned char* pixels, int A, int B)
			: data(pixels), nx(A), ny(B) {}

		~image_texture()
		{
			delete data;
		}

		virtual vec3 value(double u, double v, const vec3& p) const
		{
			// If we have no texture data, then always emit cyn (as a debugging aid)
			if (data == nullptr)
			{
				return vec3(0, 1, 1);
			}

			auto i = static_cast<int>(u * nx);
			auto j = static_cast<int>((1 - v) * ny - epsilon);

			i = std::clamp(i, 0, nx - 1);
			j = std::clamp(j, 0, ny - 1);

			auto r = static_cast<int>(data[3 * i + 3 * nx * j + 0]) / 255.0;
			auto g = static_cast<int>(data[3 * i + 3 * nx * j + 1]) / 255.0;
			auto b = static_cast<int>(data[3 * i + 3 * nx * j + 2]) / 255.0;

			return vec3(r, g, b);
		}

	private:
		unsigned char* data; // image data is stored as array of unsigned char
		int nx;
		int ny;
};
