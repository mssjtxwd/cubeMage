/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "gl_staff.h"
#include "bt_inc.h"

#include <iostream>

GLuint tex_shadow; // ��������
glm::vec4 light_pos; // ��������ϵ�й�Դλ��
glm::mat4 shadow_mat_p; // ��Դ�ӽǵ�ͶӰ����
glm::mat4 shadow_mat_v; // ��Դ�ӽǵ���ͼ����

void tex_init() // �����ʼ��
{
	// �������Ӱ����ɫ���͹��ռ��������
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// ����������󣬲���Ϊ��ǰ����
	glGenTextures(1, &tex_shadow);
	glBindTexture(GL_TEXTURE_2D, tex_shadow);
	// �������곬��[0,1]ʱ��δ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// �������������괦��ʽ�����Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ����������ֵ��Ӧ����
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	// ����Ƚ�ģʽ
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
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
	//*
	glm::mat4 m = glm::translate(glm::vec3(0.5f,0.5f,0.5f/*0.49f*/))
		* glm::scale(glm::vec3(0.5f,0.5f,0.5f)); // ��Ҫ���ü������[-1,+1]���ŵ�[0,1]
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
		* glm::scale(glm::vec3(0.5f,0.5f,0.5f)); // ��Ҫ���ü������[-1,+1]���ŵ�[0,1]
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
	//---------------------------------------��1�λ��ƣ������������--------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// ������������ڹ�Դλ�ã�ͶӰ�������ͼ����
	shadow_mat_p = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 1.0e10f);
	shadow_mat_v = glm::lookAt(glm::vec3(light_pos), glm::vec3(0), glm::vec3(0,1,0));

	glMatrixMode(GL_PROJECTION); glPushMatrix();
	glLoadMatrixf(&shadow_mat_p[0][0]); // ����ͶӰ����
	glMatrixMode(GL_MODELVIEW); glPushMatrix();
	glLoadMatrixf(&shadow_mat_v[0][0]); // ������ͼ����
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();
	
	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	glPopMatrix();

	// ������Ȼ��嵽����
	glBindTexture(GL_TEXTURE_2D, tex_shadow);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		0, 0, glStaff::get_frame_width(), glStaff::get_frame_height(), 0);
	glEnable(GL_TEXTURE_2D); // ʹ������

	/*
	GLfloat* data = new GLfloat[glStaff::get_frame_width()*glStaff::get_frame_height()];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data); // ��ȡ��������
	il_saveImgDep(L"d0.png", data, glStaff::get_frame_width(), glStaff::get_frame_height());
	delete[] data;//*/

	//-------------------------------------------��2�λ��ƣ����Ƴ���------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &light_pos[0]); // λ��ʽ��Դ
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
