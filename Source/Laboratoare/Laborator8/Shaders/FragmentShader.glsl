#version 410

layout(location = 0) in vec2 texture_coord;

uniform sampler2D textureImage;
uniform ivec2 screenSize;
uniform int flipVertical;

// 0 - original
// 1 - grayscale
// 2 - blur
uniform int outputMode = 2;

// Flip texture horizontally when
vec2 textureCoord = vec2(texture_coord.x, (flipVertical != 0) ? 1 - texture_coord.y : texture_coord.y);

layout(location = 0) out vec4 out_color;

vec4 grayscale()
{
	vec4 color = texture(textureImage, textureCoord);
	float gray = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b; 
	return vec4(gray, gray, gray,  0);
}

float avg_gray(vec4 pixel)
{
	return (pixel.r + pixel.g + pixel.b) / 3;
}


vec4 median()
{
	const int radius = 2;
	vec4 to_sort[25];
	vec2 texelSize = 1.0f / screenSize;

	//Collect all pixels
	for(int i = -radius; i <= radius; i++)
	{
		for(int j = -radius; j <= radius; j++)
		{
			to_sort[(2 * radius + 1) * (i + 2) + (j + 2)] = texture(textureImage, textureCoord + vec2(i, j) * texelSize);
		}
	}
	
	//Sort them
	for (int i = 0; i < (2 * radius + 1) * (2 * radius + 1); ++i)
	{
		for (int j = 0; j <= (2 * radius + 1) * (2 * radius + 1); ++j)
		{
			if (avg_gray(to_sort[i]) > avg_gray(to_sort[j]))
			{
				vec4 aux = to_sort[i];
				to_sort[i] = to_sort[j];
				to_sort[j] = aux;
			}
		}
	}

	//Return the middle value
	return to_sort[(2 * radius + 1) * (2 * radius + 1) / 2];
	//return texture(textureImage, textureCoord);
}

vec4 blur(int blurRadius)
{
	vec2 texelSize = 1.0f / screenSize;
	vec4 sum = vec4(0);
	for(int i = -blurRadius; i <= blurRadius; i++)
	{
		for(int j = -blurRadius; j <= blurRadius; j++)
		{
			sum += texture(textureImage, textureCoord + vec2(i, j) * texelSize);
		}
	}
		
	float samples = pow((2 * blurRadius + 1), 2);
	return sum / samples;
}

vec4 average(int radius)
{
	vec2 texelSize = 1.0f / screenSize;
	vec4 sum = vec4(0);
	for(int i = -radius; i <= radius; i++)
	{
		for(int j = -radius; j <= radius; j++)
		{
			sum += texture(textureImage, textureCoord + vec2(i, j) * texelSize);
		}
	}
	return sum / ((2 * radius + 1) * 2);
}


vec4 gaussianFive()
{
	vec2 texelSize = 1.0f / screenSize;
	vec4 sum = vec4(0);
	float to_divide = 273;
	float kernel[25] = float[](1, 4, 7, 4, 1, 4, 16, 26, 16, 4, 7, 26, 41, 26, 7, 4, 16, 26, 16, 4, 1, 4, 7, 4, 1);

	for(int i = -2; i <= 2; i++)
	{
		for(int j = -2; j <= 2; j++)
		{
			sum += texture(textureImage, textureCoord + vec2(i, j) * texelSize) * kernel[5 * (i + 2) + (j + 2)];
		}
	}
	return sum / to_divide;
}

vec4 sobel()
{
	vec2 texelSize = 1.0f / screenSize;
	float gx[9] = float[](1, 0, -1, 2, 0, -2, 1, 0, -1);
	float gy[9] = float[](1, 2, 1, 0, 0, 0, -1, -2, -1);
	vec4 dx = vec4(0);
	vec4 dy = vec4(0);

	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			dx += texture(textureImage, textureCoord + vec2(i, j) * texelSize) * gx[3 * (i + 1) + (j + 1)];
		}
	}

		for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			dy += texture(textureImage, textureCoord + vec2(i, j) * texelSize) * gy[3 * (i + 1) + (j + 1)];
		}
	}

	return sqrt(dx*dx + dy*dy);
}

vec4 prewitt()
{
	vec2 texelSize = 1.0f / screenSize;
	float gx[9] = float[](1, 0, -1, 1, 0, -1, 1, 0, -1);
	float gy[9] = float[](1, 1, 1, 0, 0, 0, -1, -1, -1);
	vec4 dx = vec4(0);
	vec4 dy = vec4(0);

	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			dx += texture(textureImage, textureCoord + vec2(i, j) * texelSize) * gx[3 * (i + 1) + (j + 1)];
		}
	}

		for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			dy += texture(textureImage, textureCoord + vec2(i, j) * texelSize) * gy[3 * (i + 1) + (j + 1)];
		}
	}

	return sqrt(dx*dx + dy*dy);
}

vec4 roberts()
{
	vec2 texelSize = 1.0f / screenSize;
	float gx[9] = float[](0, 0, 0, 0, 1, 0, 0, 0, -1);
	float gy[9] = float[](0, 0, 0, 0, 0, 1, 0, -1, 0);
	vec4 dx = vec4(0);
	vec4 dy = vec4(0);

	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			dx += texture(textureImage, textureCoord + vec2(i, j) * texelSize) * gx[3 * (i + 1) + (j + 1)];
		}
	}

		for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			dy += texture(textureImage, textureCoord + vec2(i, j) * texelSize) * gy[3 * (i + 1) + (j + 1)];
		}
	}

	return sqrt(dx*dx + dy*dy);
}

vec4 laplacian()
{
	vec2 texelSize = 1.0f / screenSize;
	float lpl[9] = float[](-1, -1, -1, -1, 8, -1, -1, -1, -1);
	vec4 d = vec4(0);

	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			d += texture(textureImage, textureCoord + vec2(i, j) * texelSize) * lpl[3 * (i + 1) + (j + 1)];
		}
	}

	return d;
}

vec4 edge_detection_laplacian()
{
	vec2 texelSize = 1.0f / screenSize;
	float lpl[9] = float[](-1, -1, -1, -1, 8, -1, -1, -1, -1);
	float minv = 255;
	float maxv = -255;
	float prag = 0.05;
	float results[9];

	for(int bi = -1; bi <= 1; bi++)
	{
		for(int bj = -1; bj <= 1; bj++)
		{
			vec4 d = vec4(0);

			for(int i = -1; i <= 1; i++)
			{
				for(int j = -1; j <= 1; j++)
				{
					d += texture(textureImage, textureCoord + vec2(i + bi, j + bj) * texelSize) * lpl[3 * (i + 1) + (j + 1)];
				}
			}

			results[3 * (bi + 1) + (bj + 1)] = d.r; 
		}
	}

	for (int i = 0; i < 9; ++i)
	{
		if (results[i] < minv) minv = results[i];
		if (results[i] > maxv) maxv = results[i];
	}
	return (minv < -prag && maxv > prag) ? vec4(255, 255, 255, 1) : vec4(0, 0, 0, 1);
}

void main()
{
	switch (outputMode)
	{
		case 1:
		{
			out_color = grayscale();
			break;
		}

		case 2:
		{
			out_color = blur(3);
			break;
		}

		case 3:
		{
			out_color = median();
			break;
		}

		case 4:
		{
			out_color = average(3);
			break;
		}

		case 5:
		{
			out_color = edge_detection_laplacian();
			break;
		}

		default:
			out_color = texture(textureImage, textureCoord);
			break;
	}
}