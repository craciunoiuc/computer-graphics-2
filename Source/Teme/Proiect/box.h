#ifndef BOX_H
#define BOX_H

#include "rtweekend.h"

#include "hittable.h"

#include <algorithm>

class box : public hittable {
public:
	box() {}

	box(point3 start, point3 stop, shared_ptr<material> m)
		: start(start), stop(stop), mat_ptr(m) {};

	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec) const override;


	point3 start;
	point3 stop;
	shared_ptr<material> mat_ptr;

};

bool box::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	const double EPS = 0.006f;
	point3 dist_start = (start - r.origin()) / r.direction();
	point3 dist_stop = (stop - r.origin()) / r.direction();

	double root1 = std::max(
		std::max(std::min(dist_start.x(), dist_stop.x()), std::min(dist_start.y(), dist_stop.y()))
		, std::min(dist_start.z(), dist_stop.z()));
	double root2 = std::min(
		std::min(std::max(dist_start.x(), dist_stop.x()), std::max(dist_start.y(), dist_stop.y()))
		, std::max(dist_start.z(), dist_stop.z()));

	if (root1 > root2) return false;

	auto root = root1 < 0 ? root2 : root1;
	if (root < t_min || t_max < root) return false;

	rec.t = root;
	rec.p = r.at(rec.t);
	vec3 outward_normal;
	if (rec.p.x() <= start.x() + EPS) outward_normal[0] = -1;
	else if (rec.p.x() >= stop.x() - EPS) outward_normal[0] = 1;
	else if (rec.p.y() <= start.y() + EPS) outward_normal[1] = -1;
	else if (rec.p.y() >= stop.y() - EPS) outward_normal[1] = 1;
	else if (rec.p.z() <= start.z() + EPS) outward_normal[2] = -1;
	else if (rec.p.z() >= stop.z() - EPS) outward_normal[2] = 1;

	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;

	return true;

}
#endif
