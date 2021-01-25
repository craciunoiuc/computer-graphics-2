#include "Proiect.h"

#include <vector>
#include <iostream>
#include <chrono>

#include <Core/Engine.h>
#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "box.h"
#include "cylinder.h"
#include "cone.h"

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

Proiect::Proiect()
{
	outputMode = 0;
	window->SetSize(1280, 720);
}

Proiect::~Proiect()
{
}

FrameBuffer *processed;

void Proiect::Init()
{
	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(0, 4, 0), glm::quat(glm::vec3(-30 * TO_RADIANS, 0, 0)));
	camera->transform->SetMoveSpeed(30);
	camera->Update();
	std::string shaderPath = "Source/Teme/Proiect/Shaders/";

	// Create a shader program for rendering to texture
	{
		Shader* shader = new Shader("Ray");
		shader->AddShader(shaderPath + "RayVert.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "RayFrag.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Load default texture fore imagine processing 
	originalImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "image", true, true);
	processedImage = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "Cube/posx.png", nullptr, "newImage", true, true);

	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;

	}


	// cpu shader
	{
		Shader *shader = new Shader("ImageProcessing");
		shader->AddShader((shaderPath + "VertexShader.glsl").c_str(), GL_VERTEX_SHADER);
		shader->AddShader((shaderPath + "FragmentShader.glsl").c_str(), GL_FRAGMENT_SHADER);

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Print usage one time to console
	std::cout << "Controls:\n"
		<< " - '1-3' Select scenes 1-3\n"
		<< " - '-' Run on CPU\n"
		<< " - '=' Run on GPU\n"
		<< " - 'h' Extra information\n"
		<< " - 'p' Print FPS\n"
		<< " - 'o' Print FPS continuously\n";
}

void Proiect::FrameStart()
{
}

// Prepares the data specifically for the GPU and calls the DrawScene functions
void Proiect::DrawOnGPU()
{
	Shader* shader = shaders["Ray"];

	shaders["Ray"]->Use();
	glm::mat4 modelMatrix = glm::mat4(1);
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));
	glUniform1f(glGetUniformLocation(shader->program, "z_near"), GetSceneCamera()->GetProjectionInfo().zNear);
	glUniform1f(glGetUniformLocation(shader->program, "z_far"), GetSceneCamera()->GetProjectionInfo().zFar);

	glUniform3f(glGetUniformLocation(shader->program, "TRS_values"), translationValue, rotationValue, scalingValue);

	vector<int> idxs;
	switch (scene) {
	case 1:
		idxs = {0, 2, 0, 2, 0, 1, 0, 1, 0, 1};
		break;
	case 2:
		idxs = {2, 5, 2, 14, 1, 4, 1, 3, 1, 4};
		break;
	case 3:
		idxs = {5, 8, 14, 24, 4, 8, 3, 5, 4, 5};
		break;

	default: break;
	}

	glUniform1i(glGetUniformLocation(shader->program, "cube_start"),     idxs[0]);
	glUniform1i(glGetUniformLocation(shader->program, "cube_stop"),      idxs[1]);
	glUniform1i(glGetUniformLocation(shader->program, "sphere_start"),   idxs[2]);
	glUniform1i(glGetUniformLocation(shader->program, "sphere_stop"),    idxs[3]);
	glUniform1i(glGetUniformLocation(shader->program, "cylinder_start"), idxs[4]);
	glUniform1i(glGetUniformLocation(shader->program, "cylinder_stop"),  idxs[5]);
	glUniform1i(glGetUniformLocation(shader->program, "cone_start"),     idxs[6]);
	glUniform1i(glGetUniformLocation(shader->program, "cone_stop"),      idxs[7]);
	glUniform1i(glGetUniformLocation(shader->program, "light_start"),    idxs[8]);
	glUniform1i(glGetUniformLocation(shader->program, "light_stop"),     idxs[9]);

	meshes["quad"]->Render();

}

void Proiect::Update(float deltaTimeSeconds)
{
	ClearScreen();
	timeToOneSec += deltaTimeSeconds;
	currTime += deltaTimeSeconds;
	frames++;
	if (timeToOneSec >= 1.) {
		framesPerSec = (timeToOneSec == 1) ? frames : frames - 1;
		frames = 0;
		timeToOneSec = 0;
		if (continuousPrint && onGPU) std::cout << framesPerSec << "\n";
	}
	if (scene == -1)
		return;
	rotationValue += 4 * rotationDirection * animationSpeed * deltaTimeSeconds;
	scalingValue += 0.05 * scalingDirection * animationSpeed * deltaTimeSeconds;
	translationValue += translationDirection * animationSpeed * deltaTimeSeconds;

	if (rotationValue > 360) rotationDirection = -1;
	if (scalingValue > 2) scalingDirection = -1;
	if (translationValue > 2) translationDirection = -1;

	if (rotationValue < -360) rotationDirection = +1;
	if (scalingValue < 0.1) scalingDirection = +1;
	if (translationValue < -2) translationDirection = +1;
	if (onGPU) DrawOnGPU(); else DrawOnCPU();
}

void Proiect::DrawOnCPU()
{
	auto shader = shaders["ImageProcessing"];
	shader->Use();

	int locTexture = shader->GetUniformLocation("textureImage");
	glUniform1i(locTexture, 0);
	auto textureImage = processedImage;
	textureImage->BindToTextureUnit(GL_TEXTURE0);

	RenderMesh(meshes["quad"], shader, glm::mat4(1));
}

void Proiect::FrameEnd()
{
}

hittable_list Proiect::w_scene() {
	hittable_list world;

	switch (scene) {
	case 1:
	{
		auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
		world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

		auto material4 = make_shared<lambertian>(color(0.1, 1, 0.1));
		world.add(make_shared<cone>(point3(6.3, 1, 0), 0.5, 1, material4));

		auto material2 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
		world.add(make_shared<box>(point3(.5, 0.2, -1), point3(-0.3, 1, -1.8), material2));

		auto material3 = make_shared<dielectric>(1.5);
		world.add(make_shared<sphere>(point3(2, 0.2, -1), 0.2, material3));

		auto material1 = make_shared<lambertian>(color(0.1, 1, 0.1));
		world.add(make_shared<cylinder>(point3(-3, 0.7, -1), 0.5, 1, material1));

		//std::cout << "scene 1\n";
	}
	break;
	case 2:
	{
		auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
		world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

		auto material4 = make_shared<metal>(color(0.1, 1, 0.1), 0.2);
		world.add(make_shared<cone>(point3(6.3, 1, 0), 0.5, 1, material4));
		auto material9 = make_shared<metal>(color(0, 1, 1), 0.2);
		world.add(make_shared<cone>(point3(5, 1, 2), 0.5, 1, material9));

		auto material2 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
		world.add(make_shared<box>(point3(.5, 0.2, -1), point3(-0.3, 1, -1.8), material2));
		auto material21 = make_shared<metal>(color(1, 0.4, 0.4), 0.0);
		world.add(make_shared<box>(point3(.5, 0.4, -2), point3(-0.3, 1, -2.8), material21));
		auto material22 = make_shared<metal>(color(0.4, 1, 0.4), 0.0);
		world.add(make_shared<box>(point3(.5, 0.8, -3), point3(-0.3, 1.3, -3.8), material22));

		auto material3 = make_shared<dielectric>(1.5);
		world.add(make_shared<sphere>(point3(2, 0.2, -1), 0.2, material3));
		auto material7 = make_shared<dielectric>(1.5);
		world.add(make_shared<sphere>(point3(2.4, 0.6, 2), 0.2, material7));
		world.add(make_shared<sphere>(point3(2, 1.6, 3), 0.3, material7));
		auto material6 = make_shared<dielectric>(1.5);
		world.add(make_shared<sphere>(point3(1.5, 0.8, 0), 0.4, material6));
		world.add(make_shared<sphere>(point3(1.5, 1.4, 0), 0.4, material6));
		world.add(make_shared<sphere>(point3(-1.5, 1.4, 0), 0.4, material6));
		world.add(make_shared<sphere>(point3(-2, 1.4, 1), 0.4, material6));
		world.add(make_shared<sphere>(point3(-2.3, 1.4, 1), 0.4, material6));
		world.add(make_shared<sphere>(point3(-3, 1.5, 2.5), 0.35, material6));
		world.add(make_shared<sphere>(point3(-2.7, 2, 2), 0.4, material6));
		auto material8 = make_shared<metal>(color(1, 1, 0), 0.2);
		world.add(make_shared<sphere>(point3(-5, 1.4, 2.3), 0.4, material8));
		auto material10 = make_shared<metal>(color(1, 0, 0), 0.2);
		world.add(make_shared<sphere>(point3(-4.3, 2, 3.4), 0.4, material10));



		auto material1 = make_shared<lambertian>(color(1, 1, 0.4));
		world.add(make_shared<cylinder>(point3(-3, 0.7, -1), 0.5, 1, material1));

		auto material5 = make_shared<metal>(color(0.5, 1, 1), 0.2);
		world.add(make_shared<cylinder>(point3(-4.3, 0.7, -1), 0.5, 1, material5));
		//std::cout << "scene 2\n";
	}
	break;
	case 3:
	{
		auto ground_material = make_shared<metal>(color(0.9, 0.9, 0.9), 0.5);
		world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));
		world.add(make_shared<sphere>(point3(-1020, 0, 0), 1000, ground_material));
		world.add(make_shared<sphere>(point3(0, 0, 1004), 1000, ground_material));
		world.add(make_shared<sphere>(point3(0, 1008, 0), 1000, ground_material));

		/*auto material3 = make_shared<dielectric>(1.5);
		world.add(make_shared<sphere>(point3(2, 0.2, -1), 0.2, material3));
		auto material5 = make_shared<metal>(color(0.5, 1, 1), 1);
		world.add(make_shared<cylinder>(point3(-4.3, 0.7, -1), 0.5, 1, material5));*/

		int cnt = 0;

		int rows[] = { -11, -7, -5, 2, 8 };
		int cols[] = { 11, 7, 5, -2, -9 };

		for (auto a : rows) {
			for (auto b : cols) {
				auto choose_mat = random_double();
				point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

				if ((center - point3(4, 0.2, 0)).length() > 0.9) {
					shared_ptr<material> material;

					if (choose_mat < 0.8) {
						// diffuse
						auto albedo = color::random() * color::random();
						material = make_shared<lambertian>(albedo);
						if (cnt % 4 == 0)
							world.add(make_shared<sphere>(center, 0.2, material));
						else if (cnt % 4 == 1)
							world.add(make_shared<cone>(center, 0.2, 0.4, material));
						else if (cnt % 4 == 2)
							world.add(make_shared<box>(center + point3(-1, -1, -1), center + point3(1, 1, 1), material));
						else if (cnt % 4 == 3)
							world.add(make_shared<cylinder>(center, 0.2, 0.4, material));
					}
					else if (choose_mat < 0.95) {
						// metal
						auto albedo = color::random(0.5, 1);
						auto fuzz = random_double(0, 0.5);
						material = make_shared<metal>(albedo, fuzz);
						if (cnt % 3 == 0)
							world.add(make_shared<sphere>(center, 0.2, material));
						else if (cnt % 3 == 1)
							world.add(make_shared<cylinder>(center, 0.2, 0.4, material));
						else if (cnt % 4 == 2)
							world.add(make_shared<box>(center + point3(-1, -1, -1), center + point3(1, 1, 1), material));
					}
					else {
						// glass
						material = make_shared<dielectric>(1.5);
						world.add(make_shared<sphere>(center, 0.2, material));
						cout << "entered 333333333333\n";

					}
					cnt++;
				}
			}
		}

		auto material1 = make_shared<dielectric>(1.5);
		world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

		auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
		world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

		auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
		world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
	}
	break;
	}

	return world;
}

color ray_color(const ray& r, const hittable& world, int depth) {
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)) {
		ray scattered;
		color attenuation;
		assert(rec.mat_ptr != nullptr);
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}


void Proiect::Ray()
{
	using namespace std::chrono;
	auto start = high_resolution_clock::now();
	unsigned int channels = originalImage->GetNrChannels();
	unsigned char* data = originalImage->GetImageData();
	unsigned char* newData = processedImage->GetImageData();

	if (channels < 3)
		return;

	glm::ivec2 imageSize = glm::ivec2(originalImage->GetWidth(), originalImage->GetHeight());
	// Image

	const auto aspect_ratio = 16.0 / 9.0;
	const int samples_per_pixel = 20;
	const int max_depth = 20;

	// World
	auto world = w_scene();

	// Camera

	point3 lookfrom(-13, 4, 5);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10;
	auto aperture = 0.1;

	camera_cpu cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);


	#pragma omp parallel for
	for (int j = imageSize.y - 1; j >= 0; j--)
	{
		for (int i = 0; i < imageSize.x; i++)
		{

			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_double()) / ((double)imageSize.x - 1);
				auto v = (j + random_double()) / ((double)imageSize.y - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			int offset = channels * (j * imageSize.x + i);

			write_color(&newData[offset], pixel_color, samples_per_pixel);
		}
	}

	processedImage->UploadNewData(newData);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(stop - start);
	if (continuousPrint) {
		cout << 3600.f / static_cast<float>(duration.count()) << endl;
	}
}

void Proiect::SaveImage(std::string fileName)
{
	cout << "Saving image! ";
	processedImage->SaveToFile((fileName + ".png").c_str());
	cout << "[Done]" << endl;
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void Proiect::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
};

void Proiect::OnKeyPress(int key, int mods)
{

	if (!onGPU) {
		if (key == GLFW_KEY_S && mods & GLFW_MOD_CONTROL)
		{
			SaveImage("processCPU_" + std::to_string(outputMode));
		}
	}
	switch (key)
	{
		case GLFW_KEY_1:
		{
			scene = 1;
			if (!onGPU) {
				Ray();
			}
			break;
		}
		case GLFW_KEY_2:
		{
			scene = 2;
			if (!onGPU) {
				Ray();
			}
			break;
		}
		case GLFW_KEY_3:
		{
			scene = 3;
			if (!onGPU) {
				Ray();
			}
			break;
		}
		case GLFW_KEY_MINUS:
		{
			onGPU = false;
			scene = -1;
			break;
		}
		case GLFW_KEY_EQUAL:
		{
			onGPU = true;
			scene = -1;
			break;
		}
		case GLFW_KEY_P:
		{
			std::cout << framesPerSec << "\n";
			break;
		}
		case GLFW_KEY_O:
		{
			continuousPrint = !continuousPrint;
			break;
		}
		case GLFW_KEY_H:
		{
			std::cout << "The program will start running on the CPU.\n";
			break;
		}
		default: break;
	}
	
};

void Proiect::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Proiect::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Proiect::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Proiect::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Proiect::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Proiect::OnWindowResize(int width, int height)
{
	// treat window resize event
}
