/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#ifndef _GL_INC_
#define _GL_INC_

#ifdef _WIN32
#	ifdef _WIN64
#		pragma comment (lib, "glew32_x64.lib") // "Release/x64/" find only from current dir
#		pragma comment (lib, "freeglut_x64.lib")
#		pragma comment (lib, "glfw3dll_x64.lib")
#	else
#		pragma comment (lib, "glew32.lib")
#		pragma comment (lib, "freeglut.lib")
#		pragma comment (lib, "glfw3dll.lib")
#	endif
#endif

#pragma comment (lib, "glu32.lib")    // link OpenGL Utility lib
#pragma comment (lib, "opengl32.lib") // link Microsoft OpenGL lib

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"

//#define GLM_FORCE_RADIANS // need not for glm 9.6
#include "glm/glm.hpp"
#include "glm/ext.hpp"


#endif // #ifndef _GL_INC_