#version 430
layout(location = 0) out vec2 texture_coord;

// Generates the positions and texcoords of the quad in front of the screen
void main() {
    const vec2 positions[6] = vec2[](
        vec2(-1, -1),
        vec2(+1, -1),
        vec2(-1, +1),
        vec2(+1, -1),
        vec2(+1, +1),
        vec2(-1, +1)
    );
    const vec2 texcoords[6] = vec2[](
        vec2(0, 0),
        vec2(1, 0),
        vec2(0, 1),
        vec2(1, 0),
        vec2(1, 1),
        vec2(0, 1)
    );

    texture_coord = texcoords[gl_VertexID];
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
}
