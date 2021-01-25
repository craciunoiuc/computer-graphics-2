#version 430
layout(lines) in;
layout(location = 0) in vec2 v_texture_coord[];

layout(triangle_strip, max_vertices = 64) out;
layout(location = 0) out vec2 texture_coord;

uniform mat4 View;
uniform mat4 Projection;

uniform vec3 mainRiver_p1, mainRiver_p2, mainRiver_p3, mainRiver_p4;
uniform vec3 firstSplice_p2, firstSplice_p3, firstSplice_p4;
uniform vec3 secondSplice_p2, secondSplice_p3, secondSplice_p4;
uniform vec3 thirdSplice_p2, thirdSplice_p3, thirdSplice_p4;
uniform int chooseRiver;
uniform int chooseSide;
uniform int middlePiece;
uniform float start;

int iter = -1;
int firstPoint = 1;

// Generates the points on the curve
vec3 bezier(float t, vec3 control_p1, vec3 control_p2, vec3 control_p3, vec3 control_p4)
{
	return control_p1 * pow((1 - t), 3) + control_p2 * 3 * t * pow((1 - t), 2) + control_p3 * 3 * pow(t, 2) * (1 - t) + control_p4 * pow(t, 3);
}

// Generates the corresponding curves for each river
void call_gl_Position(float i, vec3 control_p1, vec3 control_p2, vec3 control_p3, vec3 control_p4)
{
	vec3 point = bezier(i, control_p1, control_p2, control_p3, control_p4);
	vec3 otherPoint;
	iter++;
	if (middlePiece == 1)
	{
		if (chooseRiver == 2)
		{
			if (chooseSide == 1)
			{
				otherPoint = bezier(i, mainRiver_p4, secondSplice_p2, secondSplice_p3, secondSplice_p4);
			}
		}
		if (chooseRiver == 3)
		{
			if (chooseSide == 0)
			{
				otherPoint = bezier(i, mainRiver_p4, firstSplice_p2, firstSplice_p3, firstSplice_p4);
			}
			else
			{
				otherPoint = bezier(i, mainRiver_p4, thirdSplice_p2, thirdSplice_p3, thirdSplice_p4);
			}
		}
		if (chooseRiver == 4)
		{
			if (chooseSide == 0)
			{
				otherPoint = bezier(i, mainRiver_p4, secondSplice_p2, secondSplice_p3, secondSplice_p4);
			}
		}
		otherPoint = vec3((otherPoint.x + point.x) / 2.f, i * 3 + 2, (otherPoint.z + point.z) / 2.f);
		otherPoint = (firstPoint == 0) ? otherPoint : vec3(otherPoint.x, 0, otherPoint.z);
		if (firstPoint == 1) firstPoint = 0;
		otherPoint += (chooseSide == 0) ? vec3(2, 0, 0) : vec3(-2, 0, 0);
	}
	else
	{
		otherPoint = point;
		otherPoint += (chooseSide == 0) ? vec3(-3, 3, 0) : vec3(3, 3, 0);
	}

	if (chooseSide == 0)
	{
		texture_coord = (iter % 2 == 1) ? vec2(0, 1) : vec2(0, 0);
		gl_Position = Projection * View * vec4(point + vec3(-2, 0, 0), 1); EmitVertex();
		texture_coord = (iter % 2 == 1) ? vec2(1, 1) : vec2(1, 0);
		gl_Position = Projection * View * vec4(otherPoint + vec3(-2, 0, 0), 1); EmitVertex();
		return;
	}
	if (chooseSide == 1)
	{
		texture_coord = (iter % 2 == 1) ? vec2(0, 1) : vec2(0, 0);
		gl_Position = Projection * View * vec4(point + vec3(2, 0, 0), 1); EmitVertex();
		texture_coord = (iter % 2 == 1) ? vec2(1, 1) : vec2(1, 0);
		gl_Position = Projection * View * vec4(otherPoint + vec3(2, 0, 0), 1); EmitVertex();
		return;
	}
}

// Generates each river based on input
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

	for (float i = start; i <= 1.f; i += curve_step)
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
