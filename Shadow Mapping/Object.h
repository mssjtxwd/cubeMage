#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "tga.h"
#include "stdafx.h"
#include <iostream>
using namespace std;
struct Vertex
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
struct VT
{
	GLfloat  u;
	GLfloat  v;
};
struct VN
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
struct Face
{
	int a;
	int b;
	int c;
};
struct FT
{
	int a;
	int b;
	int c;
};
struct FN
{
	int a;
	int b;
	int c;
};
struct material{
	string name;
	GLfloat Ka[4];
	GLfloat Kd[4];
	GLfloat Ks[4];
	GLfloat Ns;
	string texture;
	TextureImage * texImg;
};
struct S{
	int num_f;
	string usema;
	vector<Face> face;
	vector<FT> ft;
	vector<FN> fn;
};
class Object
{
public:
	void draw(GLfloat x,GLfloat y,GLfloat z,GLfloat s);
	int readfile(string str);
	int readMaterial();
	Object();
	Object(string str);
private:
	string materialset;
	int num_v;
	int num_vn;
	int num_vt;
	int num_s;
	int num_ma;
	vector<Vertex> vertex;
	vector<VN> vn;
	vector<VT> vt;
	vector<S> sset;

	map<string,material> masMap;
};

#endif