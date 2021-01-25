#version 430
in vec3 world_position_out;
in vec3 world_normal_out;
in vec2 texcoord;

layout(location = 0) out vec4 out_color;


uniform vec3 camera_position;
uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;

vec3 myReflect()
{
	vec3 incRay = world_position_out - camera_position;
	vec3 reflectRay = reflect(incRay, world_normal_out);
	return texture(texture_cubemap, reflectRay).xyz;
}


void main()
{
	vec4 color1 = texture(texture_1, texcoord);
    vec4 color2 = vec4(myReflect(), 1);
	out_color = mix(color2, color1, 0.7f);
}