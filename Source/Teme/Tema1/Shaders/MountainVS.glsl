#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;

layout(location = 0) out vec2 texture_coord;

void main()
{
	gl_Position =  Model * vec4(v_position, 1);
}


