/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "gl_staff.h"
#include "bt_inc.h"

#include <iostream>

/*
 glActiveTexture(GL_TEXTURE0) 普通贴图
 glActiveTexture(GL_TEXTURE1) 阴影纹理
 系统支持的最大纹理宽度和长度，查询 glGetIntegerv(GL_MAX_TEXTURE_SIZE, GLint*)
 我的机器，GT240 1GB GDDR5 OpenGL 3.3，最大纹理尺寸为 8192
*/

GLuint tex_walls, tex_shadow;
GLuint frame_buffer_s;
const int shadow_w=2048, shadow_h=2048;
glm::vec4 light_direction; // 光源发光方向
bool light_rotate=false, save_shadow=false;
static FTFont* ftgl;

void tex_init()
{
	glActiveTexture(GL_TEXTURE0); // 普通贴图
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

	glActiveTexture(GL_TEXTURE1); // 阴影纹理
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);

	glGenTextures(1, &tex_shadow);
	glBindTexture(GL_TEXTURE_2D, tex_shadow);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat c[4] = {1,1,1, 1};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_w, shadow_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	/*GLuint render_buff_rgba, render_buff_depth;
	glGenRenderbuffers(1, &render_buff_rgba);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buff_rgba);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, shadow_w, shadow_h);
	glGenRenderbuffers(1, &render_buff_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buff_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, shadow_w, shadow_h);*/

	// frame buffer object
	glGenFramebuffers(1, &frame_buffer_s);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buff_rgba);
	//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buff_depth);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow, 0);
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

void dlight() // 在灯上绘制一个球
{
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	GLfloat cc[4]; glGetFloatv(GL_CURRENT_COLOR, cc);
	glColor3f(0.8f, 0.8f, 0.1f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(light_direction[0], light_direction[1], light_direction[2]);
	glutSolidSphere(0.1f, 50, 50);
	glPopMatrix();
	if(li) glEnable(GL_LIGHTING);
	glColor4fv(cc);
}

void draw_world()
{
	//glStaff::xyz_frame(2,2,2,false);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(90, -1, 0, 0);
	glTranslatef(-10, -10, 0);
		xoy_foor(20, 3, 100);
	glPopMatrix();
}

void draw_model()
{	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(0, 0, 0);
	glutSolidCube(2);

	glTranslatef(2, 0, 2);
	glRotatef(-90, 1,0,0);
	glutSolidCone(1, 2, 50, 50);
	glRotatef(90, 1,0,0);

	glTranslatef(0, 0, -4);
	glutSolidSphere(1, 50, 50);

	glTranslatef(-4, 1, 0);
	glRotatef(45, 1,0,0);
	glutSolidTorus(0.5f, 1, 50, 50);
	glRotatef(-45, 1,0,0);
	
	glPopMatrix();

	/*float c[4] = {0,1,0,1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);

	float d = ftgl->Advance(L"亮亮的园子");
	glTranslatef(-d/2,1,0);
	ftgl->Render(L"亮亮的园子");
	glTranslatef(d/2,-1,0);

	d = ftgl->Advance(L"cnblogs.com/liangliangh");
	glTranslatef(-d/2,0,0);
	ftgl->Render(L"cnblogs.com/liangliangh");
	glTranslatef(d/2,0,0);*/
}

void draw(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	double tt = omp_get_wtime();
	//-------------------------------- 第1次绘制，生成深度纹理 -------------------------
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, shadow_w, shadow_h);
	//GLenum re = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	glClear(GL_DEPTH_BUFFER_BIT);

	// 不需要光照和纹理
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);


	glm::mat4 shadow_mat_p = glm::ortho(-5.0f,5.0f, -5.0f,5.0f, -5.0f,5.0f);
	glm::vec4 cen = glStaff::get_mat_model()*glm::vec4(0,0,0,1);
	glm::mat4 shadow_mat_v = glm::lookAt(glm::vec3(cen), glm::vec3(-light_direction), glm::vec3(0,1,0));
	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);
	glMatrixMode(GL_PROJECTION); glPushMatrix();
	glLoadMatrixf(&shadow_mat_p[0][0]);
	glMatrixMode(GL_MODELVIEW); glPushMatrix();
	glLoadMatrixf(&shadow_mat_v[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	glDisable(GL_CULL_FACE); glCullFace(GL_BACK);

	glActiveTexture(GL_TEXTURE1);
	// When the eye planes are specified, the GL will automatically post-multiply them
	// with the inverse of the current modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	glm::mat4 mat = glm::translate(glm::vec3(0.5f,0.5f,0.5f))
		* glm::scale(glm::vec3(0.5f,0.5f,0.5f)) * shadow_mat_p * shadow_mat_v;
	mat = glm::transpose(mat);
	glTexGenfv(GL_S, GL_EYE_PLANE, &mat[0][0]);
	glTexGenfv(GL_T, GL_EYE_PLANE, &mat[1][0]);
	glTexGenfv(GL_R, GL_EYE_PLANE, &mat[2][0]);
	glTexGenfv(GL_Q, GL_EYE_PLANE, &mat[3][0]);

	if(save_shadow){
		save_shadow = false;
		glActiveTexture(GL_TEXTURE1);
		GLfloat* data = new GLfloat[shadow_w*shadow_h];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);//获取纹理数据
		il_saveImgDep(L"d.png", data, shadow_w, shadow_w);
		delete[] data;
	}

	// 恢复视口、颜色模板、光照
	glViewport(0, 0, glStaff::get_frame_width(), glStaff::get_frame_height());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(li) glEnable(GL_LIGHTING);

	//-------------------------------- 第2次绘制，绘制场景 ----------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 1 环境光
	glDisable(GL_LIGHT0);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
	//float gac2[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac2); // black

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	// 2 光源
	GLfloat la[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, la);
	float gac[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac); // black
	glEnable(GL_LIGHT0);
	glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);

	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &light_direction[0]); // 位置式光源
		draw_world();
	glMultMatrixf(&mat_model[0][0]); glDisable(GL_TEXTURE_2D);
		draw_model();

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la); // 恢复环境光
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// 3 绘制光源处的球
	/*glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	dlight();*/

	//glStaff::text_upperLeft("www.cnblogs.com/liangliangh/", 6);

	if(light_rotate)
		light_direction = glm::rotate(glm::radians(1.0f), glm::vec3(0,1,0)) * light_direction;

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
	glStaff::init_win(800, 800,
		"OpenGL Shadow    - cnblogs.com/liangliangh", "C:\\Windows\\Fonts\\msyh.ttf");
	glStaff::init_gl();

	float gac[4] = {0.4f, 0.4f, 0.4f, 1};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac);

	glStaff::set_mat_view(
		glm::lookAt( glm::vec3(10,10,10), glm::vec3(0,0,0), glm::vec3(0,1,0) ) );
	glStaff::set_mat_model( glm::translate( glm::vec3(0,1,0) ) );

	light_direction = glm::vec4(3,6,2, 0);

	GLfloat vec4f[4]={0};
	vec4f[0]=0.8f; vec4f[1]=0.8f; vec4f[2]=0.8f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, vec4f);
	glLightfv(GL_LIGHT0, GL_SPECULAR, vec4f);

	glStaff::add_key_callback('P', key_p, L"改变光源位置");
	glStaff::add_key_callback('U', key_u, L"保存阴影图（d.png）");

	tex_init();
	ftgl = new FTExtrudeFont("C:\\Windows\\Fonts\\msyh.ttf");
	ftgl->FaceSize(1);
	ftgl->Depth(0.1f);

	glStaff::renderLoop(draw);
}
