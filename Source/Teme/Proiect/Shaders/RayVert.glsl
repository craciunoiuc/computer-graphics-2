#version 430
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float z_near;
uniform float z_far;

layout(location = 0) out vec2 text_coord;
layout(location = 1) out vec3 ray_direction_from_vertex;
layout(location = 2) out vec3 eye_pos;

mat4 proj_view = inverse(Projection * View);
const vec2 positions[6] = vec2[](
        vec2(-1, -1),
        vec2(+1, -1),
        vec2(-1, +1),
        vec2(-1, +1),
        vec2(+1, -1),
        vec2(+1, +1)
);

void main()
{

    vec2 pos = positions[gl_VertexID];
    eye_pos = (proj_view * vec4(pos, -1.0, 1.0) * z_near).xyz;
    ray_direction_from_vertex = (proj_view * (vec4(pos, 1.0, 1.0) * z_far - vec4(pos, -1.0, 1.0) * z_near)).xyz - eye_pos;
    gl_Position = vec4(pos, 0.0, 1.0);
}