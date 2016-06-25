/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#ifndef _BT_INC_
#define _BT_INC_

#ifdef _WIN32
#	ifdef _WIN64
#		ifdef _DEBUG
#			pragma comment (lib, "LinearMath_x64_d.lib")
#			pragma comment (lib, "BulletCollision_x64_d.lib")
#			pragma comment (lib, "BulletDynamics_x64_d.lib")
#			pragma comment (lib, "BulletSoftBody_x64_d.lib")
#		else
#			pragma comment (lib, "LinearMath_x64.lib")
#			pragma comment (lib, "BulletCollision_x64.lib")
#			pragma comment (lib, "BulletDynamics_x64.lib")
#			pragma comment (lib, "BulletSoftBody_x64.lib")
#		endif
#	else
#		ifdef _DEBUG
#			pragma comment (lib, "LinearMath_d.lib")
#			pragma comment (lib, "BulletCollision_d.lib")
#			pragma comment (lib, "BulletDynamics_d.lib")
#			pragma comment (lib, "BulletSoftBody_d.lib")
#		else
#			pragma comment (lib, "LinearMath.lib")
#			pragma comment (lib, "BulletCollision.lib")
#			pragma comment (lib, "BulletDynamics.lib")
#			pragma comment (lib, "BulletSoftBody.lib")
#		endif
#	endif
#endif

#include"btBulletDynamicsCommon.h"


#endif // #ifndef _BT_INC_