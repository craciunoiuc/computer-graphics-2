#version 430

layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;

uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;

uniform vec3 camera_position;

layout(location = 0) out vec4 out_color;

vec3 myReflect()
{
    // TODO - compute the reflection color value
	vec3 razaIncidenta = world_position - camera_position;
	vec3 refl = reflect(razaIncidenta, world_normal);
	return texture(texture_cubemap, refl).xyz;
}

vec3 myRefract(float refractive_index)
{
    // TODO - compute the refraction color value
	vec3 razaIncidenta = world_position - camera_position;
	vec3 refr = refract(razaIncidenta, world_normal, refractive_index);
	return texture(texture_cubemap, refr).xyz;
}

void main()
{
    out_color = vec4(myReflect(), 0);
	out_color += vec4(myRefract(1.33), 0);
}
