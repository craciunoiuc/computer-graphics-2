#version 430
layout(location = 0) out vec4 out_color;

uniform int part;

void main()
{
	switch (part)
	{
		case 0:
			out_color = vec4(0.3, 0.3, 0.3, 0); // Seat
			break;
		case 1:
			out_color = vec4(0.8, 0.3, 0, 0); // Head
			break;
		case 2:
			out_color = vec4(0.7, 0.7, 0, 0); // Body
			break;

		default: out_color = vec4(1, 1, 1, 0);
	}
}
