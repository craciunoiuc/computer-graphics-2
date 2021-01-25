#include "Tema2.h"

#include <vector>
#include <iostream>
#include <Core/Engine.h>

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

Tema2::Tema2()
{
	window->SetSize(600, 600);
}

Tema2::~Tema2()
{
}

// Returns the maximum of 2 numbers
inline float Tema2::myMax(float a, float b) {
	return (a > b) ? a : b;
}

// Returns the minimum of 2 numbers
inline float Tema2::myMin(float a, float b) {
	return (a > b) ? b : a;
}

// Converts a RGB triplet to a HSV one
glm::vec3 Tema2::RGBtoHSV(glm::vec3 pixel)
{
	glm::vec3 ret = glm::vec3(pixel.r / 255., pixel.g / 255., pixel.b / 255.);
	float maxi = myMax(myMax(ret.r, ret.g), ret.b);
	float mini = myMin(myMin(ret.r, ret.g), ret.b);
	float h = -1, s, v = maxi;
	float diff = maxi - mini;
	s = (maxi == 0) ? 0 : diff / maxi;
	if (maxi == mini) {
		h = 0;
	}
	else {
		if (maxi == ret.r) {
			h = (ret.g - ret.b) / diff + ((ret.g < ret.b) ? 6 : 0);
		}

		if (maxi == ret.g) {
			h = (ret.b - ret.r) / diff + 2;
		}

		if (maxi == ret.b) {
			h = (ret.r - ret.g) / diff + 4;
		}
	}
	return glm::vec3(h * 60, s, v);
}

// Converts an RGB triplet to a YCbCr one
glm::vec3 Tema2::RGBtoYCbCr(glm::vec3 pixel)
{
	float y = 0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b;
	float cb = 128 - 0.168736 * pixel.r - 0.331364 * pixel.g + 0.5 * pixel.b;
	float cr = 128 + 0.5 * pixel.r - 0.418688 * pixel.g - 0.081312 * pixel.b;

	return glm::vec3(y, cb, cr);
}

// The function testes the homogenity of an area. It is configured to work for
// any type of chosen format, but only one at a time.
bool Tema2::Omogen(int N1, int M1, int N2, int M2, glm::vec3 T)
{
#if USE_RGB
	int maxr = 0, maxg = 0, maxb = 0;
	int minr = 256, ming = 256, minb = 256;
#endif

#if USE_HSV
	float maxh = 0, maxs = 0, maxv = 0;
	float minh =400, mins = 2, minv = 2;
#endif

#if USE_YCbCr
	float maxy = 0, maxcb = 0, maxcr = 0;
	float miny = 256, mincb = 256, mincr = 256;
#endif

	for (int i = M1; i < M2; ++i) {
		for (int j = N1; j < N2; ++j) {
			int offset = channels * (i * imageSize.x + j);
#if USE_RGB
			if (data[offset + 0] < minr) minr = data[offset + 0];
			if (data[offset + 0] > maxr) maxr = data[offset + 0];
			if (data[offset + 1] < ming) ming = data[offset + 1];
			if (data[offset + 1] > maxg) maxg = data[offset + 1];
			if (data[offset + 2] < minb) minb = data[offset + 2];
			if (data[offset + 2] > maxb) maxb = data[offset + 2];
#endif

#if USE_HSV
			glm::vec3 result = Tema2::RGBtoHSV(glm::vec3(data[offset + 0], data[offset + 1], data[offset + 2]));
			if (result.x < minh) minh = result.x;
			if (result.x > maxh) maxh = result.x;
			if (result.y < mins) mins = result.y;
			if (result.y > maxs) maxs = result.y;
			if (result.z < minv) minv = result.z;
			if (result.z > maxv) maxv = result.z;
#endif

#if USE_YCbCr
			glm::vec3 result = Tema2::RGBtoYCbCr(glm::vec3(data[offset + 0], data[offset + 1], data[offset + 2]));
			if (result.x < miny) miny = result.x;
			if (result.x > maxy) maxy = result.x;
			if (result.y < mincb) mincb = result.y;
			if (result.y > maxcb) maxcb = result.y;
			if (result.z < mincr) mincr = result.z;
			if (result.z > maxcr) maxcr = result.z;
#endif
		}
	}
#if USE_RGB
	return (maxr - minr < T.r && maxg - ming < T.g && maxb - minb < T.b) ? 1 : 0;
#endif

#if USE_HSV
	return (maxh - minh < T.x && maxs - mins < T.y && maxv - minv < T.z) ? 1 : 0;
#endif

#if USE_YCbCr
	return (maxy - miny < T.x && maxcb - mincb < T.y && maxcr - mincr < T.z) ? 1 : 0;
#endif

	return -1;
}

// Function that segments the image recursively. It comes back from recursion when
// one zone is homogenous.
void Tema2::Segment(int N1, int M1, int N2, int M2, glm::vec3 T, int *N)
{
	long sr, sg, sb;
	if (!Tema2::Omogen(N1, M1, N2, M2, T) && (N2 - N1) > 1 && (M2 - M1) > 1)
	{
		Tema2::Segment(N1, M1, (N1 + N2) / 2, (M1 + M2) / 2, T, N);
		Tema2::Segment((N1 + N2) / 2, M1, N2, (M1 + M2) / 2, T, N);
		Tema2::Segment(N1, (M1 + M2) / 2, (N1 + N2) / 2, M2, T, N);
		Tema2::Segment((N1 + N2) / 2, (M1 + M2) / 2, N2, M2, T, N);
	}
	else {
		(*N)++;
		sr = sg = sb = 0;

		for (int i = M1; i < M2; ++i) {
			for (int j = N1; j < N2; ++j) {
				int offset = channels * (i * imageSize.x + j);
				sr += data[offset + 0];
				sg += data[offset + 1];
				sb += data[offset + 2];
			}
		}

		sr /= (N2 - N1) * (M2 - M1);
		sg /= (N2 - N1) * (M2 - M1);
		sb /= (N2 - N1) * (M2 - M1);

		for (int i = M1; i < M2; ++i) {
			for (int j = N1; j < N2; ++j) {
				int offset = channels * (i * imageSize.x + j);
				newData[offset + 0] = (unsigned char)sr;
				newData[offset + 1] = (unsigned char)sg;
				newData[offset + 2] = (unsigned char)sb;
			}
		}
	}
}

void Tema2::Init()
{
	debug_transforms = false;
	// Load default texture for imagine processing 
	originalImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "image", true, true);
	processedImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "image", true, true);

	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	static std::string shaderPath = "Source/Teme/Tema2/Shaders/";

	{
		Shader *shader = new Shader("ImageProcessing");
		shader->AddShader((shaderPath + "VertexShader.glsl").c_str(), GL_VERTEX_SHADER);
		shader->AddShader((shaderPath + "FragmentShader.glsl").c_str(), GL_FRAGMENT_SHADER);

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Test RGB transforms
	if (debug_transforms) {
		float r, g, b;
		std::cin >> r >> g >> b;
		std::cout << RGBtoHSV(glm::vec3(r, g, b));
		std::cout << RGBtoYCbCr(glm::vec3(r, g, b));
	}

	int N = 0;
	channels = originalImage->GetNrChannels();
	data = originalImage->GetImageData();
	newData = processedImage->GetImageData();
	imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	// Test values
#if USE_RGB | USE_YCbCr
	const float maxDiff1 = 120, maxDiff2 = 120, maxDiff3 = 120;
#else
	const float maxDiff1 = 120, maxDiff2 = 0.7, maxDiff3 = 0.7;
#endif

	// Run Segmentation
	Tema2::Segment(0, 0, originalImage->GetWidth(), originalImage->GetHeight(), glm::vec3(maxDiff1, maxDiff2, maxDiff3), &N);
	std::cout << "Segments: " << N << "\n";
	processedImage->UploadNewData(newData);
}

void Tema2::FrameStart()
{
}

void Tema2::Update(float deltaTimeSeconds)
{
	ClearScreen();

	auto shader = shaders["ImageProcessing"];
	shader->Use();

	// Display to quad
	int locTexture = shader->GetUniformLocation("textureImage");
	glUniform1i(locTexture, 0);
	auto textureImage = processedImage;
	textureImage->BindToTextureUnit(GL_TEXTURE0);


	RenderMesh(meshes["quad"], shader, glm::mat4(1));
}

void Tema2::FrameEnd()
{
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
};

void Tema2::OnKeyPress(int key, int mods)
{
	// add key press event
};

void Tema2::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Tema2::OnWindowResize(int width, int height)
{
	// treat window resize event
}
