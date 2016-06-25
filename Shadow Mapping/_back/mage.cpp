#include "global.h"
#include "mage.h"

const float Wall::THICKNESS = 0.5;
void Wall::setTexture(GLuint tex)
{
	tex_walls = tex;
}
void Wall::drawTexture()
{
	
	const GLfloat EPS = 0;
	GLfloat x1 = p1.x-EPS,x2 = p2.x+EPS,y1 = p1.y-EPS,y2 = p2.y+EPS,z1 = p1.z-EPS,z2 = p2.z+EPS;
	glActiveTexture(GL_TEXTURE0);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_walls);  
	glBegin(GL_QUADS);  
        // 前面  
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y1, z2);  // 纹理和四边形的左下  
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x2, y1, z2);  // 纹理和四边形的右下  
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x2, y2, z2);  // 纹理和四边形的右上  
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x1, y2, z2);  // 纹理和四边形的左上  
        // 后面  
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x1, y1, z1);  // 纹理和四边形的右下  
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x1, y2, z1);  // 纹理和四边形的右上  
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x2, y2, z1);  // 纹理和四边形的左上  
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x2, y1, z1);  // 纹理和四边形的左下  
        // 顶面  
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x1, y2, z1);  // 纹理和四边形的左上  
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y2, z2);  // 纹理和四边形的左下  
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x2, y2, z2);  // 纹理和四边形的右下  
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x2, y2, z1);  // 纹理和四边形的右上  
        // 底面  
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x1, y1, z1);  // 纹理和四边形的右上  
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x2, y1, z1);  // 纹理和四边形的左上  
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x2, y1, z2);  // 纹理和四边形的左下  
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x1, y1, z2);  // 纹理和四边形的右下  
        // 右面  
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x2, y1, z1);  // 纹理和四边形的右下  
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x2, y2, z1);  // 纹理和四边形的右上  
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x2, y2, z2);  // 纹理和四边形的左上  
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x2, y1, z2);  // 纹理和四边形的左下  
        // 左面  
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y1, z1);  // 纹理和四边形的左下  
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x1, y1, z2);  // 纹理和四边形的右下  
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x1, y2, z2);  // 纹理和四边形的右上  
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x1, y2, z1);  // 纹理和四边形的左上  
    glEnd();  
	
}
static void
drawBox(GLfloat size, GLenum type)
{
  static GLfloat n[6][3] =
  {
    {-1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, -1.0}
  };
  static GLint faces[6][4] =
  {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };
  GLfloat v[8][3];
  GLint i;

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

  for (i = 5; i >= 0; i--) {
    glBegin(type);
    glNormal3fv(&n[i][0]);
    glVertex3fv(&v[faces[i][0]][0]);
    glVertex3fv(&v[faces[i][1]][0]);
    glVertex3fv(&v[faces[i][2]][0]);
    glVertex3fv(&v[faces[i][3]][0]);
    glEnd();
  }
}

void APIENTRY
glutSolidCube2(GLdouble size)
{
  drawBox(size, GL_QUADS);
}
void Wall::draw()
{
	/*
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::vec3 tmp = p1 + p2;
	
	glTranslatef(tmp.x / 2, tmp.y / 2 - 0.5f,tmp.z / 2 );
	
	tmp = p1 - p2;
	glScalef(fabs(tmp.x),fabs(tmp.y),fabs(tmp.z));
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    //glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, tex_walls);
    glutSolidCube2(1);
    //glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    //glDisable(GL_TEXTURE_GEN_T);
	

	glPopMatrix();
	*/
	drawTexture();
}

bool Wall::includ(glm::vec3 pos)
{
	const GLfloat EPS = 0.7;
	auto v1 = p1 - glm::vec3(EPS,EPS,EPS),v2 = p2 + glm::vec3(EPS,EPS,EPS);
	return (pos.x >= v1.x && pos.x <= v2.x &&
		pos.y >= v1.y && pos.y <= v2.y &&
		pos.z >= v1.z && pos.z <= v2.z);
}
void Wall::Log()
{
	char t;
	switch (type)
	{
	case X:t = 'x';break;
	case Y:t = 'y';break;
	case Z:t = 'z';
	}
	cout<<"a new "<<t<<" Wall is created; p1 = {"<<p1.x<<','<<p1.y<<','<<p1.z<<"};";
	cout<<"p2 = {"<<p2.x<<','<<p2.y<<','<<p2.z<<"};";
	cout<<endl;
}
Wall Wall::wallFac(WCHAR type,glm::vec2 v1,glm::vec2 v2,GLfloat plane,GLfloat thickness) 
{
	using namespace glm;
	auto ret = *(new Wall());
	switch (type)
	{
	case L'x':ret.p1 = vec3(plane,v1.x,v1.y); ret.p2 = vec3(plane + thickness,v2.x,v2.y); ret.type = X;break;
	case L'y':ret.p1 = vec3(v1.x,plane,v1.y); ret.p2 = vec3(v2.x,plane + thickness,v2.y); ret.type = Y;break;
	case L'z':ret.p1 = vec3(v1.x,v1.y,plane); ret.p2 = vec3(v2.x,v2.y,plane + thickness); ret.type = Z;break;
	default: cout<<type<<" is not right char for wall factory"<<endl; 
	}
	//ret.Log();
	return ret;
}

Maze::Maze()
{
	//freopen("log.txt","w",stdout);
	size = MAZE_SIZE;
	n = N;
	cellSize = size / n;
	mazeL = -size / 2;
	mazeR = size / 2;
	srand(time(NULL));
	int ans = 0;
	for (int i=0;i<=n;i++)
		for (int j=0;j<n;j++)
				for (int k=0;k<n;k++)
				{/*
					if (i==0 || i==n){
						xPlane[i][j][k] = 1;
						yPlane[i][j][k] = 1;
						zPlane[i][j][k] = 1; 
					}
					else */{
						xPlane[i][j][k] = rand() % 2;
						yPlane[i][j][k] = rand() % 2;
						zPlane[i][j][k] = rand() % 2;
					}
				}
	int tk = Wall::THICKNESS;
	for (int i=0;i<=n;i++)
		for (int j=0;j<n;j++)
				for (int k=0;k<n;k++)
				{
					ans += int(xPlane[i][j][k]) + int(yPlane[i][j][k]) + int(xPlane[i][j][k]);
					using namespace glm;
					auto v1 = vec2(j*cellSize,k*cellSize),v2 = vec2((j+1)*cellSize,(k+1)*cellSize);
					if (xPlane[i][j][k]) 
					{
						walls.push_back(Wall::wallFac('x',v1,v2,i*cellSize));
						xPlane[i][j][k] = int(walls.size()) - 1;
					}
					else xPlane[i][j][k] = -1;
					if (yPlane[i][j][k]) 
					{
						walls.push_back(Wall::wallFac('y',v1,v2,i*cellSize));
						yPlane[i][j][k] = int(walls.size()) - 1;
					}
					else yPlane[i][j][k] = -1;
					if (zPlane[i][j][k]) 
					{
						walls.push_back(Wall::wallFac('z',v1,v2,i*cellSize));
						zPlane[i][j][k] = int(walls.size()) - 1;
					}
					else zPlane[i][j][k] = -1;
				}

	cout<<"the number of wall in maze is "<<ans<<endl;
}

glm::vec3 Maze::getIdx(GLfloat x,GLfloat y,GLfloat z)
{
	const float EPS = 1E-5;
	int i = int(x / cellSize + EPS),
		j = int(y / cellSize + EPS),
		k = int(z / cellSize + EPS);

	return glm::vec3(i,j,k);
}
glm::vec3 Maze::getIdx(glm::vec3 pos)
{
	GLfloat x = pos.x,y = pos.y,z = pos.z;
	const float EPS = 1E-5;
	int i = int(x / cellSize + EPS),
		j = int(y / cellSize + EPS),
		k = int(z / cellSize + EPS);

	return glm::vec3(i,j,k);
}
void Maze::draw()
{
	for (int i=0;i<walls.size();i++)
	{
		walls[i].draw();
	}
		
}

void Maze::setTexture(GLuint tex)
{
	for (int i=0;i<walls.size();i++)
	{
		walls[i].setTexture(tex);;
	}
}

void Maze::drawTexture()
{
	for (int i=0;i<walls.size();i++)
	{
		walls[i].drawTexture();
	}
}

void print(glm::mat4 x)
{
	double dArray[16] = {0.0};

	const float *pSource = (const float*)glm::value_ptr(x);
	for (int i = 0; i < 16; ++i)
		cout<<pSource[i]<<' ';
	cout<<endl;
}
bool Maze::check(glm::mat4& mat_view)
{
	using namespace glStaff::Internal;
	//cout<<"mat view:"; print(glStaff::get_mat_view());
	const GLfloat H = 1;
	glm::mat4 viewModel = inverse(glStaff::get_mat_view());
	glm::vec3 pos(viewModel[3]); // Might have to divide by w if you can't assume w == 1
	auto idx = getIdx(pos);
	int x = idx.x,y = idx.y,z = idx.z;
	if (!g){
		if (yPlane[y][x][z]>=0 || y == 0) {
			//do nothing
		}
		else {
			using namespace glStaff;
			using namespace glStaff::Internal;
			glm::vec3 v = glm::vec3(mat_view*glm::vec4(0,1,0,0));
			
			mat_view = glm::translate(speed_scale*v) * mat_view; 
			
		}
	}
	else
	{
		if (yPlane[y+1][x][z]>=0 || y==n-1) {
			//do nothing
		}
		else {
			using namespace glStaff;
			using namespace glStaff::Internal;
			GLfloat mat[16]; 
			glGetFloatv( GL_MODELVIEW_MATRIX, mat );

			glm::vec3 v = glm::vec3(mat_view*glm::vec4(0,1,0,0));
				mat_view = glm::translate(-speed_scale*v) * mat_view; 
		}
	}
	//cout<<"mat view now is:"; print(glStaff::get_mat_view());
	return true;
}

bool Maze::checkCrack(glm::vec3 pos)
{
	glm::vec3 idx = getIdx(pos);
	int x = idx.x,y = idx.y,z = idx.z;
	bool ret = false;
	if (xPlane[x][y][z] >= 0) ret|= walls[xPlane[x][y][z]].includ(pos);
	if (xPlane[x+1][y][z] >= 0) ret|= walls[xPlane[x+1][y][z]].includ(pos);

	if (yPlane[y][x][z] >= 0) ret |= walls[yPlane[y][x][z]].includ(pos);
	if (yPlane[y+1][x][z] >= 0) ret |= walls[yPlane[y+1][x][z]].includ(pos);

	if (zPlane[z][x][y] >= 0) ret|= walls[zPlane[z][x][y]].includ(pos);
	if (zPlane[z+1][x][y] >= 0) ret|= walls[zPlane[z+1][x][y]].includ(pos);

	return ret;
}

