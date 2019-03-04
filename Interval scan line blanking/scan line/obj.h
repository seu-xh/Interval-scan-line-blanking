#pragma once
#include<GL/glut.h>
#include<vector>
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include"glm\glm.hpp"
#include"glm\gtc\matrix_transform.hpp"
#include"glm\gtc\type_ptr.hpp"
class Obj
{
public:
	Obj() {
		offset =glm::vec2(0.0, 0.0);
		scale = 1.0;
	};
	//~Obj();
	void loadObj(std::string filepath) {
		std::ifstream file(filepath);
		std::string line;
		glm::vec3 point;
		std::vector<GLint>face;
		//std::vector<glm::vec3>f;
		GLdouble MinX(999999), MaxX(-999999), MinY(999999), MaxY(-999999);
		while (file>>line) {
			if (line == "v") {
				{
					file>> point.x >> point.y >> point.z;
					if (point.x < MinX) MinX = point.x;
					if (point.x > MaxX) MaxX = point.x;
					if (point.y < MinY) MinY = point.y;
					if (point.y > MaxY) MaxY = point.y;
					Vertex.push_back(point);
				}
			}
			else if (line=="f") {
				int vIndex,nIndex,tIndex;
				face.clear();
				while (true)
				{
					char ch = file.get();
					if (ch == ' ') continue;
					else if (ch == '\n' || ch == EOF) break;
					else file.putback(ch);
					file >> vIndex;
					char splitter = file.get();

					if (splitter == '/')
					{
						splitter = file.get();
						if (splitter == '/')
						{
							file >> nIndex;
						}
						else
						{
							file.putback(splitter);
							file >> tIndex;
							splitter = file.get();
							if (splitter == '/')
							{
								file >> nIndex;
							}
							else file.putback(splitter);
						}
					}
					else file.putback(splitter);
					face.push_back(vIndex-1);
				}
				IFaces.push_back(face);
			}
		}
		GLdouble spanx = MaxX - MinX, spany = MaxY - MinY;
		GLdouble scalex = win_width / 4 / spanx, scaley = win_height / 4 / spany;
		scale = scalex>scaley?scalex:scaley;
		offset.x = win_width / 2 - (MaxX + MinX) / 2 * scale;
		offset.y = win_height / 2 - (MaxY + MinY) / 2 * scale;

		nvetex = Vertex.size();
		nface = IFaces.size();
	};
	Obj& translate(glm::vec3 off=glm::vec3(0,0,0)) {
		glm::vec3 tran(offset.x, offset.y, 0);
		tran = tran + off;
		glm::mat4 model(1.0f);

		model = glm::translate(model, tran);

		model = glm::scale(model, glm::vec3(scale, scale, scale));

		

		std::vector<glm::vec3>V(Vertex);
		glm::vec4 tmp;
		for (int i = 0; i < Vertex.size(); i++) {
			tmp = model * glm::vec4(Vertex[i], 1.0);
			V[i] = glm::vec3(tmp.x, tmp.y, tmp.z);
		}

		//VFaces.resize(nface);
		VFaces.clear();
		std::vector<glm::vec3>vface;
		for (int i = 0; i < IFaces.size(); i++) {
			for (int j = 0; j < IFaces[i].size(); j++) {
				vface.push_back(V[IFaces[i][j]]);
			}
			VFaces.push_back(vface);
			vface.clear();
		}
		V.clear();
		return *this;

	}
	Obj& rotateObj(GLdouble angle, glm::vec3 axis) {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(offset, 0));
		model = glm::rotate(model, (GLfloat)angle, axis);
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		std::vector<glm::vec3>V(Vertex);
		glm::vec4 tmp;
		for (int i = 0; i < V.size(); i++) {
			tmp = model * glm::vec4(V[i], 1.0);
			V[i] = glm::vec3(tmp.x, tmp.y, tmp.z);
		}
		VFaces.clear();
		std::vector<glm::vec3>vface;
		for (int i = 0; i < IFaces.size(); i++) {
			for (int j = 0; j < IFaces[i].size(); j++) {
				vface.push_back(V[IFaces[i][j]]);
			}
			VFaces.push_back(vface);
			vface.clear();
		}
		V.clear();
		return *this;
	}
	std::vector<glm::vec3>Vertex;
	std::vector<std::vector<GLint>>IFaces;
	std::vector<std::vector<glm::vec3>>VFaces;
	GLint nvetex;
	GLint nface;
	glm::vec2 offset;
	GLdouble scale;
	GLint win_width, win_height;

};

