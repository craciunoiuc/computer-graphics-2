#include "Tema1.h"

#include <vector>
#include <iostream>
#include <stb/stb_image.h>
#include <Core/Engine.h>

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(0, 8, 8), glm::quat(glm::vec3(-40 * TO_RADIANS, -40 * TO_RADIANS, 0)));
	camera->Update();


	std::string shaderPath = "Source/Teme/Tema1/Shaders/";
	std::string texturePath = RESOURCE_PATH::TEXTURES + "Cube/";

	// River Shader
	{
		Shader* shader = new Shader("River");
		shader->AddShader(shaderPath + "RiverVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "RiverGS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "RiverFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Cubemap Shader
	{
		Shader* shader = new Shader("Cubemap");
		shader->AddShader(shaderPath + "CubemapVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "CubemapFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Mountain Shader
	{
		Shader* shader = new Shader("Mountain");
		shader->AddShader(shaderPath + "MountainVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "MountainGS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "MountainFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Boat Shader
	{
		Shader* shader = new Shader("Boat");
		shader->AddShader(shaderPath + "BoatVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "BoatFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Particle Shader
	{

		Shader* shader = new Shader("Particle");
		shader->AddShader(shaderPath + "ParticleVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "ParticleFS.glsl", GL_FRAGMENT_SHADER);
		shader->AddShader(shaderPath + "ParticleGS.glsl", GL_GEOMETRY_SHADER);

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;


		nrParticles = 1'000;

		particleEffect = new ParticleEffect<Tema1::Particle>();
		particleEffect->Generate(nrParticles, true);

		auto particleSSBO = particleEffect->GetParticleBuffer();
		Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

		int cubeSize = 2;
		int hSize = cubeSize / 2;

		for (unsigned int i = 0; i < nrParticles; i++)
		{
			glm::vec4 pos(1);
			pos.x = (rand() % cubeSize - hSize) / 10.0f;
			pos.y = (rand() % cubeSize - hSize) / 10.0f;
			pos.z = (rand() % cubeSize - hSize) / 10.0f;

			glm::vec4 speed(0);
			speed.x = (rand() % 20 - 10) / 10.0f;
			speed.z = (rand() % 20 - 10) / 10.0f;
			speed.y = rand() % 2 + 2.0f;

			data[i].SetInitial(pos, speed);
		}
		particleSSBO->SetBufferData(data);
	}

	// Depth of Field Shader
	{
		Shader* shader = new Shader("Depth");
		shader->AddShader(shaderPath + "DepthVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "DepthFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Cube
	{
		Mesh* mesh = new Mesh("cube");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Sphere
	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Oildrum
	{
		Mesh* mesh = new Mesh("oil");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props", "oildrum.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Cubemap files
	cubeMapTextureID = UploadCubeMapTexture(
		texturePath + "posx.png",
		texturePath + "posy.png",
		texturePath + "posz.png",
		texturePath + "negx.png",
		texturePath + "negy.png",
		texturePath + "negz.png"
	);

	// Line
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(-4.0, -2.5,  1.0),
			glm::vec3(-4.0, 5.5,  1.0)
		};

		vector<unsigned short> indices =
		{
			0, 1
		};

		std::vector<glm::vec3> normals
		{
			glm::vec3(0, -1, 0),
			glm::vec3(0, 1, 0)
		};

		std::vector<glm::vec2> textureCoords
		{
			glm::vec2(0, 0),
			glm::vec2(0, 1)

		};
		meshes["surface"] = new Mesh("initial");
		meshes["surface"]->InitFromData(vertices, normals, textureCoords, indices);
		meshes["surface"]->SetDrawMode(GL_LINES);
	}

	// Quad
	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Water
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Teme/Tema1/Textures/water.jpg", GL_REPEAT);
		mapTextures["water"] = texture;
	}

	// Dirt
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D("Source/Teme/Tema1/Textures/dirt.jpg", GL_REPEAT);
		mapTextures["ground"] = texture;
	}

	// Particle
	{
		TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES, "particle.png");
	}

	// Random generation
	{
		mainRiver_p1 = glm::vec3(-8 - static_cast<float>(rand() % 10) / 4.f + 10.f, 1.0, -7.5 + static_cast<float>(rand() % 15) / 4.f - 10.f);
		mainRiver_p2 = glm::vec3(-6.5 - static_cast<float>(rand() % 10) / 4.f + 10.f, 1.0, 7.5 + static_cast<float>(rand() % 15) / 4.f - 10.f);
		mainRiver_p3 = glm::vec3(-5.5 - static_cast<float>(rand() % 10) / 4.f + 10.f, 1.0, 10.0 + static_cast<float>(rand() % 15) / 4.f - 10.f);
		mainRiver_p4 = glm::vec3(-8.0 - static_cast<float>(rand() % 10) / 2.f + 10.f, 1.0, 15.5 + static_cast<float>(rand() % 15) / 2.f - 10.f);

		firstSplice_p2 = glm::vec3(-8 - static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 22 + static_cast<float>(rand() % 10) / 5.f - 1.f);
		firstSplice_p3 = glm::vec3(-12 - static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 16 + static_cast<float>(rand() % 10) / 5.f - 1.f);
		firstSplice_p4 = glm::vec3(-14 - static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 24 + static_cast<float>(rand() % 10) / 5.f - 1.f);

		secondSplice_p2 = glm::vec3(-4 + static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 22 + static_cast<float>(rand() % 10) / 5.f - 1.f);
		secondSplice_p3 = glm::vec3(-5 + static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 23 + static_cast<float>(rand() % 10) / 5.f - 1.f);
		secondSplice_p4 = glm::vec3(-5 + static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 24 + static_cast<float>(rand() % 10) / 5.f - 1.f);

		thirdSplice_p2 = glm::vec3(5 + static_cast<float>(rand() % 8) / 5.f + 10.f, 1.0, 22 + static_cast<float>(rand() % 8) / 5.f - 1.f);
		thirdSplice_p3 = glm::vec3(5 + static_cast<float>(rand() % 8) / 5.f + 10.f, 1.0, 23 + static_cast<float>(rand() % 8) / 5.f - 1.f);
		thirdSplice_p4 = glm::vec3(5 + static_cast<float>(rand() % 8) / 5.f + 10.f, 1.0, 24 + static_cast<float>(rand() % 8) / 5.f - 1.f);

		currentRiver_p1 = mainRiver_p1;
		currentRiver_p2 = mainRiver_p2;
		currentRiver_p3 = mainRiver_p3;
		currentRiver_p4 = mainRiver_p4;

		framePosition = bezier(currentPosition, currentRiver_p1, currentRiver_p2, currentRiver_p3, currentRiver_p4);
		intersections = generateStartOfIntersections();
	}

	// New framebuffer
	{
		auto resolution = window->GetResolution();
		frameBuffer = new FrameBuffer();
		frameBuffer->Bind();
		frameBuffer->Generate(resolution.x, resolution.y, 1, true);
	}

}

void Tema1::FrameStart()
{

}

// Function from the lab to load the texture of the CubeMap
unsigned int Tema1::UploadCubeMapTexture(const std::string& posx, const std::string& posy, const std::string& posz, const std::string& negx, const std::string& negy, const std::string& negz)
{
	int width, height, chn;

	unsigned char* data_posx = stbi_load(posx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posy = stbi_load(posy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posz = stbi_load(posz.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negx = stbi_load(negx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negy = stbi_load(negy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negz = stbi_load(negz.c_str(), &width, &height, &chn, 0);

	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posz);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negz);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// free memory
	SAFE_FREE(data_posx);
	SAFE_FREE(data_posy);
	SAFE_FREE(data_posz);
	SAFE_FREE(data_negx);
	SAFE_FREE(data_negy);
	SAFE_FREE(data_negz);

	return textureID;
}

// Function from the lab to render a standard mesh
void Tema1::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader 
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture1)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 1);
	}

	// Draw the object instanced
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

}

// Function to change the current river
void Tema1::chooseNewRiver(int newRiver)
{
	switch (newRiver)
	{
		case 1:
		{
			currentRiver_p1 = mainRiver_p1;
			currentRiver_p2 = mainRiver_p2;
			currentRiver_p3 = mainRiver_p3;
			currentRiver_p4 = mainRiver_p4;
			break;
		}
		case 2:
		{
			currentRiver_p1 = mainRiver_p4;
			currentRiver_p2 = firstSplice_p2;
			currentRiver_p3 = firstSplice_p3;
			currentRiver_p4 = firstSplice_p4;
			break;
		}
		case 3:
		{
			currentRiver_p1 = mainRiver_p4;
			currentRiver_p2 = secondSplice_p2;
			currentRiver_p3 = secondSplice_p3;
			currentRiver_p4 = secondSplice_p4;
			break;
		}
		case 4:
		{
			currentRiver_p1 = mainRiver_p4;
			currentRiver_p2 = thirdSplice_p2;
			currentRiver_p3 = thirdSplice_p3;
			currentRiver_p4 = thirdSplice_p4;
			break;
		}
		default: break;
	}
}

void Tema1::Update(float deltaTimeSeconds)
{
	ClearScreen();

	frameBuffer->Bind();

	if (toggleWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Sky
	{
		Shader* shader = shaders["Cubemap"];
		shader->Use();

		glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(50));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		meshes["cube"]->Render();
	}

	// River
	{
		Shader* shader = shaders["River"];
		shader->Use();

		glUniform3f(glGetUniformLocation(shader->program, "mainRiver_p1"), mainRiver_p1.x, mainRiver_p1.y, mainRiver_p1.z);
		glUniform3f(glGetUniformLocation(shader->program, "mainRiver_p2"), mainRiver_p2.x, mainRiver_p2.y, mainRiver_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "mainRiver_p3"), mainRiver_p3.x, mainRiver_p3.y, mainRiver_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "mainRiver_p4"), mainRiver_p4.x, mainRiver_p4.y, mainRiver_p4.z);
		glUniform3f(glGetUniformLocation(shader->program, "firstSplice_p2"), firstSplice_p2.x, firstSplice_p2.y, firstSplice_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "firstSplice_p3"), firstSplice_p3.x, firstSplice_p3.y, firstSplice_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "firstSplice_p4"), firstSplice_p4.x, firstSplice_p4.y, firstSplice_p4.z);
		glUniform3f(glGetUniformLocation(shader->program, "secondSplice_p2"), secondSplice_p2.x, secondSplice_p2.y, secondSplice_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "secondSplice_p3"), secondSplice_p3.x, secondSplice_p3.y, secondSplice_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "secondSplice_p4"), secondSplice_p4.x, secondSplice_p4.y, secondSplice_p4.z);
		glUniform3f(glGetUniformLocation(shader->program, "thirdSplice_p2"), thirdSplice_p2.x, thirdSplice_p2.y, thirdSplice_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "thirdSplice_p3"), thirdSplice_p3.x, thirdSplice_p3.y, thirdSplice_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "thirdSplice_p4"), thirdSplice_p4.x, thirdSplice_p4.y, thirdSplice_p4.z);

		auto cameraPosition = GetSceneCamera()->transform->GetWorldPosition();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);
		int chooseRiver = 1;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		glUniform3f(shader->GetUniformLocation("camera_position"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
		Mesh* mesh = meshes["surface"];
		RenderMesh(mesh, shader, glm::mat4(1), mapTextures["water"]);

		chooseRiver = 2;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		glUniform3f(shader->GetUniformLocation("camera_position"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
		mesh = meshes["surface"];
		RenderMesh(mesh, shader, glm::mat4(1), mapTextures["water"]);

		chooseRiver = 3;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		glUniform3f(shader->GetUniformLocation("camera_position"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
		mesh = meshes["surface"];
		RenderMesh(mesh, shader, glm::mat4(1), mapTextures["water"]);

		chooseRiver = 4;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);

		mesh = meshes["surface"];
		RenderMesh(mesh, shader, glm::mat4(1), mapTextures["water"]);
	}

	// Mountains
	{
		Shader* shader = shaders["Mountain"];
		shader->Use();

		glUniform3f(glGetUniformLocation(shader->program, "mainRiver_p1"), mainRiver_p1.x, mainRiver_p1.y, mainRiver_p1.z);
		glUniform3f(glGetUniformLocation(shader->program, "mainRiver_p2"), mainRiver_p2.x, mainRiver_p2.y, mainRiver_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "mainRiver_p3"), mainRiver_p3.x, mainRiver_p3.y, mainRiver_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "mainRiver_p4"), mainRiver_p4.x, mainRiver_p4.y, mainRiver_p4.z);
		glUniform3f(glGetUniformLocation(shader->program, "firstSplice_p2"), firstSplice_p2.x, firstSplice_p2.y, firstSplice_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "firstSplice_p3"), firstSplice_p3.x, firstSplice_p3.y, firstSplice_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "firstSplice_p4"), firstSplice_p4.x, firstSplice_p4.y, firstSplice_p4.z);
		glUniform3f(glGetUniformLocation(shader->program, "secondSplice_p2"), secondSplice_p2.x, secondSplice_p2.y, secondSplice_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "secondSplice_p3"), secondSplice_p3.x, secondSplice_p3.y, secondSplice_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "secondSplice_p4"), secondSplice_p4.x, secondSplice_p4.y, secondSplice_p4.z);
		glUniform3f(glGetUniformLocation(shader->program, "thirdSplice_p2"), thirdSplice_p2.x, thirdSplice_p2.y, thirdSplice_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "thirdSplice_p3"), thirdSplice_p3.x, thirdSplice_p3.y, thirdSplice_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "thirdSplice_p4"), thirdSplice_p4.x, thirdSplice_p4.y, thirdSplice_p4.z);

		// First generate the walls without colisions
		int chooseRiver = 1;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		int chooseSide = 0;
		glUniform1i(glGetUniformLocation(shader->program, "chooseSide"), chooseSide);
		int middlePiece = 0;
		glUniform1i(glGetUniformLocation(shader->program, "middlePiece"), middlePiece);
		glUniform1f(glGetUniformLocation(shader->program, "start"), 0.f);
		RenderMesh(meshes["surface"], shader, glm::mat4(1), mapTextures["ground"]);

		chooseRiver = 1;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		chooseSide = 1;
		glUniform1i(glGetUniformLocation(shader->program, "chooseSide"), chooseSide);
		middlePiece = 0;
		glUniform1i(glGetUniformLocation(shader->program, "middlePiece"), middlePiece);
		glUniform1f(glGetUniformLocation(shader->program, "start"), 0.f);
		RenderMesh(meshes["surface"], shader, glm::mat4(1), mapTextures["ground"]);

		chooseRiver = 4;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		chooseSide = 1;
		glUniform1i(glGetUniformLocation(shader->program, "chooseSide"), chooseSide);
		middlePiece = 0;
		glUniform1i(glGetUniformLocation(shader->program, "middlePiece"), middlePiece);
		glUniform1f(glGetUniformLocation(shader->program, "start"), 0.f);
		RenderMesh(meshes["surface"], shader, glm::mat4(1), mapTextures["ground"]);

		chooseRiver = 2;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		chooseSide = 0;
		glUniform1i(glGetUniformLocation(shader->program, "chooseSide"), chooseSide);
		middlePiece = 0;
		glUniform1i(glGetUniformLocation(shader->program, "middlePiece"), middlePiece);
		glUniform1f(glGetUniformLocation(shader->program, "start"), 0.f);
		RenderMesh(meshes["surface"], shader, glm::mat4(1), mapTextures["ground"]);

		// Then generate the ones with colisions

		chooseRiver = 2;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		chooseSide = 1;
		glUniform1i(glGetUniformLocation(shader->program, "chooseSide"), chooseSide);
		middlePiece = 1;
		glUniform1i(glGetUniformLocation(shader->program, "middlePiece"), middlePiece);
		glUniform1f(glGetUniformLocation(shader->program, "start"), intersections.x);
		RenderMesh(meshes["surface"], shader, glm::mat4(1), mapTextures["ground"]);

		chooseRiver = 3;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		chooseSide = 0;
		glUniform1i(glGetUniformLocation(shader->program, "chooseSide"), chooseSide);
		middlePiece = 1;
		glUniform1i(glGetUniformLocation(shader->program, "middlePiece"), middlePiece);
		glUniform1f(glGetUniformLocation(shader->program, "start"), intersections.z);
		RenderMesh(meshes["surface"], shader, glm::mat4(1), mapTextures["ground"]);

		chooseRiver = 3;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		chooseSide = 1;
		glUniform1i(glGetUniformLocation(shader->program, "chooseSide"), chooseSide);
		middlePiece = 1;
		glUniform1i(glGetUniformLocation(shader->program, "middlePiece"), middlePiece);
		glUniform1f(glGetUniformLocation(shader->program, "start"), intersections.y);
		RenderMesh(meshes["surface"], shader, glm::mat4(1), mapTextures["ground"]);

		chooseRiver = 4;
		glUniform1i(glGetUniformLocation(shader->program, "chooseRiver"), chooseRiver);
		chooseSide = 0;
		glUniform1i(glGetUniformLocation(shader->program, "chooseSide"), chooseSide);
		middlePiece = 1;
		glUniform1i(glGetUniformLocation(shader->program, "middlePiece"), middlePiece);
		glUniform1f(glGetUniformLocation(shader->program, "start"), intersections.t);
		RenderMesh(meshes["surface"], shader, glm::mat4(1), mapTextures["ground"]);
	}

	// Don't show the boat if it is stopped
	if (!startBoat)
	{
		frameBuffer->BindDefault();
		// Render the frameBuffer to screen
		{
			frameBuffer->BindDefault();
			auto shader = shaders["Depth"];
			shader->Use();
			frameBuffer->BindTexture(0, GL_TEXTURE0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, frameBuffer->GetTexture(0)->GetTextureID());
			glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture_1"), 0);

			glUniform1i(shader->GetUniformLocation("texture_depth"), 1);
			frameBuffer->BindDepthTexture(GL_TEXTURE0 + 1);

			meshes["quad"]->Render();
		}
		return;
	}

	// Either the boat is in the middle and it chooses a new river or the same
	// or the boat is at the end of 1 river and it just goes back to the middle
	// currentPosition will be the first point on the bezier path and direction
	// will be the destination second point.
	if (currentPosition >= 1.f && randomRiver == 1 ||
		currentPosition <= 0.f && randomRiver != 1)
	{
		int newRiver = rand() % 4 + 1;
		if (newRiver == randomRiver)
		{
			direction = (direction == 1.f) ? 0.f : 1.f;
			boatStep = (direction == 1.f) ? +0.01f : -0.01f;
		}
		else
		{
			randomRiver = newRiver;
			currentPosition = (newRiver != 1) ? 0.f : 1.f;
			boatStep = (newRiver != 1) ? +0.01f : -0.01f;
			direction = (newRiver != 1) ? 1.f : 0.f;
			chooseNewRiver(newRiver);
		}
		currentPosition += boatStep;
	}
	else
	{
		if (currentPosition >= 0.85f && randomRiver != 1 ||
			currentPosition <= 0.05f && randomRiver == 1)
		{
			direction = (direction == 1.f) ? 0.f : 1.f;
			boatStep = (direction == 1.f) ? +0.01f : -0.01f;
		}
	}

	currentPosition += boatStep;
	nextPosition = bezier(currentPosition + boatStep, currentRiver_p1, currentRiver_p2, currentRiver_p3, currentRiver_p4);

	// Boat
	{
		Shader* shader = shaders["Boat"];
		shader->Use();

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, framePosition);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, -0.3));
		modelMatrix *= orientBoat(framePosition, nextPosition);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0.3));
		modelMatrix = glm::rotate(modelMatrix, (float)glm::radians(90.), glm::vec3(1, 0, 0));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));
		glUniform1i(shader->GetUniformLocation("part"), 2);
		meshes["oil"]->Render();

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, framePosition);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, -0.3));
		modelMatrix *= orientBoat(framePosition, nextPosition);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0.3));
		modelMatrix = glm::rotate(modelMatrix, (float)glm::radians(90.), glm::vec3(1, 0, 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.6));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));
		glUniform1i(shader->GetUniformLocation("part"), 1);
		meshes["sphere"]->Render();

		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, framePosition);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.1, -0.3));
		modelMatrix *= orientBoat(framePosition, nextPosition);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0.3));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0.5));
		modelMatrix = glm::rotate(modelMatrix, (float)glm::radians(90.), glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.6));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));
		glUniform1i(shader->GetUniformLocation("part"), 0);
		meshes["sphere"]->Render();
	}

	// Particles
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		auto shader = shaders["Particle"];

		shader->Use();
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, framePosition);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.1, -0.5));
		modelMatrix *= orientBoat(framePosition, nextPosition);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0.5));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1, 0, 0.1));
		TextureManager::GetTexture("particle.png")->BindToTextureUnit(GL_TEXTURE0);
		particleEffect->Render(GetSceneCamera(), shader, modelMatrix);

		glDisable(GL_BLEND);
	}

	// Render the frameBuffer to screen
	{
		frameBuffer->BindDefault();
		auto shader = shaders["Depth"];
		shader->Use();
		frameBuffer->BindTexture(0, GL_TEXTURE0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBuffer->GetTexture(0)->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture_1"), 0);
		glUniform1i(shader->GetUniformLocation("texture_depth"), 1);
		frameBuffer->BindDepthTexture(GL_TEXTURE1);
		glUniform1f(shader->GetUniformLocation("focalDistance"), 0.15);
		glUniform1f(shader->GetUniformLocation("focalInterval"), 0.55);

		meshes["quad"]->Render();
	}

	framePosition = nextPosition;
}

void Tema1::FrameEnd()
{
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input with window->KeyHold()
};

void Tema1::OnKeyPress(int key, int mods)
{
	// Show Wireframe
	if (key == GLFW_KEY_F1) {
		toggleWireframe = !toggleWireframe;
	}

	// Render Boat
	if (key == GLFW_KEY_SPACE) {
		startBoat = !startBoat;
	}

	// Randomize points again (this should be done at start)
	if (key == GLFW_KEY_R) {
		mainRiver_p1 = glm::vec3(-8 - static_cast<float>(rand() % 10) / 4.f + 10.f, 1.0, -7.5 + static_cast<float>(rand() % 15) / 4.f - 10.f);
		mainRiver_p2 = glm::vec3(-6.5 - static_cast<float>(rand() % 10) / 4.f + 10.f, 1.0, 7.5 + static_cast<float>(rand() % 15) / 4.f - 10.f);
		mainRiver_p3 = glm::vec3(-5.5 - static_cast<float>(rand() % 10) / 4.f + 10.f, 1.0, 10.0 + static_cast<float>(rand() % 15) / 4.f - 10.f);
		mainRiver_p4 = glm::vec3(-8.0 - static_cast<float>(rand() % 10) / 2.f + 10.f, 1.0, 15.5 + static_cast<float>(rand() % 15) / 2.f - 10.f);

		firstSplice_p2 = glm::vec3(-8 - static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 22 + static_cast<float>(rand() % 10) / 5.f - 1.f);
		firstSplice_p3 = glm::vec3(-12 - static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 16 + static_cast<float>(rand() % 10) / 5.f - 1.f);
		firstSplice_p4 = glm::vec3(-14 - static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 24 + static_cast<float>(rand() % 10) / 5.f - 1.f);

		secondSplice_p2 = glm::vec3(-4 + static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 22 + static_cast<float>(rand() % 10) / 5.f - 1.f);
		secondSplice_p3 = glm::vec3(-5 + static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 23 + static_cast<float>(rand() % 10) / 5.f - 1.f);
		secondSplice_p4 = glm::vec3(-5 + static_cast<float>(rand() % 10) / 5.f + 10.f, 1.0, 24 + static_cast<float>(rand() % 10) / 5.f - 1.f);

		thirdSplice_p2 = glm::vec3(5 + static_cast<float>(rand() % 8) / 5.f + 10.f, 1.0, 22 + static_cast<float>(rand() % 8) / 5.f - 1.f);
		thirdSplice_p3 = glm::vec3(5 + static_cast<float>(rand() % 8) / 5.f + 10.f, 1.0, 23 + static_cast<float>(rand() % 8) / 5.f - 1.f);
		thirdSplice_p4 = glm::vec3(5 + static_cast<float>(rand() % 8) / 5.f + 10.f, 1.0, 24 + static_cast<float>(rand() % 8) / 5.f - 1.f);

		currentRiver_p1 = mainRiver_p1;
		currentRiver_p2 = mainRiver_p2;
		currentRiver_p3 = mainRiver_p3;
		currentRiver_p4 = mainRiver_p4;

		framePosition = bezier(currentPosition, currentRiver_p1, currentRiver_p2, currentRiver_p3, currentRiver_p4);
		intersections = generateStartOfIntersections();
	}
};

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Tema1::OnWindowResize(int width, int height)
{
	// treat window resize event
}
