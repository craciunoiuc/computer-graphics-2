#version 430
layout(location = 0) in vec2 texture_coord;

uniform sampler2D texture_1;
uniform sampler2D texture_depth;

layout(location = 0) out vec4 out_color;

uniform float zNear = 0.1; // No need to modify
uniform float zFar = 500.0; // No need to modify
uniform float focalDistance = 0.15; // Input
uniform float focalInterval = 0.55; // Input
const float focalTransition = 0.1;

// Liniarises the depth
float linearDepth(float depthSample)
{
    depthSample = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
    return zLinear;
}

// Calculates the texture based on the depth
vec3 depth()
{
    float t2 = pow(texture(texture_depth, texture_coord).x, 256);
	return vec3(t2, t2, t2);
}

// Blurs the image
vec4 gaussianFive()
{
	vec2 texelSize = 1.0f / ivec2(1280, 720);
	vec4 sum = vec4(0);
	float to_divide = 273;
	const float kernel[25] = float[](1, 4, 7, 4, 1, 4, 16, 26, 16, 4, 7, 26, 41, 26, 7, 4, 16, 26, 16, 4, 1, 4, 7, 4, 1);

	for(int i = -2; i <= 2; i++)
	{
		for(int j = -2; j <= 2; j++)
		{
			sum += texture(texture_1, texture_coord + vec2(i, j) * texelSize) * kernel[5 * (i + 2) + (j + 2)];
		}
	}
	return sum / to_divide;
}

// Calculates the intervals to apply bluring
void main()
{
	vec4 color = texture(texture_1, texture_coord);
    float zLin = linearDepth(depth().r);
    if (zLin < focalDistance || zLin > focalDistance + focalInterval)
	{
        out_color = gaussianFive();
    }
	else
	{
		if (zLin > focalDistance && zLin < focalDistance + focalTransition)
		{
			out_color = mix(color, gaussianFive(),  normalize(zLin - focalDistance));
		}
		else
		{
			if (zLin > focalDistance + focalInterval && zLin < focalDistance + focalInterval + focalTransition)
			{
				out_color = mix(gaussianFive(), color, normalize(zLin - focalDistance - focalInterval));
			}
			else
			{
				out_color = color;
			}
		}
	}
}
