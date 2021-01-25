#include "Laborator8.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

Laborator8::Laborator8()
{
	outputMode = 0;
	gpuProcessing = false;
	saveScreenToImage = false;
	window->SetSize(600, 600);
}

Laborator8::~Laborator8()
{
}

static FrameBuffer *processed;

void Laborator8::Init()
{
	// Load default texture fore imagine processing 
	originalImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "blurredGS.png", nullptr, "image", true, true);
	processedImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "blurredGS.png", nullptr, "newImage", true, true);

	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	static std::string shaderPath = "Source/Laboratoare/Laborator8/Shaders/";

	// Create a shader program for particle system
	{
		Shader *shader = new Shader("ImageProcessing");
		shader->AddShader((shaderPath + "VertexShader.glsl").c_str(), GL_VERTEX_SHADER);
		shader->AddShader((shaderPath + "FragmentShader.glsl").c_str(), GL_FRAGMENT_SHADER);

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

void Laborator8::FrameStart()
{
}

void Laborator8::Update(float deltaTimeSeconds)
{
	ClearScreen();

	auto shader = shaders["ImageProcessing"];
	shader->Use();

	if (saveScreenToImage)
	{
		window->SetSize(originalImage->GetWidth(), originalImage->GetHeight());
	}

	int flip_loc = shader->GetUniformLocation("flipVertical");
	glUniform1i(flip_loc, saveScreenToImage ? 0 : 1);

	int screenSize_loc = shader->GetUniformLocation("screenSize");
	glm::ivec2 resolution = window->GetResolution();
	glUniform2i(screenSize_loc, resolution.x, resolution.y);

	int outputMode_loc = shader->GetUniformLocation("outputMode");
	glUniform1i(outputMode_loc, outputMode);

	int gpuProcessing_loc = shader->GetUniformLocation("outputMode");
	glUniform1i(outputMode_loc, outputMode);

	int locTexture = shader->GetUniformLocation("textureImage");
	glUniform1i(locTexture, 0);
	auto textureImage = (gpuProcessing == true) ? originalImage : processedImage;
	textureImage->BindToTextureUnit(GL_TEXTURE0);

	RenderMesh(meshes["quad"], shader, glm::mat4(1));

	if (saveScreenToImage)
	{
		saveScreenToImage = false;
		GLenum format = GL_RGB;
		if (originalImage->GetNrChannels() == 4)
		{
			format = GL_RGBA;
		}
		glReadPixels(0, 0, originalImage->GetWidth(), originalImage->GetHeight(), format, GL_UNSIGNED_BYTE, processedImage->GetImageData());
		processedImage->UploadNewData(processedImage->GetImageData());
		SaveImage("shader_processing_" + std::to_string(outputMode));

		float aspectRatio = static_cast<float>(originalImage->GetWidth()) / originalImage->GetHeight();
		window->SetSize(static_cast<int>(600 * aspectRatio), 600);
	}
}

void Laborator8::FrameEnd()
{
	DrawCoordinatSystem();
}

void Laborator8::OnFileSelected(std::string fileName)
{
	if (fileName.size())
	{
		std::cout << fileName << endl;
		originalImage = TextureManager::LoadTexture(fileName.c_str(), nullptr, "image", true, true);
		processedImage = TextureManager::LoadTexture(fileName.c_str(), nullptr, "newImage", true, true);

		float aspectRatio = static_cast<float>(originalImage->GetWidth()) / originalImage->GetHeight();
		window->SetSize(static_cast<int>(600 * aspectRatio), 600);
	}
}

void Laborator8::GrayScale()
{
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (i * imageSize.x + j);

			// Reset save image data
			char value = static_cast<char>(data[offset + 0] * 0.2f + data[offset + 1] * 0.71f + data[offset + 2] * 0.07);
			memset(&newData[offset], value, 3);
		}
	}

	processedImage->UploadNewData(newData);
}

// Not implemented
void Laborator8::Gaussian()
{
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (i * imageSize.x + j);

			// Reset save image data
			char value = static_cast<char>(data[offset + 0] * 0.33f + data[offset + 1] * 0.33f + data[offset + 2] * 0.33f);
			memset(&newData[offset], value, 3);
		}
	}

	processedImage->UploadNewData(newData);
}

// Not implemented
void Laborator8::Average()
{
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	const int radius = 2;

	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (i * imageSize.x + j);

			int red, green, blue;
			red = green = blue = 0;
			//for (int ii = (i - 2 > 0) ? i - 2 : 0; ii < (i + 2 < imageSize.y) ? i + 2 : i; ++ii)
			//{
			//	for (int jj = (j - 2 > 0) ? j - 2 : 0; jj < (j + 2 < imageSize.x) ? j + 2 : j; ++jj)
			//	{
			//		int kernOffset = channels * (ii * imageSize.x + jj);
			//		printf("%d %d %d\n", ii, jj, kernOffset);
			//		red += data[kernOffset + 0];
			//		green += data[kernOffset + 1];
			//		blue += data[kernOffset + 2];
			//	}
			//}
			newData[offset] = static_cast<char>(red);
			newData[offset + 1] = static_cast<char>(green);
			newData[offset + 2] = static_cast<char>(blue);
		}
	}

	processedImage->UploadNewData(newData);
}

void Laborator8::Sepia()
{
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());

	for (int i = 0; i < imageSize.y; i++)
	{
		for (int j = 0; j < imageSize.x; j++)
		{
			int offset = channels * (i * imageSize.x + j);

			// Reset save image data
			float rvalue = static_cast<float>(data[offset + 0] * 0.393f + data[offset + 1] * 0.769f + data[offset + 2] * 0.189f);
			float gvalue = static_cast<float>(data[offset + 0] * 0.349f + data[offset + 1] * 0.686f + data[offset + 2] * 0.168f);
			float bvalue = static_cast<float>(data[offset + 0] * 0.272f + data[offset + 1] * 0.534f + data[offset + 2] * 0.131f);
			if (rvalue > 255) rvalue = 255;
			if (gvalue > 255) gvalue = 255;
			if (bvalue > 255) bvalue = 255;
			newData[offset] = static_cast<char>(rvalue);
			newData[offset + 1] = static_cast<char>(gvalue);
			newData[offset + 2] = static_cast<char>(bvalue);
		}
	}

	processedImage->UploadNewData(newData);
}

void Laborator8::SaveImage(std::string fileName)
{
	cout << "Saving image! ";
	processedImage->SaveToFile((fileName + ".png").c_str());
	cout << "[Done]" << endl;
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void Laborator8::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
};

void Laborator8::OnKeyPress(int key, int mods)
{

	if (key == GLFW_KEY_E)
	{
		gpuProcessing = !gpuProcessing;
		cout << "Processing on GPU: " << (gpuProcessing ? "true" : "false") << endl;
	}

	if (key - GLFW_KEY_0 >= 0 && key < GLFW_KEY_8)
	{
		outputMode = key - GLFW_KEY_0;
	}

	if (key - GLFW_KEY_F1 >= 0 && key < GLFW_KEY_F8)
	{
		if (!gpuProcessing)
		{
			switch (key - GLFW_KEY_F1 + 1)
			{
			case 1:
			{
				GrayScale();
				break;
			}

			case 2:
			{
				Gaussian();
				break;
			}

			case 3:
			{
				Average();
				break;
			}

			case 4:
			{
				Sepia();
				break;
			}
			default:
				break;
			}
		}
	}

	if (key == GLFW_KEY_S && mods & GLFW_MOD_CONTROL)
	{
		if (!gpuProcessing)
		{
			SaveImage("processCPU_" + std::to_string(outputMode));
		}
		else
		{
			saveScreenToImage = true;
		}
	}
};

void Laborator8::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Laborator8::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Laborator8::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Laborator8::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator8::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Laborator8::OnWindowResize(int width, int height)
{
	// treat window resize event
}
