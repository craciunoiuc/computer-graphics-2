#version 410

layout(location = 0) in vec2 texture_coord;

uniform sampler2D textureImage;

layout(location = 0) out vec4 out_color;

void main()
{
	vec2 textureCoord = vec2(texture_coord.x, 1 - texture_coord.y);
	out_color = texture(textureImage, textureCoord);
}