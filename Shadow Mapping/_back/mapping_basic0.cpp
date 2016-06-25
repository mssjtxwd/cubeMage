/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "gl_staff.h"
#include "bt_inc.h"

#include <iostream>

GLuint tex_shadow; // 纹理名字
glm::vec4 light_pos; // 世界坐标系中光源位置
glm::mat4 shadow_mat_p; // 光源视角的投影矩阵
glm::mat4 shadow_mat_v; // 光源视角的视图矩阵

void tex_init() // 纹理初始化
{
	// 纹理如何影响颜色，和光照计算结果相乘
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// 分配纹理对象，并绑定为当前纹理
	glGenTextures(1, &tex_shadow);
	glBindTexture(GL_TEXTURE_2D, tex_shadow);
	// 纹理坐标超出[0,1]时如何处理
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// 非整数纹理坐标处理方式，线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 深度纹理，深度值对应亮度
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	// 纹理比较模式
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
}

void dlight() // 在灯上绘制一个球
{
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	GLfloat cc[4]; glGetFloatv(GL_CURRENT_COLOR, cc);
	glColor3f(0.8f, 0.8f, 0.1f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	glutSolidSphere(0.05f, 50, 50);
	glPopMatrix();
	if(li) glEnable(GL_LIGHTING);
	glColor4fv(cc);
}

void draw_world() // 绘制世界，一个地板
{
	glm::vec4 v1(-3, 0,-3, 1), v2(-3, 0, 3, 1), v3( 3, 0, 3, 1), v4( 3, 0,-3, 1);
	//*
	glm::mat4 m = glm::translate(glm::vec3(0.5f,0.5f,0.5f/*0.49f*/))
		* glm::scale(glm::vec3(0.5f,0.5f,0.5f)); // 需要将裁剪坐标的[-1,+1]缩放到[0,1]
	glm::vec4 t;
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0);
	t = m*shadow_mat_p*shadow_mat_v*v1;
	glTexCoord4fv(&t[0]); glVertex3fv(&v1[0]);
	t = m*shadow_mat_p*shadow_mat_v*v2;
	glTexCoord4fv(&t[0]); glVertex3fv(&v2[0]);
	t = m*shadow_mat_p*shadow_mat_v*v3;
	glTexCoord4fv(&t[0]); glVertex3fv(&v3[0]);
	t = m*shadow_mat_p*shadow_mat_v*v4;
	glTexCoord4fv(&t[0]); glVertex3fv(&v4[0]);
	glEnd();//*/
	/*
	glm::mat4 m = glm::translate(glm::vec3(0.5f,0.5f,0.5f))
		* glm::scale(glm::vec3(0.5f,0.5f,0.5f)); // 需要将裁剪坐标的[-1,+1]缩放到[0,1]
	m = m*shadow_mat_p*shadow_mat_v;
	glMatrixMode(GL_TEXTURE); glLoadMatrixf(&m[0][0]); glMatrixMode(GL_MODELVIEW);
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0);
	glTexCoord4fv(&v1[0]); glVertex3fv(&v1[0]);
	glTexCoord4fv(&v2[0]); glVertex3fv(&v2[0]);
	glTexCoord4fv(&v3[0]); glVertex3fv(&v3[0]);
	glTexCoord4fv(&v4[0]); glVertex3fv(&v4[0]);
	glEnd();//*/
}

void draw_model() // 绘制模型，一个茶壶
{	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(0, 1, 0);
		glutSolidTeapot(1);
	glPopMatrix();
}

void draw(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	double tt = omp_get_wtime();
	//---------------------------------------第1次绘制，生成深度纹理--------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 将摄像机放置在光源位置，投影矩阵和视图矩阵
	shadow_mat_p = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 1.0e10f);
	shadow_mat_v = glm::lookAt(glm::vec3(light_pos), glm::vec3(0), glm::vec3(0,1,0));

	glMatrixMode(GL_PROJECTION); glPushMatrix();
	glLoadMatrixf(&shadow_mat_p[0][0]); // 加载投影矩阵
	glMatrixMode(GL_MODELVIEW); glPushMatrix();
	glLoadMatrixf(&shadow_mat_v[0][0]); // 加载视图矩阵
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();
	
	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	glPopMatrix();

	// 拷贝深度缓冲到纹理
	glBindTexture(GL_TEXTURE_2D, tex_shadow);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		0, 0, glStaff::get_frame_width(), glStaff::get_frame_height(), 0);
	glEnable(GL_TEXTURE_2D); // 使能纹理

	/*
	GLfloat* data = new GLfloat[glStaff::get_frame_width()*glStaff::get_frame_height()];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data); // 获取纹理数据
	il_saveImgDep(L"d0.png", data, glStaff::get_frame_width(), glStaff::get_frame_height());
	delete[] data;//*/

	//-------------------------------------------第2次绘制，绘制场景------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &light_pos[0]); // 位置式光源
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	dlight();

	tt = omp_get_wtime()-tt;
	char ss[50];
	sprintf(ss, "render time (ms): %.2f", tt*1000);
	glStaff::text_upperLeft(ss, 1);
}

int main(void)
{
	glStaff::init_win(800, 800,
		"OpenGL Shadow    - cnblogs.com/liangliangh", "C:\\Windows\\Fonts\\msyh.ttf");
	glStaff::init_gl();
	glStaff::set_mat_view( glm::lookAt( glm::vec3(2,10,6), glm::vec3(0,0,0), glm::vec3(0,1,0) ) );
	light_pos = glm::vec4(2,5,0.5f, 1);
	tex_init();
	glStaff::renderLoop(draw);
}
