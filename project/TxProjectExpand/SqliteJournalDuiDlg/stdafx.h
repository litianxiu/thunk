// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
//#include <afxwin.h>
#include <windows.h>
#include <Commdlg.h>
#include <atltypes.h>
#include <objbase.h>
#include <Tlhelp32.h>
#include <shlwapi.h>
#include <Dbghelp.h>


// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>


// TODO: 在此处引用程序需要的其他头文件

#include <UIlib.h>

//#ifdef _DEBUG
//#   ifdef _UNICODE
//#       pragma comment(lib, "../bin/DuiLib_ud.lib")
//#   else
//#       pragma comment(lib, "../bin/DuiLib_d.lib")
//#   endif
//#else
//#   ifdef _UNICODE
//#       pragma comment(lib, "../lib/DuiLib_u.lib")
//#   else
//#       pragma comment(lib, "../lib/DuiLib.lib")
//#   endif
//#endif

#define DefDuiSkin _T("skin")

#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"shlwapi.lib")