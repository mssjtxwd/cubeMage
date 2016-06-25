#ifndef _TGA_H_
#define _TGA_H_
#include "stdafx.h"

using namespace std;  
  
//纹理结构体定义  
typedef struct  
{  
    GLubyte *imageData;//图像数据  
    GLuint bpp;//像素深度  
    GLuint width;//图像宽度  
    GLuint height;//图像高度  
    GLuint texID;//对应的纹理ID  
}TextureImage;  
//加载TGA图像（无压缩格式），生成纹理  

bool LoadTGA(TextureImage *texture,const char *filename) ;
#endif