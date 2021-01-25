#version 430
layout(lines) in;
//TODO 
//prima data generati o curba (cu line strip), apoi o suprafata de rotatie/translatie cu triangle_strip
layout(triangle_strip, max_vertices = 256) out;

uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p1, control_p2, control_p3, control_p4;
uniform int no_of_instances;

uniform int no_of_generated_points;
uniform float max_translate;
uniform float max_rotate;

in int instance[2];


vec3 rotateY(vec3 point, float u)
{
	float x = point.x * cos(u) - point.z *sin(u);
	float z = point.x * sin(u) + point.z *cos(u);
	return vec3(x, point.y, z);
}

vec3 translateX(vec3 point, float t)
{
	return vec3(point.x + t, point.y, point.z);
}

vec3 bezier(float t)
{
	return control_p1 * pow((1 - t), 3) + control_p2 * 3 * t * pow((1 - t), 2) + control_p3 * 3 * pow(t, 2) * (1 - t) + control_p4 * pow(t, 3);
}

void main()
{
	const float translate_unit = 0.2f;
	if (instance[0] < no_of_instances)
	{
		float translate_step;
		float curve_step = (1.f / no_of_generated_points);
		translate_step = float(instance[0]) * translate_unit;

			//TODO 
			//in loc sa emiteti varfuri reprezentand punctele de control, emiteti varfuri care sa aproximeze curba Bezier
			for (float i = 0.f; i <= 1.f; i += curve_step) {

				gl_Position = Projection * View * vec4(translateX(bezier(i), translate_step), 1);	EmitVertex();
				gl_Position = Projection * View * vec4(translateX(bezier(i), translate_step + translate_unit), 1);	EmitVertex();

				if (i + curve_step > 1.f) {

					gl_Position = Projection * View * vec4(translateX(bezier(1.f), translate_step), 1);	EmitVertex();
					gl_Position = Projection * View * vec4(translateX(bezier(1.f), translate_step + translate_unit), 1);	EmitVertex();

					break;
				}
			}
		EndPrimitive();
	}
}
