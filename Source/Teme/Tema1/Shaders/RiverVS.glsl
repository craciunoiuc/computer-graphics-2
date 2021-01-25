#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;

out vec3 world_position;
out vec3 world_normal;

void main()
{
	world_position = (Model * vec4(v_position, 1)).xyz;
    world_normal = normalize(mat3(Model) * v_normal);
	gl_Position =  Model * vec4(v_position, 1);
}


