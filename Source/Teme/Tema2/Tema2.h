#pragma once

#include <Component/SimpleScene.h>

// Only one at once
#define USE_RGB 0
#define USE_HSV 1
#define USE_YCbCr 0

class Tema2 : public SimpleScene
{
	public:
		Tema2();
		~Tema2();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		glm::vec3 RGBtoHSV(glm::vec3 pixel);
		glm::vec3 RGBtoYCbCr(glm::vec3 pixel);
		void Segment(int N1, int M1, int N2, int M2, glm::vec3 T, int* N);
		bool Omogen(int N1, int M1, int N2, int M2, glm::vec3 T);
		inline float Tema2::myMax(float a, float b);
		inline float Tema2::myMin(float a, float b);

	private:
		Texture2D *originalImage, *processedImage;
		bool debug_transforms = true;
		unsigned int channels;
		unsigned char* data;
		unsigned char* newData;
		glm::ivec2 imageSize;

};
