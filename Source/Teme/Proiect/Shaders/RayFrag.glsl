#version 430

layout(location = 0) in vec2 text_coord;
layout(location = 1) in vec3 ray_direction_from_vertex;
layout(location = 2) in vec3 eye_pos;

const float REFR_IDX = 1.33f;
const float DIST_MAX = 600.f;
const float EPS = 0.006f;

uniform int cube_start = -1;
uniform int cube_stop = -1;
uniform int sphere_start = -1;
uniform int sphere_stop = -1;
uniform int cylinder_start = -1;
uniform int cylinder_stop = -1;
uniform int cone_start = -1;
uniform int cone_stop = -1;
uniform int light_start = -1;
uniform int light_stop = -1;

layout(location = 0) out vec4 out_color;

// Shadows toggle
#define ENABLE_SHADOWS 1

//Object material properties as bit flags
#define DIFFUSE     1
#define SPECULAR    2
#define REFLECTIVE  4
#define REFRACTIVE  8

//Objects ID's
#define OBJ_SPHERE_ID   0
#define OBJ_BOX_ID      1
#define OBJ_CONE_ID     2
#define OBJ_CYLINDER_ID 3

// How the light interacts with the objects' color
struct ObjectMatProperties
{
	vec3 diffraction; // Can be used for color
	vec3 specular;
	vec3 reflection;
};

// Rectangle face definition
struct Rectangle
{
	vec3 position;
	float width;
	float len;
};

// Triangle face definition
struct Triangle
{
	vec3 v1;
	vec3 v2;
	vec3 v3;
};

// Simple Sphere definition
struct Sphere
{
	vec3 position;
	float radius;
	int properties;
	ObjectMatProperties lightprops;
};

// Simple Cube definition
struct Box
{
	vec3 start;
	vec3 stop;
	int properties;
	ObjectMatProperties lightprops;
};

// Simple Cone definition
struct Cone
{
	vec3 position;
	float radius;
	float height;
	int properties;
	ObjectMatProperties lightprops;
};

// Simple Cylinder definition
struct Cylinder
{
	vec3 position;
	float radius;
	float height;
	int properties;
	ObjectMatProperties lightprops;
};


// Ray definition
struct Ray
{
	vec3 origin;
	vec3 direction;
};

// Light definition
struct Light
{
	vec3 position;
	vec3 color;
};

//SCENES

Sphere spheres[] = Sphere[](
	// Scene 1
	Sphere(vec3(0, 25, -10), 10, REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0), vec3(0.95), vec3(0.2, 0.9, 0.95))),
	Sphere(vec3(15, 25, 56), 2, REFRACTIVE | SPECULAR, ObjectMatProperties(vec3(0), vec3(0), vec3(1))),

	// Scene 2
	Sphere(vec3(0, 25, -10), 10, REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0.0), vec3(0.95), vec3(0.2, 0.9, 0.95))),
	Sphere(vec3(-40, 74, -25), 18, REFRACTIVE, ObjectMatProperties(vec3(0), vec3(0), vec3(1))),
	Sphere(vec3(0, 55, 110), 10, DIFFUSE | SPECULAR, ObjectMatProperties(vec3(0.6, 0.1, 0.6), vec3(0.95), vec3(0.2, 0.9, 0.95))),	
	Sphere(vec3(85, 55, 76), 7, REFLECTIVE, ObjectMatProperties(vec3(0.6), vec3(0.6), vec3(1))),
	Sphere(vec3(-40, 75, -30), 10, DIFFUSE, ObjectMatProperties(vec3(0.7, 0.7, 0.8), vec3(0.95), vec3(0.2, 0.9, 0.95))),
	Sphere(vec3(50, 26, 20), 7, REFRACTIVE | SPECULAR, ObjectMatProperties(vec3(0.0), vec3(0), vec3(1))),
	Sphere(vec3(60, 85, -10), 10, REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0.0), vec3(0.95), vec3(0.8, 0.3, 0.95))),
	Sphere(vec3(15, 105, 26), 7, DIFFUSE | SPECULAR, ObjectMatProperties(vec3(0.6, 0.2, 0.2), vec3(0.6), vec3(1))),
	Sphere(vec3(-40, 45, 96), 20, REFLECTIVE, ObjectMatProperties(vec3(0.7, 0.9, 0.7), vec3(0.7), vec3(0.9))),
	Sphere(vec3(5, 30, 56), 2, REFRACTIVE, ObjectMatProperties(vec3(0), vec3(0), vec3(1))),
	Sphere(vec3(15, 30, 56), 2, REFRACTIVE, ObjectMatProperties(vec3(0), vec3(0), vec3(1))),
	Sphere(vec3(10, 20, 56), 2, REFRACTIVE, ObjectMatProperties(vec3(0), vec3(0), vec3(1))),
	
	// Scene 3
	Sphere(vec3(0, 25, -10), 10, REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0.0), vec3(0.95), vec3(0.2, 0.9, 0.95))),
	Sphere(vec3(-40, 74, -25), 18, REFRACTIVE, ObjectMatProperties(vec3(0), vec3(0), vec3(1))),
	Sphere(vec3(0, 55, 110), 10, DIFFUSE | SPECULAR, ObjectMatProperties(vec3(0.6, 0.1, 0.6), vec3(0.95), vec3(0.2, 0.9, 0.95))),	
	Sphere(vec3(85, 55, 76), 7, REFLECTIVE, ObjectMatProperties(vec3(0.6), vec3(0.6), vec3(1))),
	Sphere(vec3(-40, 75, -30), 10, DIFFUSE, ObjectMatProperties(vec3(0.7, 0.7, 0.8), vec3(0.95), vec3(0.2, 0.9, 0.95))),
	Sphere(vec3(50, 26, 20), 7, REFRACTIVE | SPECULAR, ObjectMatProperties(vec3(0.0), vec3(0), vec3(1))),
	Sphere(vec3(60, 85, -10), 10, REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0.0), vec3(0.95), vec3(0.8, 0.3, 0.95))),
	Sphere(vec3(15, 105, 26), 7, DIFFUSE | SPECULAR, ObjectMatProperties(vec3(0.6, 0.2, 0.2), vec3(0.6), vec3(1))),
	Sphere(vec3(-40, 45, 96), 20, REFLECTIVE, ObjectMatProperties(vec3(0.7, 0.9, 0.7), vec3(0.7), vec3(0.9))),
	Sphere(vec3(15, 55, 15), 2, REFRACTIVE, ObjectMatProperties(vec3(0.0), vec3(0), vec3(1)))
);

Box boxes[] = Box[](
	// Scene 1
	Box(vec3(23, 9, 13), vec3(37, 23, 27), REFRACTIVE, ObjectMatProperties(vec3(0), vec3(0.95), vec3(0.9, 0.95, 0.94))),
	Box(vec3(-70, 0, -70), vec3(120, 110, 120),  DIFFUSE, ObjectMatProperties(vec3(0.8), vec3(0.6), vec3(0.8))),

	// Scene 2
	Box(vec3(43, 12, 13), vec3(57, 27, 27), REFLECTIVE, ObjectMatProperties(vec3(0), vec3(0.55), vec3(0.7, 0.75, 0.34))),
	Box(vec3(-70, 0, -70), vec3(120, 110, 120),  DIFFUSE, ObjectMatProperties(vec3(0.8), vec3(0.6), vec3(0.8))),
	Box(vec3(-3, 9, 53), vec3(-12, 33, 63), REFRACTIVE, ObjectMatProperties(vec3(0), vec3(0.95), vec3(0.4, 0.55, 0.54))),

	// Scene 3
	Box(vec3(43, 12, 13), vec3(57, 27, 27), REFLECTIVE, ObjectMatProperties(vec3(0), vec3(0.55), vec3(0.7, 0.75, 0.34))),
	Box(vec3(-70, 0, -70), vec3(120, 110, 120),  REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0.5), vec3(0.2), vec3(0.85))),
	Box(vec3(-3, 9, 53), vec3(-12, 33, 63), REFRACTIVE, ObjectMatProperties(vec3(0), vec3(0.95), vec3(0.4, 0.55, 0.54)))
);

Cylinder cylinders[] = Cylinder[](
	// Scene 1
	Cylinder(vec3(10, 10, 15), 6, 7, DIFFUSE, ObjectMatProperties(vec3(0.2, 0.8, 0.2), vec3(0), vec3(0.6))),

	// Scene 2
	Cylinder(vec3(50, 30, -25), 2, 12, REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0.5, 0.5, 0.2), vec3(1), vec3(0.6))),
	Cylinder(vec3(-35, 7, -35), 6, 7, DIFFUSE, ObjectMatProperties(vec3(0.8, 0.2, 0.2), vec3(0), vec3(0.6))),
	Cylinder(vec3(10, 10, 75), 6, 7, DIFFUSE, ObjectMatProperties(vec3(0.2, 0.8, 0.2), vec3(0), vec3(0.6))),

	// Scene 3
	Cylinder(vec3(50, 30, -25), 2, 12, REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0.5, 0.5, 0.2), vec3(1), vec3(0.6))),
	Cylinder(vec3(-35, 7, -35), 6, 7, DIFFUSE, ObjectMatProperties(vec3(0.8, 0.2, 0.2), vec3(0), vec3(0.6))),
	Cylinder(vec3(10, 10, 75), 6, 7, DIFFUSE, ObjectMatProperties(vec3(0.2, 0.8, 0.2), vec3(0), vec3(0.6)))
);

Cone cones[] = Cone[](
	// Scene 1
	Cone(vec3(100, 20, 15), 16, 16, DIFFUSE | SPECULAR, ObjectMatProperties(vec3(0.2, 0.2, 0.9), vec3(0), vec3(0.6))),

	// Scene 2
	Cone(vec3(108, 20, 15), 16, 16, DIFFUSE, ObjectMatProperties(vec3(0.2, 0.7, 0.3), vec3(0), vec3(0.6))),
	Cone(vec3(-52, 20, 15), 16, 16, REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0.9, 0.2, 0.9), vec3(1), vec3(0.6))),

	// Scene 3
	Cone(vec3(108, 20, 15), 16, 16, DIFFUSE, ObjectMatProperties(vec3(0.2, 0.7, 0.3), vec3(0), vec3(0.6))),
	Cone(vec3(-52, 20, 15), 16, 16, REFLECTIVE | SPECULAR, ObjectMatProperties(vec3(0.9, 0.2, 0.9), vec3(1), vec3(0.6)))

);

Light lights[] = Light[](
	// Scene 1
	Light(vec3(15, 35, 56), vec3(0.9)),

	// Scene 2
	Light(vec3(5, 30, 56), vec3(1, 0, 0)),
	Light(vec3(15, 30, 56), vec3(0, 1, 0)),
	Light(vec3(10, 20, 56), vec3(0, 0, 1)),

	// Scene 3
	Light(vec3(15, 55, 15), vec3(0.9))
);

// Calculates the closest intersection between a ray and a cone. Returns -1 if no intersection is detected.
// The cone is generated by the square function on all axis and the bottom is added afterwards.
float cone_intersection(const Ray ray, const Cone cone)
{
	float root = (-cone.height - (ray.origin.z - cone.position.z)) / ray.direction.z;

	vec3 point = (ray.origin + root * ray.direction);
	float rad = (point.x - cone.position.x) * (point.x - cone.position.x) + (point.y - cone.position.y) * (point.y - cone.position.y);

	if (rad < cone.radius * cone.radius && dot(ray.direction, vec3(0, 0, point.z  - cone.position.z)) < 0.0f) {
		return root;
	}

	vec3 dir_sq = ray.direction * ray.direction;
	vec3 orig_sq = (ray.origin - cone.position) * (ray.origin - cone.position);
	vec3 or_dir = (ray.origin - cone.position) * ray.direction;

	float a = dir_sq.x + dir_sq.y - dir_sq.z;
	float b = 2.f * (or_dir.x + or_dir.y - or_dir.z);
	float c = orig_sq.x + orig_sq.y - orig_sq.z;
	float delta = b * b - 4.f * a * c;
	
	//No intersection
	if(delta < 0.f) return -1.f;
		
	float root1 = (-b + sqrt(delta)) / (2.f * a);
	float root2 = (-b - sqrt(delta)) / (2.f * a);

	if (root1 > root2) {
		float aux = root1;
		root1 = root2;
		root2 = aux;
	}
	if (root2 < 0) return -1.f;

	root = root1 < 0.f ? root2 : root1;

	point = (ray.origin + root * ray.direction);
	if (point.z < -cone.height + cone.position.z || point.z > cone.position.z) return -1.f;
	return root;

}

// Calculates the closest intersection between a ray and a cylinder. Returns -1 if no intersection is detected.
// The cylinder is generated similar to the sphere and to tops are added afterwards.
float cylinder_intersection(const Ray ray, const Cylinder cylinder)
{
	float root1 = (-cylinder.height - (ray.origin.z - cylinder.position.z)) / ray.direction.z;
	float root2 = (cylinder.height - (ray.origin.z - cylinder.position.z)) / ray.direction.z;

	if (root1 > root2) {
		float aux = root1;
		root1 = root2;
		root2 = aux;
	}

	float root = root1 < 0.f ? root2 : root1;
	vec3 point = (ray.origin + root * ray.direction);
	float rad = (point.x - cylinder.position.x) * (point.x - cylinder.position.x) + (point.y - cylinder.position.y) * (point.y - cylinder.position.y);

	if (rad < cylinder.radius * cylinder.radius && dot(ray.direction, vec3(0, 0, point.z  - cylinder.position.z)) < 0.0f) {
		return root;
	}

	float a = dot(ray.direction.xy, ray.direction.xy);
	float b = 2.f * dot(ray.origin.xy - cylinder.position.xy, ray.direction.xy);
	float c = -(cylinder.radius * cylinder.radius) + dot(ray.origin.xy - cylinder.position.xy, ray.origin.xy - cylinder.position.xy);

	float delta = b * b - 4.f * a * c;
	
	//No intersection
	if(delta < 0.f) return -1.f;
		
	root1 = (-b + sqrt(delta)) / (2.f * a);
	root2 = (-b - sqrt(delta)) / (2.f * a);

	if (root1 > root2) {
		float aux = root1;
		root1 = root2;
		root2 = aux;
	}
	if (root2 < 0) return -1.f;

	root = root1 < 0.f ? root2 : root1;

	point = (ray.origin + root * ray.direction);
	if (point.z < -cylinder.height + cylinder.position.z) return -1.f;
	if (point.z > cylinder.height + cylinder.position.z) return -1.f;
	return root;
}

// Calculates the closest intersection between a box and a ray. Returns -1 if no intersection is detected.
float box_intersection(const Ray ray, const Box box)
{
	vec3 dist_start = (box.start - ray.origin) / ray.direction;
	vec3 dist_stop = (box.stop - ray.origin) / ray.direction;

	float root1 = max(max(min(dist_start.x, dist_stop.x), min(dist_start.y, dist_stop.y)), min(dist_start.z, dist_stop.z));
	float root2 = min(min(max(dist_start.x, dist_stop.x), max(dist_start.y, dist_stop.y)), max(dist_start.z, dist_stop.z));
	
	if(root1 > root2) return -1.f;
	
	return root1 < 0 ? root2 : root1;
}

// Calculates the closest intersection between a ray and a sphere. Returns -1 if no intersection is detected.
float sphere_intersection(const Ray ray, const Sphere sphere)
{
	float a = dot(ray.direction, ray.direction);
	float b = 2.f * dot(ray.origin - sphere.position, ray.direction);
	float c = -(sphere.radius * sphere.radius) + dot(ray.origin - sphere.position, ray.origin - sphere.position);

	float delta = b * b - 4.f * a * c;
	
	//No intersection
	if(delta < 0) return -1.f;
	
	float root1 = (-b + sqrt(delta)) / (2.0f * a);
	float root2 = (-b - sqrt(delta)) / (2.0f * a);

	if(root1 > EPS && root2 > EPS) return min(root1, root2);

	return max(root1, root2);
}

// The function that implements the ray tracing algorithm
bool trace_ray(inout Ray start_ray, out vec3 color, inout vec3 color_intensity)
{
	float refr_current = 1.0f / REFR_IDX;
	float closest_dist = DIST_MAX-2;
	float current_dist;
	int closest_object_index = -1;
	int closest_object_ID;

	// All spheres
	for(int i = sphere_start; i < sphere_stop; ++i) {
		current_dist = sphere_intersection(start_ray, spheres[i]);
		if(current_dist > 0.0f && current_dist < closest_dist) {
			closest_dist = current_dist;
			closest_object_index = i;
			closest_object_ID = OBJ_SPHERE_ID;
		}
	}
	
	// All boxes
	for(int i = cube_start; i < cube_stop; ++i) {
		current_dist = box_intersection(start_ray, boxes[i]);
		if(current_dist > 0.0f && current_dist < closest_dist) {
			closest_dist = current_dist;
			closest_object_index = i;
			closest_object_ID = OBJ_BOX_ID;
		}
	}
	
	// All cylinders
	for (int i = cylinder_start; i < cylinder_stop; ++i) {
		current_dist = cylinder_intersection(start_ray, cylinders[i]);
		if(current_dist > 0.0f && current_dist < closest_dist) {
			closest_dist = current_dist;
			closest_object_index = i;
			closest_object_ID = OBJ_CYLINDER_ID;
		}
	}

	// All cones
	for (int i = cone_start; i < cone_stop; ++i) {
		current_dist = cone_intersection(start_ray, cones[i]);
		if(current_dist > 0.0f && current_dist < closest_dist) {
			closest_dist = current_dist;
			closest_object_index = i;
			closest_object_ID = OBJ_CONE_ID;
		}
	}

	//if the Ray hit any object
	if(closest_object_index != -1) {
		vec3 point = start_ray.direction * closest_dist + start_ray.origin; // exact point
		vec3 normal = vec3(0, 0, 0);
		vec3 diffraction;
		vec3 specular;
		vec3 reflection;
		int properties;

		// Gets the values from the structures
		switch(closest_object_ID) {
			case OBJ_SPHERE_ID:
				properties = spheres[closest_object_index].properties;
				diffraction = spheres[closest_object_index].lightprops.diffraction;
				specular = spheres[closest_object_index].lightprops.specular;
				reflection = spheres[closest_object_index].lightprops.reflection;
				normal = normalize(point - spheres[closest_object_index].position);
				break;

			case OBJ_BOX_ID:
				properties = boxes[closest_object_index].properties;
				diffraction = boxes[closest_object_index].lightprops.diffraction;
				specular = boxes[closest_object_index].lightprops.specular;
				reflection = boxes[closest_object_index].lightprops.reflection;
				if(point.x <= boxes[closest_object_index].start.x + EPS) normal.x = -1;
				else if(point.x >= boxes[closest_object_index].stop.x - EPS) normal.x = 1;
				else if(point.y <= boxes[closest_object_index].start.y + EPS) normal.y = -1;
				else if(point.y >= boxes[closest_object_index].stop.y - EPS) normal.y = 1;
				else if(point.z <= boxes[closest_object_index].start.z + EPS) normal.z = -1;
				else if(point.z >= boxes[closest_object_index].stop.z - EPS) normal.z = 1;
				break;

			case OBJ_CONE_ID:
				properties = cones[closest_object_index].properties;
				diffraction = cones[closest_object_index].lightprops.diffraction;
				specular = cones[closest_object_index].lightprops.specular;
				reflection = cones[closest_object_index].lightprops.reflection;
				float root = (-cones[closest_object_index].height - (start_ray.origin.z - cones[closest_object_index].position.z)) / start_ray.direction.z;
				vec3 diff = normalize(point - cones[closest_object_index].position);
				if (abs(closest_dist - root) <= EPS) {
					normal = vec3(0, 0, -diff.z);
				}
				else {
					normal = vec3(-diff.x, -diff.y, diff.z);
				}
				break;

			case OBJ_CYLINDER_ID:
				properties = cylinders[closest_object_index].properties;
				diffraction = cylinders[closest_object_index].lightprops.diffraction;
				specular = cylinders[closest_object_index].lightprops.specular;
				reflection = cylinders[closest_object_index].lightprops.reflection;
				float root1 = (-cylinders[closest_object_index].height - (start_ray.origin.z - cylinders[closest_object_index].position.z)) / start_ray.direction.z;
				float root2 = (cylinders[closest_object_index].height - (start_ray.origin.z - cylinders[closest_object_index].position.z)) / start_ray.direction.z;

				if (abs(closest_dist - root1) <= EPS) {
					normal = vec3(0, 0, -1);
				}
				else if (abs(closest_dist - root2) <= EPS) {
					normal = vec3(0, 0, 1);
				}
				else {
					vec3 diff = normalize(point - cylinders[closest_object_index].position);
					float theta = atan(diff.y / diff.x);
					normal = vec3(cos(theta), sin(theta), 0);
				}
				break;

			default: break;
		}
		
		// If the object is transparent
		if(dot(start_ray.direction, normal) > 0.f) {
			normal *= -1.f;
			refr_current = REFR_IDX;
		}
		
		// Add ambient lightning
		color += color_intensity * diffraction * 0.2f;
		
		// Calculate the diffuse and specular lightning coming to the pixel
		for(int i = light_start; i < light_stop; ++i) {
			vec3 to_light = normalize(lights[i].position - point);
			bool in_shadow = false;

#if ENABLE_SHADOWS
			Ray ray_to_light = Ray(point, to_light);
			float dist_point_to_light = distance(lights[i].position, point);
			for(int j = sphere_start; !in_shadow && j < sphere_stop; ++j) {
				
				// Skip checking the shadow with itself
				if(!(closest_object_index == j && closest_object_ID == OBJ_SPHERE_ID) && ((spheres[j].properties & REFRACTIVE) == 0)) {
					float hit_dist_point = sphere_intersection(ray_to_light, spheres[j]);
					in_shadow = EPS < hit_dist_point && dist_point_to_light > hit_dist_point; // Margin of error
				}
			}
			for(int j = cube_start; !in_shadow && j < cube_stop; ++j) {
				if(!(j == closest_object_index && OBJ_BOX_ID == closest_object_ID) && (0 == (boxes[j].properties & REFRACTIVE))) {
					float hit_dist_point = box_intersection(ray_to_light, boxes[j]);
					in_shadow = EPS < hit_dist_point && dist_point_to_light > hit_dist_point;
				}
			}
			for(int j = cone_start; !in_shadow && j < cone_stop; ++j) {
				if(!(j == closest_object_index && OBJ_CONE_ID == closest_object_ID) && (0 == (cones[j].properties & REFRACTIVE))) {
					float hit_dist_point = cone_intersection(ray_to_light, cones[j]);
					in_shadow = EPS < hit_dist_point && dist_point_to_light > hit_dist_point;
				}
			}
			for(int j = cylinder_start; !in_shadow && j < cylinder_stop; ++j) {
				if(!(j == closest_object_index && OBJ_CYLINDER_ID == closest_object_ID) && (0 == (cylinders[j].properties & REFRACTIVE))) {
					float hit_dist_point = cylinder_intersection(ray_to_light, cylinders[j]);
					in_shadow = EPS < hit_dist_point && dist_point_to_light > hit_dist_point;
				}
			}
#endif //ENABLE_SHADOWS

			// Calculate direct illumination based on the properties
			if(!in_shadow) {
				if(bool(properties & DIFFUSE)) {
					color += color_intensity * lights[i].color * diffraction * max(0.f, dot(to_light, normal));
				}
				if(bool(properties & SPECULAR)) {
					vec3 halfway = normalize((-1 * start_ray.direction) + to_light);
					color += color_intensity * lights[i].color * specular * pow(max(0.f, dot(halfway, normal)), 150);
				}
			}
		}
		
		// The resulting ray is either reflected or refracted
		if(bool(properties & REFLECTIVE)) {
			//the color that comes from the reflected direction, should be modulated by this object's color
			color_intensity *= reflection;
			
			// The new ray starts where the old one ended (plus a small error so it's not equal)
			start_ray = Ray(point + (EPS * reflect(start_ray.direction, normal)), reflect(start_ray.direction, normal));

			return true;
		}
		if(bool(properties & REFRACTIVE)) {
			color_intensity *= reflection;
			
			Ray newRay = Ray(point + (EPS * refract(start_ray.direction, normal, refr_current)), refract(start_ray.direction, normal, refr_current));
			
			start_ray = (newRay.direction == vec3(0)) ? Ray(point + (EPS * reflect(start_ray.direction, normal)), reflect(start_ray.direction, normal)) : newRay;
			
			return true;
		}
	}
	return false;
}

void main()
{
	const int max_bounces = 40; // Change based on performance
	Ray start_ray = Ray(eye_pos, normalize(ray_direction_from_vertex));
	vec3 color = vec3(0);
	vec3 color_intensity = vec3(1);
	bool ray_colision = true;

	for(int i = 0; ray_colision && i < max_bounces; ++i) {
		ray_colision = trace_ray(start_ray, color, color_intensity);
	}

	out_color = vec4(color, 1);
}
