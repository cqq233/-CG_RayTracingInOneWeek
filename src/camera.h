#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include <iostream>
#include <fstream>

#define WHITE color(1.0, 1.0, 1.0)

class camera
{
public:

	//init image;
	double	aspect_ratio			= 1.0;
	int		image_width				= 100;
	int		samples_per_pixel		= 10;
	int		max_depth				= 10; // prevent ray_color() from recursing too much;

	double vfov = 90;
	point3 lookFrom = point3(0, 0, -1);
	point3 lookAt	= point3(0, 0,  0);
	vec3	vup		= vec3	(0, 1,  0);

	double defocus_angle = 0; // Variation angle of rays through each pixel
	double focus_dist = 10;	 // Distance from camera lookfrom point to plane of perfect focus

	void render(const hittable& world, std::ofstream& outfile)
	{
		initialized();
		outfile << "P3\n" << image_width << " " << image_height << "\n255\n";

		for (int j = 0; j < image_height; ++j)
		{
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			for (int i = 0; i < image_width; ++i)
			{
				color pixel_color(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; ++sample)
				{
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_depth, world);
				}
				set_color(outfile, pixel_color, samples_per_pixel);
			}
		}
		std::clog << "\rDone.                 \n";
	}
//----------------------------------------------------------------------------------------------------------
private:

	int image_height;		// Rendered image height;
	point3 center;			// Camera center;
	point3 pixel00_loc;		// location of the first pixel, as (0, 0);
	vec3 pixel_delta_u;		// unit length of y axis of image, from right to left;
	vec3 pixel_delta_v;		// as same as pixel_delta_y, but it is x axis;
	vec3 u, v, w;			// Camera frame basis vectors;
	vec3   defocus_disk_u;  // Defocus disk horizontal radius
	vec3   defocus_disk_v;  // Defocus disk vertical radius

	void initialized()
	{
		image_height = static_cast<int> (image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height; // calc height by aspect ratio, and ensure height >= 1

		//Camera
		center = lookFrom;
		//auto focal_length = (lookFrom - lookAt).length(); // just a unit length about the distances between carema and viewport;
		auto theta = degress_to_radians(vfov);
		auto h = tan(theta / 2);
		auto viewport_height = 2 * h * focus_dist;
		auto viewport_width = viewport_height * (static_cast<double> (image_width) / image_height);
		//why not to use aspect_ratio directly?
		// Note that aspect_ratio is an ideal ratio, 
		// which we approximate as best as possible with the integer-based ratio of image width over image height. 
		// In order for our viewport proportions to exactly match our image proportions, 
		// we use the calculated image aspect ratio to determine our final viewport width.

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		w = normalize(lookFrom - lookAt);
		u = normalize(cross(vup, w));
		v = cross(w, u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
		vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Calculate the location of the upper left pixel.
		auto viewport_upper_left = 
			center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
		// move camera into viewport, and translate camera to upper_left
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
		// translate the camera's coordinate to (0.5, 0.5);
		// 
		// Calculate the camera defocus disk basis vectors.
		auto defocus_radius = focus_dist * tan(degress_to_radians(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}

	ray get_ray(int i, int j)
	{
		// Get a randomly-sampled camera ray for the pixel at location i,j, originating from
		// the camera defocus disk.
		auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
		auto pixel_sample = pixel_center + pixel_sample_square();

		auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
	}

	vec3 pixel_sample_square() const 
	{
		auto px = -0.5 + random_double();
		auto py = -0.5 + random_double();
		return (px * pixel_delta_u) + (py * pixel_delta_v);

	}

	point3 defocus_disk_sample() const {
		// Returns a random point in the camera defocus disk.
		auto p = random_in_unit_disk();
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	color ray_color(const ray& r, int depth, const hittable& world) const
	{
		hit_record rec;

		if (depth <= 0)
		{
			return color(0, 0, 0);
		}

		if (world.hit(r, interval(0.001, INF), rec))
		{
			ray scattered;
			color attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered))
				return attenuation * ray_color(scattered, depth - 1, world);
			return color(0, 0, 0);
		}

		vec3 unit_direction = normalize(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
	}
};
#endif // !CAMERA_H

