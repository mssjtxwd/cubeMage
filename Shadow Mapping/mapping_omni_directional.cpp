/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "gl_staff.h"
#include "bt_inc.h"

#include <iostream>

/*
 glActiveTexture(GL_TEXTURE0) ��ͨ��ͼ
 glActiveTexture(GL_TEXTURE1) ��Ӱ����
 ϵͳ֧�ֵ���������Ⱥͳ��ȣ���ѯ glGetIntegerv(GL_MAX_TEXTURE_SIZE, GLint*)
 �ҵĻ�����GT240 1GB GDDR5 OpenGL 3.3���������ߴ�Ϊ 8192
*/

GLuint tex_walls;
GLuint tex_shadow[6]; // �������6���棬ȫ�����Դ
const int shadow_w=1024, shadow_h=1024;
GLuint frame_buffer_s;
glm::vec4 directions_s[6];
glm::mat4 shadow_mat_p, shadow_mat_v[6];
glm::vec4 light_pos; // ��������
bool light_rotate=false, save_shadow=false;
FTFont* ftgl;

void tex_init()
{
	glActiveTexture(GL_TEXTURE0); // general texture
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	void* data; int w, h;
	il_readImg(L"checker.png", &data, &w, &h);
	glGenTextures(1, &tex_walls);
	glBindTexture(GL_TEXTURE_2D, tex_walls);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1); // shadow texture
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);

	glGenTextures(6, tex_shadow);
	for(int i=0; i<6; ++i){
		glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		// �����������صĴ洢�ռ䣬�� OpenGL 3.3 (Compatibility Profile) Specification pp.237
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_w, shadow_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		GLfloat c[4]={0,0,0,1}; glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
	}
	shadow_mat_p = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0e10f);
	directions_s[0]=glm::vec4( 1, 0, 0, 0); directions_s[1]=glm::vec4(-1, 0, 0, 0);
	directions_s[2]=glm::vec4( 0, 0, 1, 0); directions_s[3]=glm::vec4( 0, 0,-1, 0);
	directions_s[4]=glm::vec4( 0, 1, 0, 0); directions_s[5]=glm::vec4( 0,-1, 0, 0);

	// frame buffer object
	glGenFramebuffers(1, &frame_buffer_s);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow[i], 0);

}

void xoy_foor(float len, float tex_repeat, int subdivision)
{
	GLfloat color[]={.8f, 0.8f, 0.8f, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(len, len, 1);
	glNormal3f(0,0,1);
	float u = 1.0f/subdivision;
	float tu = tex_repeat/subdivision;
	for(int i=0; i<subdivision; ++i)
	for(int j=0; j<subdivision; ++j){
		float ox = u*i, oy = u*j;
		float tox = tu*i, toy = tu*j;
		glBegin(GL_POLYGON);
		glTexCoord2f(tox   , toy);		glVertex3f(ox  , oy  , 0);
		glTexCoord2f(tox+tu, toy);		glVertex3f(ox+u, oy  , 0);
		glTexCoord2f(tox+tu, toy+tu);	glVertex3f(ox+u, oy+u, 0);
		glTexCoord2f(tox   , toy+tu);	glVertex3f(ox  , oy+u, 0);
		glEnd();
	}
	glPopMatrix();
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
	glutSolidSphere(0.1f, 50, 50);
	glPopMatrix();
	if(li) glEnable(GL_LIGHTING);
	glColor4fv(cc);
}

void draw_world()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); glTranslatef(0,1,0);
		//glStaff::xyz_frame(2,2,2,false);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); glRotatef(90, -1, 0, 0); glTranslatef(-10, -10, 0);
		xoy_foor(20, 3, 50);
	glPopMatrix();
	glPushMatrix(); glRotatef(90,  0, 1, 0); glTranslatef(-10, 0, -10);
		xoy_foor(20, 3, 50);
	glPopMatrix();
	glPushMatrix(); glTranslatef(-10, 0, -10);
		xoy_foor(20, 3, 50);
	glPopMatrix();
}

void draw_model()
{	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(-5, 0, 0);
	glRotatef(-45, 1,0,0);
	glutSolidCube(2);
	glRotatef(45, 1,0,0);
	glTranslatef(5, 0, 0);

	glTranslatef(0, 0, -5);
	glRotatef(-60, 1,0,0);
	glutSolidCone(1, 2, 50, 50);
	glRotatef(60, 1,0,0);
	glTranslatef(0, 0, 5);

	glTranslatef(0, -5, 0);
	glutSolidSphere(1, 50, 50);
	glTranslatef(0, 5, 0);

	glTranslatef(-5, 2, -5);
	glRotatef(45, 1,0,0);
	glutSolidTorus(0.5f, 1, 50, 50);
	glRotatef(-45, 1,0,0);

	glPopMatrix();
}

void draw(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	double tt = omp_get_wtime();
	//-------------------------------- ��1�λ��ƣ������������ -------------------------
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, shadow_w, shadow_h);
	//GLenum re = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	GLboolean li = glIsEnabled(GL_LIGHTING); // ����Ҫ���պ�����
	if(li) glDisable(GL_LIGHTING);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);
	glMatrixMode(GL_PROJECTION); glPushMatrix();
	glMatrixMode(GL_MODELVIEW); glPushMatrix();
	for(int i=0; i<6; ++i){ // 6 ��
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow[i], 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		if(i<4)	shadow_mat_v[i] = glm::lookAt(glm::vec3(light_pos), glm::vec3(light_pos+directions_s[i]), glm::vec3(0,1,0));
		else    shadow_mat_v[i] = glm::lookAt(glm::vec3(light_pos), glm::vec3(light_pos+directions_s[i]), glm::vec3(0,0,1));
		glMatrixMode(GL_PROJECTION); glLoadMatrixf(&shadow_mat_p[0][0]);
		glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&shadow_mat_v[i][0][0]);
			draw_world();
		glMultMatrixf(&mat_model[0][0]);
			draw_model();
	}
	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	glDisable(GL_CULL_FACE); glCullFace(GL_BACK);

	if(save_shadow){
		save_shadow = false;
		glActiveTexture(GL_TEXTURE1);
		GLfloat* data = new GLfloat[shadow_w*shadow_h];
		for(int i=0; i<6; ++i){
			glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);//��ȡ��������
			wchar_t ss[50]; swprintf(ss, L"d%d.png", i);
			il_saveImgDep(ss, data, shadow_w, shadow_w);
		}
		delete[] data;
	}

	// �ָ��ӿڡ���ɫģ�塢����
	glViewport(0, 0, glStaff::get_frame_width(), glStaff::get_frame_height());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(li) glEnable(GL_LIGHTING);

	//-------------------------------- ��2�λ��ƣ����Ƴ��� ----------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 1 ������
	glDisable(GL_LIGHT0);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
	//float gac2[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac2); // black

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	// 2 ���Դ
	GLfloat la[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, la);
	float gac[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac); // black
	glEnable(GL_LIGHT0);
	glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);

	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &light_pos[0]); // λ��ʽ��Դ

	for(int i=0; i<6; ++i){
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
		// When the eye planes are specified, the GL will automatically post-multiply them
		// with the inverse of the current modelview matrix.
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&mat_view[0][0]); //glLoadIdentity();
		glm::mat4 mat = glm::translate(glm::vec3(0.5f,0.5f,0.5f))
			* glm::scale(glm::vec3(0.5f,0.5f,0.5f)) * shadow_mat_p * shadow_mat_v[i];
		mat = glm::transpose(mat);
		glTexGenfv(GL_S, GL_EYE_PLANE, &mat[0][0]);
		glTexGenfv(GL_T, GL_EYE_PLANE, &mat[1][0]);
		glTexGenfv(GL_R, GL_EYE_PLANE, &mat[2][0]);
		glTexGenfv(GL_Q, GL_EYE_PLANE, &mat[3][0]);

		glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&mat_view[0][0]);
			draw_world();
		glMultMatrixf(&mat_model[0][0]); glDisable(GL_TEXTURE_2D);
			draw_model();
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la); // �ָ�������
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 3 ���ƹ�Դ������
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	dlight();

	//glStaff::text_upperLeft("www.cnblogs.com/liangliangh/", 6);

	if(light_rotate)
		light_pos = glm::rotate(glm::radians(1.0f), glm::vec3(0,1,0)) * light_pos;

	tt = omp_get_wtime()-tt;
	char ss[50];
	sprintf(ss, "render time (ms): %.2f", tt*1000);
	glStaff::text_upperLeft(ss, 1);
}

void key_p()
{
	light_rotate = !light_rotate;
}

void key_u()
{
	save_shadow = true;
	std::cout << "shadow map saved" << '\n';
}

int main(void)
{
	glStaff::init_win(1000, 800,
		"OpenGL Shadow    - cnblogs.com/liangliangh", "C:\\Windows\\Fonts\\msyh.ttf");
	glStaff::init_gl();

	glStaff::set_mat_view(
		glm::lookAt( glm::vec3(10,10,10), glm::vec3(0,10,0), glm::vec3(0,1,0) ) );
	glStaff::set_mat_model( glm::translate( glm::vec3(0,10,0) ) );

	light_pos = glm::vec4(-2,10,-2, 1);

	glStaff::add_key_callback('P', key_p, L"�ı��Դλ��");
	glStaff::add_key_callback('U', key_u, L"������Ӱͼ��d.png��");

	tex_init();
	ftgl = new FTExtrudeFont("C:\\Windows\\Fonts\\msyh.ttf");
	ftgl->FaceSize(1);
	ftgl->Depth(0.1f);

	glStaff::renderLoop(draw);
}
