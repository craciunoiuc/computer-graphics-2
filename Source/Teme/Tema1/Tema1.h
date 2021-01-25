#pragma once

#include <Component/SimpleScene.h>

class Tema1 : public SimpleScene
{
	public:
		Tema1();
		~Tema1();

		void Init() override;

	private:
		struct Particle
		{
			glm::vec4 position;
			glm::vec4 speed;
			glm::vec4 initialPos;
			glm::vec4 initialSpeed;

			Particle() {};

			Particle(const glm::vec4& pos, const glm::vec4& speed)
			{
				SetInitial(pos, speed);
			}

			void SetInitial(const glm::vec4& pos, const glm::vec4& speed)
			{
				position = pos;
				initialPos = pos;

				this->speed = speed;
				initialSpeed = speed;
			}
		};
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;
		void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1);
		unsigned int Tema1::UploadCubeMapTexture(const std::string& posx, const std::string& posy, const std::string& posz, const std::string& negx, const std::string& negy, const std::string& negz);
		void chooseNewRiver(int newRiver);
		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		// Orients the boat to face the direction it is going
		static inline glm::mat4 orientBoat(glm::vec3 point1, glm::vec3 point2)
		{
			glm::vec3 d = glm::normalize(point2 - point1);
			return glm::transpose(glm::mat4(d.z, 0, d.x, 0,
				0, 1, 0, 0,
				-d.x, 0, d.z, 0,
				0, 0, 0, 1));
		}

		// Generates the points on the curve
		static inline glm::vec3 bezier(float t, glm::vec3 control_p1, glm::vec3 control_p2, glm::vec3 control_p3, glm::vec3 control_p4)
		{
			return control_p1 * pow((1 - t), 3) + control_p2 * glm::vec3(3, 3, 3) * t * pow((1 - t), 2) + control_p3 * glm::vec3(3, 3, 3) * pow(t, 2) * (1 - t) + control_p4 * pow(t, 3);
		}

		// Generates the intersections between the rivers by checking each point on the bezier curves if it intersects with all the points
		// on the other curve.
		static float calculateIntersection(glm::vec3 firstRiver_p1, glm::vec3 firstRiver_p2, glm::vec3 firstRiver_p3, glm::vec3 firstRiver_p4,
											glm::vec3 secondRiver_p1, glm::vec3 secondRiver_p2, glm::vec3 secondRiver_p3, glm::vec3 secondRiver_p4)
		{
			glm::vec3 point1_s, point2_s;
			glm::vec3 point1_e, point2_e;
			float curve_step = 0.001f;
			for (float i = 0.f; i <= 1.f; i += curve_step)
			{
				point1_s = bezier(i, firstRiver_p1, firstRiver_p2, firstRiver_p3, firstRiver_p4) + glm::vec3(1.6, 0, 0);
				point1_e = bezier(i + curve_step, firstRiver_p1, firstRiver_p2, firstRiver_p3, firstRiver_p4) + glm::vec3(1.6, 0, 0);
				for (float j = 0.f; j <= 1.f; j += curve_step)
				{
					point2_s = bezier(j, secondRiver_p1, secondRiver_p2, secondRiver_p3, secondRiver_p4) + glm::vec3(-1.6, 0, 0);
					point2_e = bezier(j + curve_step, secondRiver_p1, secondRiver_p2, secondRiver_p3, secondRiver_p4) + glm::vec3(-1.6, 0, 0);
					glm::vec3 r = point1_e - point1_s;
					glm::vec3 s = point2_e - point2_s;
					glm::vec3 difference = (point2_s - point1_s);
					float divider = r.x * s.z - r.z * s.x;
					float t = (difference.x * s.z - difference.z * s.x) / divider;
					float u = (difference.x * r.z - difference.z * r.x) / divider;
					glm::vec3 result = abs(point1_s + t * r - point2_s + u * s);
					if (result.x < 0.01f && result.z < 0.01f) {
						return j;
					}
				}
			}
			return 0.f;
		}

		// Called only once when the points are generated
		glm::vec4 generateStartOfIntersections()
		{
			return glm::vec4(calculateIntersection(mainRiver_p4, firstSplice_p2, firstSplice_p3, firstSplice_p4, mainRiver_p4, secondSplice_p2, secondSplice_p3, secondSplice_p4),
							 calculateIntersection(mainRiver_p4, secondSplice_p2, secondSplice_p3, secondSplice_p4, mainRiver_p4, thirdSplice_p2, thirdSplice_p3, thirdSplice_p4),
							 calculateIntersection(mainRiver_p4, firstSplice_p2, firstSplice_p3, firstSplice_p4, mainRiver_p4, secondSplice_p2, secondSplice_p3, secondSplice_p4),
							 calculateIntersection(mainRiver_p4, secondSplice_p2, secondSplice_p3, secondSplice_p4, mainRiver_p4, thirdSplice_p2, thirdSplice_p3, thirdSplice_p4));
		}

	private:
		int cubeMapTextureID;
		bool toggleWireframe = false, startBoat = false;
		// Can be 1 2 3 4
		int randomRiver = 1;
		// Can be either 1.f or 0.f
		float direction = 1.f;
		 // The start position and one step on the line
		float currentPosition = 0.2f, boatStep = 0.01f;
		unsigned int nrParticles;
		// All 4 intersections are saved here
		glm::vec4 intersections;
		// The points that generate the curve
		glm::vec3 currentRiver_p1, currentRiver_p2, currentRiver_p3, currentRiver_p4;
		glm::vec3 mainRiver_p1, mainRiver_p2, mainRiver_p3, mainRiver_p4;
		glm::vec3 firstSplice_p2, firstSplice_p3, firstSplice_p4;
		glm::vec3 secondSplice_p2, secondSplice_p3, secondSplice_p4;
		glm::vec3 thirdSplice_p2, thirdSplice_p3, thirdSplice_p4;
		// Positions
		glm::vec3 framePosition, nextPosition;
		// Holds textures
		std::unordered_map<std::string, Texture2D*> mapTextures;
		// Holds the particles
		ParticleEffect<Tema1::Particle>* particleEffect;
		// Holds the rendered frame before applying post-processing effects
		FrameBuffer* frameBuffer;
};
