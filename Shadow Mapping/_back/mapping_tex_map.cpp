/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "gl_staff.h"
#include "bt_inc.h"

#include <iostream>

GLuint tex_lena; // ��������
glm::vec4 light_pos; // ��������ϵ�й�Դλ��
glm::mat4 shadow_mat_p; // ��Դ�ӽǵ�ͶӰ����
glm::mat4 shadow_mat_v; // ��Դ�ӽǵ���ͼ����

void tex_init() // �����ʼ��
{
	// �������Ӱ����ɫ���͹��ռ��������
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// ����������󣬲���Ϊ��ǰ����
	glGenTextures(1, &tex_lena);
	glBindTexture(GL_TEXTURE_2D, tex_lena);
	// �������곬��[0,1]ʱ��δ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// �߿���ɫ
	GLfloat c[4] = {0,0,0, 1};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
	// �������������괦��ʽ�����Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ���������Զ�����
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
	// ��������
	void* data; int w, h;
	//il_readImg(L"Lena Soderberg.jpg", &data, &w, &h);
	il_readImg(L"glass.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete data;
}

void dlight() // �ڵ��ϻ���һ����
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

void draw_world() // �������磬һ���ذ�
{
	glm::vec4 v1(-3, 0,-3, 1), v2(-3, 0, 3, 1), v3( 3, 0, 3, 1), v4( 3, 0,-3, 1);
	glBegin(GL_POLYGON);
		glNormal3f(0, 1, 0);
		glVertex3fv(&v1[0]);
		glVertex3fv(&v2[0]);
		glVertex3fv(&v3[0]);
		glVertex3fv(&v4[0]);
	glEnd();//*/
}

void draw_model() // ����ģ�ͣ�һ�����
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

	// ������������ڹ�Դλ�ã�ͶӰ�������ͼ����
	shadow_mat_p = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 1.0e10f);
	shadow_mat_v = glm::lookAt(glm::vec3(light_pos), glm::vec3(0), glm::vec3(0,1,0));
	// When the eye planes are specified, the GL will automatically post-multiply them
	// with the inverse of the current modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]); // glLoadIdentity();
	glm::mat4 mat =
		glm::translate(glm::vec3(0.5f,0.5f,0.5f)) * glm::scale(glm::vec3(0.5f,0.5f,0.5f))
		* shadow_mat_p * shadow_mat_v/* * glm::affineInverse(mat_view)*/;
	mat = glm::transpose(mat);
	glTexGenfv(GL_S, GL_EYE_PLANE, &mat[0][0]);
	glTexGenfv(GL_T, GL_EYE_PLANE, &mat[1][0]);
	glTexGenfv(GL_R, GL_EYE_PLANE, &mat[2][0]);
	glTexGenfv(GL_Q, GL_EYE_PLANE, &mat[3][0]);
	glEnable(GL_TEXTURE_2D);

	/*
	GLfloat* data = new GLfloat[glStaff::get_frame_width()*glStaff::get_frame_height()];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data); // ��ȡ��������
	il_saveImgDep(L"d0.png", data, glStaff::get_frame_width(), glStaff::get_frame_height());
	delete[] data;//*/

	//-------------------------------------------��2�λ��ƣ����Ƴ���------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	glDisable(GL_TEXTURE_2D);
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
