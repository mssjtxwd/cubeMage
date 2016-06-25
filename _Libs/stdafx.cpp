/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/
/*
 * 关于预编译头文件（Precompiled Header Files）
 * 参见 http://msdn.microsoft.com/en-us/library/szfdksca.aspx
 * 博客 http://www.cnblogs.com/cofd/archive/2007/11/05/949962.html
 摘自博客，下面给出一个使用预编译头文件的操作步骤:
 1) 添加一个stdafx.h文件(名字随便取, 这里用了VS默认提供的名称), 在这个.h文件里include要
    使用的头文件(一般是外部的库, 自己写的不常变的头文件也可以加进来)
 2) 添加一个stdafx.cpp文件, 并include "stdafx.h"
 3) 项目属性 - C/C++ - 预编译头（所有配置，所有平台） 设置为 使用/Yu, stdafx.h
 4) stdafx.cpp属性 - C/C++ - 预编译头（所有配置，所有平台） 设置为 创建/Yc, stdafx.h
*/

#include "stdafx.h"

#include "wrap/ply/plylib.cpp"

