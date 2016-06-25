/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "gl_staff.h"
#include "bt_inc.h"
#include <iostream>
#include "mage.h"
#include "global.h"
#include "Object.h"
/*
 glActiveTexture(GL_TEXTURE0) 普通贴图
 glActiveTexture(GL_TEXTURE1) 阴影纹理
*/
bool grav = false;
Maze maze;
int lights_num = 3;
const int LIGHTS_NUM = N*N*N;
GLuint tex_shadow[LIGHTS_NUM];
GLuint tex_walls;
GLuint frame_buffer_s;
const int shadow_w=2048, shadow_h=2048;
glm::vec4 light_pos[LIGHTS_NUM]; // 世界坐标
bool light_rotate=false, save_stencil=false;
static FTFont* ftgl;
Object *chess;
void tex_init()
{
	glActiveTexture(GL_TEXTURE0); // 普通贴图
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	void* data; int w, h;
	il_readImg(L"checker_2.png", &data, &w, &h);
	glGenTextures(1, &tex_walls);
	glBindTexture(GL_TEXTURE_2D, tex_walls);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	chess = new Object[LIGHTS_NUM];

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

	glGenTextures(lights_num, tex_shadow);
	for(int i=0; i<lights_num; ++i){
		glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
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
	}
	// frame buffer object
	glGenFramebuffers(1, &frame_buffer_s);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow, 0);
}

void xoy_foor(float len, float tex_repeat, int subdivision)
{
	GLfloat color[]={.8f, 0.8f, 0.8f, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	
	glBindTexture(GL_TEXTURE_2D, tex_walls);
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

void dlight(glm::vec4& light_pos,int idx) // 在灯上绘制一个球
{
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	GLfloat cc[4]; glGetFloatv(GL_CURRENT_COLOR, cc);
	glColor3f(1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//chess[idx].draw(light_pos[0], light_pos[1], light_pos[2],0.5);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
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
	glTranslatef(-20, -20, 0);
		xoy_foor(40, 3, 50);
	glPopMatrix();
	//maze.drawTexture();
}

void draw_model()
{	
	maze.draw();
	chess[0].draw(8,8,8,0.1);
	/*
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glRotatef(90, 0, 1, 0);
	glScalef(1,2,3);
	glutSolidCube(1);

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glRotatef(0, 0, 1, 0);
	glScalef(1,2,3);
	glutSolidCube(1);

	glPopMatrix();
	*/
}

void gameOver()
{
	cout<<"game Over"<<endl;
}

void printx(glm::mat4 x)
{
	double dArray[16] = {0.0};

	const float *pSource = (const float*)glm::value_ptr(x);
	for (int i = 0; i < 16; ++i)
		cout<<pSource[i]<<' ';
	cout<<endl;
}

void draw(const glm::mat4& mat_model, glm::mat4& mat_view)
{
	if (grav){
		if (!maze.check(mat_view)) {
			gameOver();
			return;
		};
		//grav = false;
		//cout<<"but why mat view now is:"; printx(glStaff::get_mat_view());
	}
	
	double tt = omp_get_wtime();
	//-------------------------------- 第1次绘制，生成深度纹理 -------------------------
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, shadow_w, shadow_h);
	//GLenum re = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	// 不需要光照和纹理
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);

	glMatrixMode(GL_PROJECTION); glPushMatrix();
	glMatrixMode(GL_MODELVIEW); glPushMatrix();

	glm::mat4 shadow_mat_p = glm::perspective(glm::radians(100.0f), 1.0f, 1.0f, 1.0e10f);
	glm::vec3 cen = glm::vec3(glStaff::get_mat_model()*glm::vec4(0,0,0,1));
	glm::mat4 shadow_mat_v[LIGHTS_NUM];
	for(int i=0; i<lights_num; ++i){
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow[i], 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		shadow_mat_v[i] = glm::lookAt(glm::vec3(light_pos[i]), cen, glm::vec3(0,1,0));
		glMatrixMode(GL_PROJECTION); glLoadMatrixf(&shadow_mat_p[0][0]);
		glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&shadow_mat_v[i][0][0]);
			draw_world();
		glMultMatrixf(&mat_model[0][0]);
			draw_model();
	}

	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	glDisable(GL_CULL_FACE); glCullFace(GL_BACK);

	if(save_stencil){
		save_stencil = false;
		glActiveTexture(GL_TEXTURE1);
		GLfloat* data = new GLfloat[shadow_w*shadow_h];
		for(int i=0; i<lights_num; ++i){
			glBindTexture(GL_TEXTURE_2D, tex_shadow[i]);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);//获取纹理数据
			wchar_t ss[50]; swprintf(ss, L"d%d.png", i);
			il_saveImgDep(ss, data, shadow_w, shadow_w);
		}
		delete[] data;
	}

	// 恢复视口、颜色模板、光照
	//int x = glStaff::get_frame_width(),y = glStaff::get_frame_height();
	//std::cout<<x<<' '<<y<<std::endl;
	glViewport(0, 0, glStaff::get_frame_width(), glStaff::get_frame_height());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(li) glEnable(GL_LIGHTING);

	//-------------------------------- 第2次绘制，绘制场景 ----------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 1 环境光
	for(int i=0; i<lights_num; ++i) glDisable(GL_LIGHT0+i);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
	//float gac2[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac2); // black

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	// 2 点光源
	GLfloat la[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, la);
	float gac[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac); // black
	glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);

	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);

	for(int i=0; i<lights_num; ++i){ // lights_num
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

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&mat_view[0][0]);
			glLightfv(GL_LIGHT0+i, GL_POSITION, &light_pos[i][0]); // 位置式光源
		glEnable(GL_LIGHT0+i);

		glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&mat_view[0][0]);
			draw_world();
		glMultMatrixf(&mat_model[0][0]); glDisable(GL_TEXTURE_2D);
			draw_model();

		glDisable(GL_LIGHT0+i);
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la); // 恢复环境光
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// 3 绘制光源处的球
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	for(int i=0; i<lights_num; ++i) dlight(light_pos[i],i);

	//glStaff::text_upperLeft("www.cnblogs.com/liangliangh/", 6);

	if(light_rotate){
		for(int i=0; i<lights_num; ++i)
			light_pos[i] = glm::rotate(glm::radians(1.0f), glm::vec3(0,1,0)) * light_pos[i];
		/*static int coun=0;
		if(coun<360){ ++coun;
			if(coun%10==0){
				float* data = new float[glStaff::get_frame_width()*glStaff::get_frame_height()];
				glReadPixels(0,0,glStaff::get_frame_width(),glStaff::get_frame_height(),GL_RGBA,GL_UNSIGNED_BYTE,data);
				wchar_t ss[50]; swprintf(ss, L"c%d.png", coun/10);
				il_saveImg(ss,data,glStaff::get_frame_width(),glStaff::get_frame_height());
				delete[] data;
			}}*/
	}

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
	save_stencil = true;
	std::cout << "shadow map saved" << '\n';
}

void key_t()
{
	glm::mat4 viewModel = inverse(glStaff::get_mat_view());
	glm::vec3 cameraPos(viewModel[3]); // Might have to divide by w if you can't assume w == 1
	light_pos[0] = glm::vec4(cameraPos, 1);

}

void key_c()//reverse
{
	using namespace glStaff;
	using namespace Internal;
	mat_view = glm::rotate(90.0f, glm::vec3(1,0,0) ) * mat_view;
	maze.g = !maze.g;
	//speed_scale = -speed_scale;
}

void key_g()//reverse
{
	grav = !grav;
	if (grav) cout<<"grav has been opened."<<endl;
	else cout<<"grav has been closed"<<endl;
}

void key_r()
{
	maze.startCheckCrack = !maze.startCheckCrack;
	if (maze.startCheckCrack) cout<<"maze.startCheckCrack has been opened."<<endl;
	else cout<<"maze.startCheckCrack has been closed"<<endl;
}

int main(void)
{
	//glStaff::Internal::maze = maze;
	glStaff::init_win(1000, 800,"cubeMaze", "C:\\Windows\\Fonts\\Arial.ttf");
	//int x = glStaff::get_frame_width(),y = glStaff::get_frame_height();
	//std::cout<<x<<' '<<y<<std::endl;
	glStaff::init_gl();
	
	glStaff::set_mat_view(
		glm::lookAt( glm::vec3(12,12,12), glm::vec3(0,0,0), glm::vec3(0,1,0) ) );
	glm::mat4 viewModel = inverse(glStaff::get_mat_view());
	glm::vec3 cameraPos(viewModel[3]); // Might have to divide by w if you can't assume w == 1
	GLfloat vec4f[4]={0};
	vec4f[0]=1.0f; vec4f[1]=1.0f; vec4f[2]=1.0f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, vec4f); // red
	glLightfv(GL_LIGHT0, GL_SPECULAR, vec4f);
	//vec4f[0]=0.2f; vec4f[1]=1.0f; vec4f[2]=0.2f;
	glLightfv(GL_LIGHT1, GL_DIFFUSE, vec4f); // green
	glLightfv(GL_LIGHT1, GL_SPECULAR, vec4f);
	//vec4f[0]=0.2f; vec4f[1]=0.2f; vec4f[2]=1.0f; 
	glLightfv(GL_LIGHT2, GL_DIFFUSE, vec4f); // blue
	glLightfv(GL_LIGHT2, GL_SPECULAR, vec4f);

	vec4f[0]=0; vec4f[1]=0; vec4f[2]=0;
	glLightfv(GL_LIGHT0, GL_AMBIENT, vec4f); // black AMBIENT
	glLightfv(GL_LIGHT1, GL_AMBIENT, vec4f);
	glLightfv(GL_LIGHT2, GL_AMBIENT, vec4f);

	float gac2[4]={0.15f,0.15f,0.15f,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac2);

	light_pos[0] = glm::vec4(12,12,12, 1);
	light_pos[1] = glm::vec4(4,2,3, 1);
	light_pos[2] = glm::vec4(5,1,3, 1);

	glStaff::add_key_callback('P', key_p, L"改变光源位置");
	glStaff::add_key_callback('U', key_u, L"保存阴影图（d.png）");
	glStaff::add_key_callback('T', key_t, L"保存光源位置");
	glStaff::add_key_callback('C', key_c, L"倒转");
	glStaff::add_key_callback('G', key_g, L"开关重力");
	glStaff::add_key_callback('R', key_g, L"开关碰撞检测");
	tex_init();
	
	ftgl = new FTExtrudeFont("C:\\Windows\\Fonts\\Arial.ttf");
	ftgl->FaceSize(1);
	ftgl->Depth(0.1f);

	maze.setTexture(tex_walls);

	glStaff::renderLoop(draw);
}
