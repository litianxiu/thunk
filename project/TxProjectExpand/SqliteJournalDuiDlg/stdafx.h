// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
//#include <afxwin.h>
#include <windows.h>
#include <Commdlg.h>
#include <atltypes.h>
#include <objbase.h>
#include <Tlhelp32.h>
#include <shlwapi.h>
#include <Dbghelp.h>


// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

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