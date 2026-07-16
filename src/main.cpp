#include <iostream>
#include <glm/glm.hpp>
#include "Ray.h"
#include "Color.h"
#include "Object.h"

Color ray_color(const Ray& ray, const Sphere& sphere) {
  if (sphere.hit(ray)) {
    return Color(0.8, 0.7, 0.1);
  }
  Vec3 unit_dir = glm::normalize(ray.direction());
  double a = 0.5*(unit_dir.y + 1.0);
  return (1-a) * Color(1, 1, 1) + a * Color(0.5, 0.7, 1.0);
}

int main() {
  const double aspect_ratio = 16.0 / 9.0;

  const int IMAGE_WIDTH = 400;
  const int IMAGE_HEIGHT = std::max((int)(IMAGE_WIDTH / aspect_ratio), 1);

  // Viewport Parameters

  const double FOCAL_LENGTH = 1.0;
  const double VIEWPORT_HEIGHT = 2.0;
  const double VIEWPORT_WIDTH = VIEWPORT_HEIGHT * (double)IMAGE_WIDTH / (double)IMAGE_HEIGHT;
  const Vec3 CAMERA_CENTER = Vec3(0, 0, 0);

  const Vec3 viewport_u = Vec3(VIEWPORT_WIDTH, 0, 0);
  const Vec3 viewport_v = Vec3(0, -VIEWPORT_HEIGHT, 0);
  const Vec3 viewport_du = viewport_u / (double)IMAGE_WIDTH;
  const Vec3 viewport_dv = viewport_v / (double)IMAGE_HEIGHT;

  const Vec3 upper_left = CAMERA_CENTER - Vec3(0, 0, FOCAL_LENGTH) - 0.5 * (viewport_u + viewport_v);
  const Vec3 first_pixel_location = upper_left + 0.5 * (viewport_du + viewport_dv);

  // Render

  std::cout << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";

  for (int j = 0; j < IMAGE_HEIGHT; j++) {
    std::clog << "\rScanlines remaining: " << (IMAGE_HEIGHT - j) << ' ' << std::flush;

    for (int i = 0; i < IMAGE_WIDTH; i++) {
      Vec3 pixel_center = first_pixel_location + (double)i * viewport_du + (double)j * viewport_dv;
      Vec3 ray_dir = pixel_center - CAMERA_CENTER;
      Ray ray(CAMERA_CENTER, ray_dir);
      Color pixel_color = ray_color(ray, Sphere(Vec3(0, 0, -1), 0.5));
      write_color(std::cout, pixel_color);
    }
  }
  std::clog << "\rDone.                 \n";
  return 0;
}