#pragma once
#include<GL/glut.h>
//#include"glew glfw soil\include\GL\glew.h"
#include"glm\glm.hpp"

struct  Polygon;
struct Edge;
struct ActiveEdge;

typedef struct Polygon//¶à±ßÐÎ
{
	GLdouble a, b, c, d;
	GLint id;
	//GLint dy;
	glm::vec3 color;
	Polygon* next;
	bool inflag;
	Polygon() {};
	Polygon(GLdouble a, GLdouble b, GLdouble c, GLdouble d, GLint id, glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f), Polygon* next = NULL,bool inflag=false) :
		a(a), b(b), c(c), d(d), id(id), color(color), next(next),inflag(inflag){};
}Polygon;



typedef struct Edge//±ß
{
	GLdouble x;
	GLdouble dx;
	GLint dy;
	//GLint ymax;
	GLint id;
	Edge* next;
	Polygon* pol;
	//bool used;
	Edge() {};
	Edge(GLdouble x, GLdouble dx, GLint dy, GLint id,Polygon* pol=NULL,Edge *next = NULL)
		:x(x), dx(dx), dy(dy), id(id), pol(pol),next(next) {};
}Edge;



struct Point
{
	GLint x, y;
	GLdouble z;
};