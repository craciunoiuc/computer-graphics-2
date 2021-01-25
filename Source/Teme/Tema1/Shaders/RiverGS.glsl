#version 430
layout(lines) in;
in vec3 world_position[];
in vec3 world_normal[];
layout(triangle_strip, max_vertices = 64) out;

out vec2 texcoord;
out vec3 world_position_out;
out vec3 world_normal_out;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 mainRiver_p1, mainRiver_p2, mainRiver_p3, mainRiver_p4;
uniform vec3 firstSplice_p2, firstSplice_p3, firstSplice_p4;
uniform vec3 secondSplice_p2, secondSplice_p3, secondSplice_p4;
uniform vec3 thirdSplice_p2, thirdSplice_p3, thirdSplice_p4;
uniform int chooseRiver;

vec3 bezier(float t, vec3 control_p1, vec3 control_p2, vec3 control_p3, vec3 control_p4)
{
	return control_p1 * pow((1 - t), 3) + control_p2 * 3 * t * pow((1 - t), 2) + control_p3 * 3 * pow(t, 2) * (1 - t) + control_p4 * pow(t, 3);
}

void call_gl_Position(float i, vec3 control_p1, vec3 control_p2, vec3 control_p3, vec3 control_p4)
{
	vec3 point = bezier(i, control_p1, control_p2, control_p3, control_p4);
	texcoord = vec2(vec3(point + vec3(-2.5, 0, 0)).x / 10, vec3(point + vec3(-2.5, 0, 0)).z / 10);
	world_position_out = point + vec3(-2.5, 0, 0);
	world_normal_out = vec3(0, 1, 0);
	gl_Position = Projection * View * vec4(point + vec3(-2.5, 0, 0), 1); EmitVertex();
	texcoord = vec2(vec3(point + vec3(2.5, 0, 0)).x / 10, vec3(point + vec3(2.5, 0, 0)).z / 10);
	world_position_out = point + vec3(2.5, 0, 0);
	world_normal_out = vec3(0, 1, 0);
	gl_Position = Projection * View * vec4(point + vec3(2.5, 0, 0), 1); EmitVertex();
}

void main()
{
	float curve_step = (1.f / 20.f);
	vec3 control_p1, control_p2, control_p3, control_p4;

	if (chooseRiver == 1)
	{
		control_p1 = mainRiver_p1;
		control_p2 = mainRiver_p2;
		control_p3 = mainRiver_p3;
		control_p4 = mainRiver_p4;
	}
	if (chooseRiver == 2)
	{
		control_p1 = mainRiver_p4;
		control_p2 = firstSplice_p2;
		control_p3 = firstSplice_p3;
		control_p4 = firstSplice_p4;
	}
	if (chooseRiver == 3)
	{
		control_p1 = mainRiver_p4;
		control_p2 = secondSplice_p2;
		control_p3 = secondSplice_p3;
		control_p4 = secondSplice_p4;
	}
	if (chooseRiver == 4)
	{
		control_p1 = mainRiver_p4;
		control_p2 = thirdSplice_p2;
		control_p3 = thirdSplice_p3;
		control_p4 = thirdSplice_p4;
	}

	for (float i = 0.f; i <= 1.f; i += curve_step)
	{
		call_gl_Position(i, control_p1, control_p2, control_p3, control_p4);

		if (i + curve_step > 1.f)
		{
			call_gl_Position(1.f, control_p1, control_p2, control_p3, control_p4);
			break;
		}
	}
	EndPrimitive();
}
