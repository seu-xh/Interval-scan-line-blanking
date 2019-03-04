#include<gl\glut.h>
#include"structure.h"
#include"frame_buffer.h"
#include"obj.h"
#include<vector>
#include<iostream>
#include"glm\glm.hpp"
#include<algorithm>
#include<stdlib.h>
#include<time.h>

int index;//选择载入模型的编号
std::vector<std::string>path{ "model/bunny.obj" ,"model/dolphins.obj" ,"model/gull.obj", "model/humbird.obj" };
std::string word{ "请选择一个模型:1.兔子 2.海豚 3.海鸥 4.小鸟" };
std::string ModelPath;

float angle=0;
float StartTime, LoadModelTime;
std::vector<Polygon*>tPolygons;//多边形表，负责多边形的创建、释放
std::vector<Edge*>tEdges;//边表，tEdge[i]为所有Ymax=y的边组成的链表的头指针
std::vector<Polygon*>IPL;//活化多边形表，负责记录与每个小区间相关的多边形
std::vector<Edge*>AET;//活化边表，记录当前扫描线与哪些边相交
FrameBuffer framebuffer;
Obj obj;
glm::vec3 bg_color(0, 0, 0);
int Width=800, Height=600;
void cleartEdges() {
	Edge* tmp;
	for (Edge* &p : tEdges) {
		while (p) {
			tmp = p->next;
			free(p);
			p = tmp;
		}
		p = NULL;
	}
}
void cleartPolygons() {
//	Polygon *tmp;
	for (int i = 0; i < tPolygons.size(); i++) {
		free(tPolygons[i]);
	}
}
void clearAll() {
	cleartEdges();
	cleartPolygons();
	tPolygons.clear();
	AET.clear();
	IPL.clear();
}
void init() {
	
	obj.win_height = Height;
	obj.win_width = Width;
	obj.loadObj(ModelPath);
	obj.translate();
	obj.rotateObj(0.5,glm::vec3(0, 1, 0));
	std::cout << "顶点数：" << obj.nvetex << std::endl;
	std::cout << "面片数：" << obj.nface << std::endl;
	//obj.rotateObj(1, glm::vec3(0, 1, 0));

	//obj.rotateObj(0.3, glm::vec3(1, 0, 0));

	framebuffer.Resize(Width, Height);
	framebuffer.Initbg(bg_color);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, Width, 0, Height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//zbuffer.clear();
	clearAll();
	tEdges.clear();
	tEdges.assign(Height,NULL);
}
void SolveFace_abcd(std::vector<GLdouble>& abcd, std::vector<glm::vec3>face)
{
	glm::vec3 v1, v2,v3;
	v1 = face[1] - face[0];
	v2 = face[2] - face[1];
	v3 = glm::cross(v1, v2);
	v3 = glm::normalize(v3);
	abcd[0] = v3.x;
	abcd[1] = v3.y;
	abcd[2] = v3.z;
	abcd[3] = -(glm::dot(v3, face[0]));
}

Point roundVertex(glm::vec3 v) {
	Point p;
	p.x = v.x + ((v.x < 0) ? -0.5 : 0.5);
	p.y = v.y + ((v.y < 0) ? -0.5 : 0.5);
	p.z = v.z;
	return p;
}
int PolygonId = 0;
glm::vec3 calculateColor(glm::vec3 normal) {
	//glm::vec3 ambient(0.3, 0.3, 0.3);
	glm::vec3 color(1.0, 1.0, 1.0);
	float t = glm::dot(normal, glm::vec3(0, 0, 1));
	if (t < 0)t = -t;
	//t = (t + 1) / 2;
	glm::vec3 result;
	result = color * t;
	//if (result.r > 1.0) result = glm::vec3(1.0, 1.0, 1.0);
	return result;
}
void processFace(std::vector<std::vector<glm::vec3>>&faces)
{
	for (int i = 0; i < faces.size(); i++)
	{
		PolygonId++;
		std::vector<GLdouble>abcd(4);
		SolveFace_abcd(abcd, faces[i]);
		if (abcd[2]<1e-8&&abcd[2]>-1e-8) continue;//垂直xoy
		if (abcd[2] < 0) continue;//面朝向-z轴消除
		glm::vec3 normal(abcd[0], abcd[1], abcd[2]);
		glm::vec3 color = calculateColor(normal);
		Polygon* tpolygon = new  Polygon(abcd[0], abcd[1], abcd[2], abcd[3], PolygonId, color);
		tPolygons.push_back(tpolygon);
		for (int j = 0; j < faces[i].size(); j++) {
			Point a = roundVertex(faces[i][j]);
			Point b = roundVertex(faces[i][(j + 1) % (faces[i].size())]);
			if (a.y < b.y)
				std::swap(a, b);
			if (a.y < 0) continue;
			if (a.y - b.y<1e-8&&a.y - b.y>-1e-8) continue;
			//if(b.y<0) b.y=0;
			Edge* tmp = new Edge(a.x, (b.x - a.x) / (a.y - b.y), a.y - b.y + 1, PolygonId,tpolygon);
			//std::cout << a.y << std::endl;
			if (tEdges[a.y] == NULL)
				tEdges[a.y] = tmp;
			else {
				Edge* head(tEdges[a.y]);
				while (head->next)
					head = head->next;
				head->next = tmp;
			}
		}
		
	}
}



void ActivateEdge(GLint y) {
	if (y<0 || y>=Height) return;//y超出范围
	if (tEdges[y]) {
		Edge* head(tEdges[y]);
		while (head) {
			AET.push_back(head);
			head = head->next;
		
		}
	}
}

void UpdateAET(GLint y) {
	Edge* tmp;
	for (int i = 0; i < AET.size();i++) {
		tmp = AET[i];
		tmp->dy--;
		if(tmp->dy>0) tmp->x += tmp->dx;
		else{
			AET.erase(AET.begin() + i);
			i--;
		}
	}
}

GLfloat computeZ(GLfloat midx,GLint y, Polygon* ap) {
	GLfloat z = (-ap->d - ap->a*midx - ap->b*y) / ap->c;
	return z;
}
bool cmp(Edge*a, Edge*b) {
	return a->x < b->x;
}
void IPLerase(Polygon* p) {
	for(int i=0;i<IPL.size();i++)
		if (IPL[i] == p) {
			IPL.erase(IPL.begin() + i);
			//return;
			i--;
		}
}
void ScanLine() {
	for (int y = Height - 1; y >= 0; y--) {
		//for (int i = 0; i < tPolygons.size(); i++)
			//tPolygons[i]->inflag = false;
			//	std::cout << "inflag!=false" << std::endl;
		IPL.clear();
		ActivateEdge(y);
		UpdateAET(y);
		std::sort(AET.begin(), AET.end(), cmp);
		Edge *tmp;
		GLfloat prex=0,midx;
		for (int i = 0; i < (int)AET.size(); i++) {
			tmp = AET[i];
			
			//std::cout << tmp->id << " ";
			//if (tmp->id == 4) 
			//	std::cout << tmp->x << " "<<" y: "<<y;

			midx = (tmp->x + prex) / 2;
			GLfloat maxz = -999999,z;
			glm::vec3 color;
			if (IPL.size() == 0) color=bg_color;
			if (IPL.size() == 1) color = IPL[0]->color;
			if(IPL.size()>1) {
				for (int j = 0; j < IPL.size(); j++)
				{
					z = computeZ(midx, y, IPL[j]);
					if (z > maxz) {
						maxz = z;
						color = IPL[j]->color;
					}
				}
			}
			for (int j = prex; j < tmp->x; j++) {
				framebuffer.DrawPixel(j, y, color);
				//std::cout << j << " " << y << " " <<framebuffer.GetPixel(j,y).r<< std::endl;
			}
			bool flag = tmp->pol->inflag;
			flag = !flag;
			tmp->pol->inflag = flag;
			if (flag == true)
				IPL.push_back(tmp->pol);
			else IPLerase(tmp->pol);
			prex = tmp->x;

		}

	}
	
}
void render() {
	glBegin(GL_POINTS);
	for (int i = 0; i<Width; i++)
		for (int j = 0; j < Height; j++)
		{
			glm::vec3 color = framebuffer.GetPixel(i, j);
			glColor3f(color.r, color.g, color.b);
			glVertex2d(i, j);

		}
	glEnd();
}
void display() {
	//obj.rotateObj(angle, glm::vec3(0, 1, 0));
	processFace(obj.VFaces);
	ScanLine();
	glClear(GL_COLOR_BUFFER_BIT);
	render();
	std::cout << "绘制时间：" << (clock() - LoadModelTime) / CLK_TCK << "s" << std::endl;
	std::cout << "总共耗时：" << (clock() - StartTime) / CLK_TCK << "s" << std::endl;
	glFlush();

}
int main(int argc, char *argv[]) {
	    std::cout << word << std::endl;
		std::cin >> index;
		while (index > path.size() || index <= 0) {
			std::cout << "输入无效！请重新输入！"<<std::endl;
			std::cin >> index;
		}
		ModelPath = path[index - 1];
	    StartTime = clock();
	    LoadModelTime = 0;
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
		glutInitWindowPosition(0, 0);
		glutInitWindowSize(Width, Height);
		glutCreateWindow("scanline");

		init();
		LoadModelTime = clock();
		std::cout << "加载模型时间：" << (LoadModelTime -StartTime) / CLK_TCK <<"s"<< std::endl;
		glutDisplayFunc(display);
		//glutReshapeFunc(Reshape);

		//FinishTime = clock();
		//std::cout << "总共耗时：" << (FinishTime - StartTime) / CLK_TCK << "s" << std::endl;
		//glutMouseFunc(mouse);
		glutMainLoop();
	return 0;
}

