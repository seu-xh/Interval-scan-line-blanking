#pragma once
#include<vector>
#include"glm\glm.hpp"
class FrameBuffer
{
public:
	FrameBuffer() :Width(0), Height(0) {};
	//~FrameBuffer();
	void Resize(int width, int height) {
		Width = width;
		Height = height;
		Buffer.clear();
		Buffer.resize(Width*Height);
	}
	void Initbg(glm::vec3 color) {
		for (int i = 0; i < Buffer.size(); i++)
			Buffer[i] = color;
	}

	void DrawPixel(int x, int y, glm::vec3 color) {
		if (x<0 || x>=Width || y<0 || y>=Height) return;
		Buffer[Width*y + x] = color;

	}

	glm::vec3 GetPixel(int x, int y) {
		 glm::vec3 color(0.0f,0.0f,0.0f);
		 if (x<0 || x>=Width || y<0 || y>=Height) return color;
		 color = Buffer[Width*y + x];
		 return color;
	}

	int GetWidth() {
		return this->Width;
	}
	int GetHeight() {
		return this->Height;
	}
//private:
	std::vector<glm::vec3>Buffer;
	int Width, Height;
};

