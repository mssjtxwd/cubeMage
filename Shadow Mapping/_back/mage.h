#ifndef _MAGE_H__
#define _MAGE_H__
#include "global.h"
#include "stdafx.h"

class Wall
{
private:
	GLuint tex_walls;
public:
	typedef enum { X,Y,Z
	} TYPE;
	static const float THICKNESS;
	glm::vec3 p1,p2;
	TYPE type;
	void draw();
	//void wallFac(char type,glm::vec2 v1,glm::vec2 v2,GLfloat plane,GLfloat thickness = THICKNESS);
	static Wall wallFac(WCHAR type,glm::vec2 v1,glm::vec2 v2,GLfloat plane,GLfloat thickness = THICKNESS);
	void Log();
	void drawTexture();
	void setTexture(GLuint tex);
	bool includ(glm::vec3 pos);
};

class Maze{
public:
	Maze();
	int xPlane[CELLNUM][CELLNUM][CELLNUM];
	int yPlane[CELLNUM][CELLNUM][CELLNUM];
	int zPlane[CELLNUM][CELLNUM][CELLNUM];
	int n;
	GLfloat cellSize,size,mazeL,mazeR;
	vector<Wall> walls;
	glm::vec3 getIdx(GLfloat x,GLfloat y,GLfloat z);
	glm::vec3 getIdx(glm::vec3 pos);
	void draw();
	void setTexture(GLuint tex);
	void setTexture(vector<GLuint> &tex_list);
	void drawTexture();

	bool g;
	bool check(glm::mat4& mat_view);
	bool checkCrack(glm::vec3 pos);
};

#endif