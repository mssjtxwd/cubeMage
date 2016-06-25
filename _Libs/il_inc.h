/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#ifndef _IL_INC_
#define _IL_INC_

#ifdef _WIN32
#	ifdef _WIN64
#		pragma comment (lib, "DevIL_x64.lib")
#	else
#		pragma comment (lib, "DevIL.lib")
#	endif
#endif

#include "IL/il.h"

inline void il_init(){
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_FILE_OVERWRITE);
}

// RGBA, unsigned byte, wchar_t* L"??"
inline void il_readImg(const wchar_t* name, void** data, int* w, int* h){
	ILuint img;
	ilGenImages(1, &img);
	ilBindImage(img);
	ILboolean r = ilLoadImage(name);
	*h = ilGetInteger(IL_IMAGE_HEIGHT);
	*w = ilGetInteger(IL_IMAGE_WIDTH);
	*data = new unsigned char[*w * *h * 4];
	ILuint n = ilCopyPixels(0,0,0,*w,*h,1,IL_RGBA,IL_UNSIGNED_BYTE,*data);
	ilDeleteImage(img);
	ILenum error = ilGetError();
}

// RGBA, unsigned byte, wchar_t* L"??"
inline void il_saveImg(const wchar_t* name, void* data, int w, int h){
	ILuint img;
	ilGenImages(1, &img);
	ilBindImage(img);
	ILboolean r = ilTexImage(w,h,1,4,IL_RGBA,IL_UNSIGNED_BYTE,data);
	r = ilSaveImage(name);
	ilDeleteImage(img);
	ILenum error = ilGetError();
}

// depth float
inline void il_saveImgDep(const wchar_t* name, float* data, int w, int h)
{
	unsigned char* data2 = new unsigned char[w*h*4];
	for(int i=0; i<w*h; ++i){
		data2[i*4] = data2[i*4+1] = data2[i*4+2]
		= unsigned char( std::max(0.0f,std::min(1.0f,data[i]))*255 );
		data2[i*4+3] = unsigned char( 255 );
	}
	il_saveImg(name,data2,w,h);
	delete[] data2;
}

// save the frame color buffer drawed into the window
inline void il_saveImgWin(const wchar_t* name, int ox, int oy, int w, int h){
	unsigned char* data = new unsigned char[w*h*4];
	glReadPixels(ox,oy,w,h,GL_RGBA,GL_UNSIGNED_BYTE,data);
	il_saveImg(name,data,w,h);
	delete[] data;
}

// save the frame depth buffer drawed into the window
inline void il_saveImgWinDep(const wchar_t* name, int ox, int oy, int w, int h){
	float* data = new float[w*h];
	glReadPixels(ox,oy,w,h,GL_DEPTH_COMPONENT,GL_FLOAT,data);
	/*unsigned char rgba[4] = {0xff,0xff,0xff,0xff};
	for(int i=0; i<w*h; ++i){
	rgba[0]=rgba[1]=rgba[2] = unsigned char(data[i]*255);
	*((int*)(&data[i])) = *((int*)rgba);
	}
	il_saveImg(name,data,w,h);*/
	il_saveImgDep(name, data, w, h);
	delete[] data;
}

#endif // #ifndef _IL_INC_
