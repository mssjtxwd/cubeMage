#ifndef _TGA_H_
#define _TGA_H_
#include "stdafx.h"

using namespace std;  
  
//����ṹ�嶨��  
typedef struct  
{  
    GLubyte *imageData;//ͼ������  
    GLuint bpp;//�������  
    GLuint width;//ͼ����  
    GLuint height;//ͼ��߶�  
    GLuint texID;//��Ӧ������ID  
}TextureImage;  
//����TGAͼ����ѹ����ʽ������������  

bool LoadTGA(TextureImage *texture,const char *filename) ;
#endif