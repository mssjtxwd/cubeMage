#include <gl/freeglut.h> 
#include<math.h>
#include"gl/GLAUX.H"
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<random>
#include<ctime>
#include<iostream>
#include"global.h"
#include<Windows.h>
using namespace std;
#pragma comment ( lib, "glaux.lib" )

#define WINDOW_W 800
#define WINDOW_H 800

float current_x=MAZE_LX + MAZE_XRANGE / 2 + MAZE_XRANGE / N / 2;
float current_y=HEIGHT/2;
float current_z=MAZE_LZ + MAZE_ZRANGE / 2 + MAZE_ZRANGE / M / 2 ;
float mRotate = 0;
float mElevaAngle = 0;
float lookAtX = 0,lookAtY = 0,lookAtZ = 0;
float lookAtUpX = 0,lookAtUpY = 1.0,lookAtUpZ = 0;
AUX_RGBImageRec *Image[4];
GLuint Texture[4];
bool moved = true;
class Maze{
public:
	int xPlane[MAZESIZE][MAZESIZE][MAZESIZE];
	int yPlane[MAZESIZE][MAZESIZE][MAZESIZE];
	int zPlane[MAZESIZE][MAZESIZE][MAZESIZE];
} maze;
bool mouseRightButtonDown = false;
class Wall
{
public:
	Wall(GLfloat _x1,GLfloat _y1,GLfloat _z1,GLfloat _x2,GLfloat _y2,GLfloat _z2,GLfloat _style = 0):
	x1(_x1),z1(_z1),x2(_x2),z2(_z2),y1(_y1),y2(_y2),style(_style){
		//style = 0;
	};
	GLfloat x1,y1,z1,x2,y2,z2;
	int style;

};
vector<Wall> walls;
struct Point {
	int x,y;
};

bool loadTexture()
{
	FILE *file;
	if(!(file=fopen("wall2.bmp","r")))
		return false;
	Image[0]=auxDIBImageLoad("wall2.bmp");
	glGenTextures(1,&Texture[0]);
	glBindTexture(GL_TEXTURE_2D,Texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,3,Image[0]->sizeX,Image[0]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,Image[0]->data);

	if(!(file=fopen("floor2.bmp","r")))
		return false;
	Image[1]=auxDIBImageLoad("floor2.bmp");
	glGenTextures(1,&Texture[1]);
	glBindTexture(GL_TEXTURE_2D,Texture[1]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,3,Image[1]->sizeX,Image[1]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,Image[1]->data);

	if(!(file=fopen("roof2.bmp","r")))
		return false;
	Image[2]=auxDIBImageLoad("roof2.bmp");
	glGenTextures(1,&Texture[2]);
	glBindTexture(GL_TEXTURE_2D,Texture[2]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,3,Image[2]->sizeX,Image[2]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,Image[2]->data);

	for(int i=0;i<3;i++)
	{
		if(Image[i])
		{
			if(Image[i]->data)
				free(Image[i]->data);
			free(Image[i]);
		}
	}
	glEnable(GL_TEXTURE_2D); 
	return true;
}
void addWall();
void init()
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f); 
	glColor3f(1.0,1.0,1.0); 
	glEnable(GL_DEPTH_TEST);
	loadTexture();
	addWall();
}

void makewall(GLfloat a[3],GLfloat b[3],GLfloat c[3],GLfloat d[3],int style)
{
	//GLfloat diffuse[] = {1.0, 1.0, 1.0};
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
	glBindTexture(GL_TEXTURE_2D,Texture[style]);
	//if (style == 2) cout<<"!!!!!!!"<<endl;
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f); glVertex3fv(a);
		glTexCoord2f(1.0f, 0.0f); glVertex3fv(b);
		glTexCoord2f(1.0f, 1.0f); glVertex3fv(c);
		glTexCoord2f(0.0f, 1.0f); glVertex3fv(d);
	glEnd();
}

void addWall()
{
	int n = N;
	float zs = MAZE_ZRANGE / N,xs = MAZE_XRANGE / N,ys = MAZE_YRANGE / N;
	for (int i=0;i<=n;i++)
		for (int j=0;j<n;j++)
				for (int k=0;k<n;k++)
				{
					if (maze.xPlane[i][j][k]){
						walls.push_back(Wall(MAZE_LX + i * xs,MAZE_LY + j * ys,MAZE_LZ + k * zs,MAZE_LX + i * xs,MAZE_LY + j * ys + ys,
											MAZE_LZ + k * zs + zs,0));
					}
					if (maze.yPlane[i][j][k]){
						//cout<<"!!!!!"<<endl;
						walls.push_back(Wall(MAZE_LX + j * xs,MAZE_LY + i * ys,MAZE_LZ + k * zs,MAZE_LX + j * xs + xs,MAZE_LY + i * ys,
											MAZE_LZ + k * zs + zs,1));
					}
					if (maze.zPlane[i][j][k]){
						//cout<<"!"<<endl;
						walls.push_back(Wall(MAZE_LX + j * xs,MAZE_LY + k * ys,MAZE_LZ + i * zs,MAZE_LX + j * xs + xs,MAZE_LY + k * ys + ys,
											MAZE_LZ + i * zs,2));
					}
				}
	std::cout<<walls.size()<<endl;
}

void drawWalls()
{
	for(size_t i=0;i<walls.size();i++)
	{
		
		if (walls[i].style == 1){
			GLfloat a[3]={walls[i].x1,walls[i].y1,walls[i].z1};
			GLfloat b[3]={walls[i].x1,walls[i].y2,walls[i].z2};
			GLfloat c[3]={walls[i].x2,walls[i].y2,walls[i].z2};
			GLfloat d[3]={walls[i].x2,walls[i].y2,walls[i].z1};
			makewall(a,b,c,d,walls[i].style);
		}
		else {
			GLfloat a[3]={walls[i].x1,walls[i].y1,walls[i].z1};
			GLfloat b[3]={walls[i].x2,walls[i].y1,walls[i].z2};
			GLfloat c[3]={walls[i].x2,walls[i].y2,walls[i].z2};
			GLfloat d[3]={walls[i].x1,walls[i].y2,walls[i].z1};
			makewall(a,b,c,d,walls[i].style);
		}
	}
}

void display()
{
	//Sleep(50);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	const float RANGE = 10;
	float h = RANGE*sinf(mElevaAngle * PI / 180.0f);
	float w = RANGE*cosf(mElevaAngle * PI / 180.0f);

	gluLookAt(current_x, current_y, current_z,
			current_x+w*sinf(mRotate * PI /180.0), current_y + h, current_z+w*cosf(mRotate*PI/180.0),
			lookAtUpX,lookAtUpY,lookAtUpZ);
	drawWalls();
	glutSwapBuffers();
}
const int ITERNUM = 10;
void TimerFunction(int value)
{
	if (value == 0) return;
	int i = 1 + ITERNUM - value;
	float theta = (i+0.) * PI / ITERNUM;
	static int flag = 1;
	if (value == ITERNUM)
		flag = lookAtUpY > 0 ? 1 : -1;
	lookAtUpX = flag * sinf(theta);
	lookAtUpY = flag * (cosf(theta));
	glutPostRedisplay();
	glutTimerFunc(30, TimerFunction, value - 1);
}
bool check(int x,int y,int z) {
	int i = (x - MAZE_LX) / (MAZE_XRANGE / N);
	int j = (y - MAZE_LY) / (MAZE_YRANGE / N);
	int k = (z - MAZE_LZ) / (MAZE_ZRANGE / N);
	const int EPS = 1;
	if (maze.xPlane[i][j][k] && (x - int(MAZE_LX)) % (int(MAZE_XRANGE) / N) <= EPS) return false;
	if (maze.xPlane[i+1][j][k] && (x - int(MAZE_LX)) % (int(MAZE_XRANGE) / N) >= MAZE_XRANGE / N - EPS) return false;
	if (maze.yPlane[j][i][k] && (y - int(MAZE_LY)) % (int(MAZE_YRANGE) / N) <= EPS) return false;
	if (maze.yPlane[j+1][i][k] && (y - int(MAZE_LY)) % (int(MAZE_YRANGE) / N) >= MAZE_YRANGE / N - EPS) return false;
	if (maze.zPlane[k][i][j] && (z - int(MAZE_LZ)) % (int(MAZE_ZRANGE) / N) <= EPS) return false;
	if (maze.zPlane[k+1][i][j] && (z - int(MAZE_LZ)) % (int(MAZE_ZRANGE) / N) >= MAZE_ZRANGE / N - EPS) return false;
	return true;
}
void keyboard(unsigned char key,int x,int y)
{
	float old_x = current_x;
	float old_z = current_z;
	const float RANGE = 1;
	float h = RANGE*sinf(mElevaAngle * PI / 180.0f);
	float w = RANGE;//RANGE*cosf(mElevaAngle * PI / 180.0f);
	float v = 2;
	if( key == 'w'   )  {
		current_x += v*w*sinf( mRotate * PI / 180.0f );
		current_z += v*w*cosf( mRotate * PI / 180.0f );
		if (!check(current_x,current_y,current_z)) {
			cout<<"not right"<<endl;
			current_x = old_x;
			current_z = old_z;
		}
		//current_y += v*h;

	}
	else if( key == 's' )  {
		current_x -= v / 2 * w*sinf( mRotate * PI / 180.0f );
		current_z -= v / 2 * w*cosf( mRotate * PI / 180.0f );
		if (!check(current_x,current_y,current_z)) {
			cout<<"not right"<<endl;
			current_x = old_x;
			current_z = old_z;
		}
		//current_y -= v / 2 * h;
	}
	else if( key == 'a' )  {
		mRotate += 5;
		if(mRotate>360) mRotate-360;
	}
	else if( key == 'd' )  {
		mRotate -= 5;
		if(mRotate>360) mRotate-360;
	}
	else if (key == 'z') {
		mElevaAngle += 2;
	}
	else if (key == 'x') {
		mElevaAngle -= 2;
	}
	else if (key == 'c') {
		cout<<"c is pushed"<<endl;
		glutTimerFunc(30, TimerFunction, ITERNUM);
	}
	glutPostRedisplay();
}
void placeControl(int key,int x,int y)
{

}
Point oldRightMouse = {-1,-1};
void mouseClickEvtListen(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseRightButtonDown = true;
			oldRightMouse.x = x;
			oldRightMouse.y = y;
			cout<<"right mouse is clicked; mouseRightButtonDown = true now"<<endl;
		}
		else {
			mouseRightButtonDown = false;
			oldRightMouse.x = -1;
			oldRightMouse.y = -1;
			cout<<"right mouse is released; mouseRightButtonDown = false now"<<endl;
		}
	}
}

void mouseMoveEvtListen(int x,int y) {
	if (mouseRightButtonDown == true) {
		if (oldRightMouse.x == -1 && oldRightMouse.y == -1) {
			//nothing
		}
		else {
			cout<<"right mouse has been clicked; and oldRightMouse are not -1 now"<<endl;
			float dx = x - oldRightMouse.x;
			float dy = y - oldRightMouse.y;
			cout<<"old_x = "<<oldRightMouse.x<<" old_y = "<<oldRightMouse.y<<endl;
			cout<<"x = "<<x<<" y = "<<y<<endl;
			cout<<"dx = "<<dx<<" dy = "<<dy<<endl;

			float flag = lookAtUpY > 0 ? -1 : 1;

			mRotate -= flag * dx / (WINDOW_W) * 90; 

			if (mRotate > 360) mRotate -=360;
			else if (mRotate < -360) mRotate+=360;

			mElevaAngle -= flag * dy / (WINDOW_H) * 90; 

			if (mElevaAngle > 360) mElevaAngle -=360;
			else if (mElevaAngle < -360) mElevaAngle+=360;

			oldRightMouse.x = x;
			oldRightMouse.y = y;
		}
		
	}
	glutPostRedisplay();
}
void makeMaze(const int n)
{
	srand(time(NULL));
	//for (int i=0;i<100;i++) cout<<rand()<<endl;
	//random();
	int ans = 0;
	for (int i=0;i<=n;i++)
		for (int j=0;j<n;j++)
				for (int k=0;k<n;k++)
				{
					if (i==0 || i==n){
						maze.xPlane[i][j][k] = 1;
						maze.yPlane[i][j][k] = 1;
						maze.zPlane[i][j][k] = 1;
					}
					else {
						maze.xPlane[i][j][k] = rand() % 2;
						maze.yPlane[i][j][k] = rand() % 2;
						if (maze.yPlane[i][j][k]) ans++;
						maze.zPlane[i][j][k] = rand() % 2;
					}
				}
	cout<<"ans="<<ans<<endl;
}
void reshape(int w,int h)
{
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60,(GLfloat)w/(GLfloat)h,1.0,1000);
}
void idleFunction()
{
	int x = (current_x - MAZE_LX) / (MAZE_XRANGE / N);
	int y = (current_y - MAZE_LY) / (MAZE_YRANGE / N);
	int z = (current_z - MAZE_LZ) / (MAZE_ZRANGE / N);
	//cout<<y<<' '<<x<<' '<<z<<' '<<maze.yPlane[y][x][z]<<endl;
	if (lookAtUpY > 0){
		if (maze.yPlane[y][x][z] == 0){
			current_y -= (MAZE_XRANGE / N) / 5;
			glutPostRedisplay();
		}
	}
	else if (lookAtUpY < 0) {
		if (maze.yPlane[y+1][x][z] == 0){
			current_y += (MAZE_XRANGE / N) / 5;
			glutPostRedisplay();
		}
	}
}

int main(int argc,char *argv[])
{
	makeMaze(N);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(800,800);
	glutInitWindowPosition(100,100);
	glutCreateWindow("maze");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseClickEvtListen);
	glutMotionFunc(mouseMoveEvtListen);
	glutSpecialFunc(placeControl);
	glutIdleFunc(idleFunction);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}