
#include <list>
#include <map>
#include <string>
#include <random>
#include <afx.h>
#include <iostream>

#include <Windows.h>
#include <stdio.h>
#include <Winsock2.h>
#include <Windows.h>
#include "../include/CSqliteExpandDll.h"
#include "../common/TxSystemDependent.hpp"
#include "../common/TxCrc32CheckCode.hpp"
#include "../common/TxAtomicInteger.h"
#include <Shlwapi.h>
#include <atlstr.h>
#include <memory>
#include <iostream>
#include <shlobj.h>
#include "demo_func_test.h"

#pragma comment(lib,"Shlwapi.lib")

using namespace std;

static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM aaa, LPARAM lpData)     
{  
	if(uMsg == BFFM_INITIALIZED)  
	{  
		//CTreeCtrl   treePath;  
		//HTREEITEM   hItemSel;  
		::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);  
		int jklsdf=0;
		//::MoveWindow(hWnd,0,0,500,500,TRUE);
		//treePath.SubclassWindow(::GetDlgItem(hWnd, 0x3741));  
		//hItemSel    = treePath.GetSelectedItem();  
		//treePath.Expand(hItemSel, TVE_COLLAPSE);  
		//treePath.UnsubclassWindow();  
	}   
	return 0;    
}  

void sdfssss()
{
	BROWSEINFO   bi;   
	char   szPath[MAX_PATH]="C:\\Users\\litianxiu\\Desktop\\common\\";
	LPITEMIDLIST   pList   =   NULL;   

	// 配置路径对话框
	memset(&bi, 0, sizeof(BROWSEINFO));

	bi.hwndOwner   =  NULL;
	bi.pidlRoot   =   pList;   
	bi.pszDisplayName   =   szPath;   

	bi.lpszTitle   =  "选择路径";
	bi.ulFlags   =   BIF_EDITBOX;   
	bi.lpfn   =   BrowseCallbackProc;   
	bi.lParam   =   (LPARAM)szPath; 
	bi.iImage   =   0;

	//弹出选择目录对话框 
	if((pList=SHBrowseForFolder(&bi))!=NULL)
	{    
		while(SHGetPathFromIDList(pList,szPath))
		{
			int a=0;
		}
	}
}
void dfdfsdfs()
{
	BROWSEINFO bi;
	char Buffer[MAX_PATH];
	//初始化入口参数bi开始
	bi.hwndOwner = NULL;
	bi.pidlRoot =NULL;//初始化制定的root目录很不容易
	bi.pszDisplayName = Buffer;//此参数如为NULL则不能显示对话框
	bi.lpszTitle = "选择Sis目标文件路径";
	bi.ulFlags = BIF_EDITBOX|BIF_RETURNONLYFSDIRS;//带编辑框的风格
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage=0;
	//初始化入口参数bi结束
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if(pIDList)
	{
		SHGetPathFromIDList(pIDList, Buffer);
		//取得文件夹路径到Buffer里
		CString m_cSisDes = Buffer;//将路径保存在一个CString对象里
	}

	// free memory used     
	IMalloc * imalloc = 0;
	if (SUCCEEDED(SHGetMalloc(&imalloc)))
	{
		imalloc->Free (pIDList);
		imalloc->Release();
	}
}

void sfddfdfaaaa()
{
	//{
	//	TxAtomicInteger<char> p1;
	//	TxAtomicInteger<short> p2;
	//	TxAtomicInteger<int> p3;
	//	TxAtomicInteger<long long> p4;
	//	TxAtomicInteger<double> p5;
	//	p4.plus(1);
	//	p2.plus(1);
	//	p2=p2;
	//	p3.plus(1);
	//	double adfs=p5.plus(1);
	//	float aaa=p5.minus(10);
	//	int sdf=0;
	//}
	{
		__declspec(align(8)) class AAA
		{
		public:
			char pp;
			char p2;
			char pp3;
			//__declspec(align(8)) char pp;
			//__declspec(align(8)) char p2;
		};
#pragma pack(1)
class BBB
{
public:
	char a;
	unsigned long long cc;
	AAA b;
};
#pragma pack()
class AAA aaa; (void)aaa;
class BBB bbb;

bbb.b.p2=(char)123;
bbb.b.pp=(char)10;
int fds=(int)&bbb.b.p2;
int fds1=(int)&bbb.b.pp;
int fds3=(int)&bbb.b.pp3;
int a=0;

	}
	TxSystemDependent::TxTimeSpan<false> ttt;
	for(int i=(int)100;i--;)
	{
		TxCrc32CheckCode mmm;
		mmm.pushFile("F:\\新建文件夹\\新建文件夹\\trunk.rar");
		unsigned int vvv=mmm.getUint32Value();
		char sssss[4];
		mmm.getCrc32Bytes(sssss);
		int fds=0;
	}
	printf("ggg=%g\n",(double)ttt.getCurrentMillisecond()/1000.0);
}

int main()
{
	{
		CAudioWaveOutput1::run();
		return 0;
	}
	sfddfdfaaaa();
	return 0;
	sdfssss();
	dfdfsdfs();
	TxSqliteDbmsService::initializeSQLiteServiceEnv(20,1440);
	if(1)
	{
		if(!TxSqliteDbmsService::createDbmsServer(6060,"../bin/log.db",true))
		{
			printf("初始化数据库失败!!!!!!!!!!!!!!!!!!\n");
		}
		else
		{
			printf("初始化数据库成功\n");
		}
		for(int bLoopRun=1;bLoopRun;)
		{
			char sss[1024];
			printf("程序正在运行中。。。\n请输入（q表示退出）：");
			scanf("%s",sss);
			if(sss[1]!=0) continue;
			switch(sss[0])
			{
			case 'q':
			case 'Q':
				bLoopRun=0;
				break;
			default:
				break;
			}
		}
		printf("程序退出\n");
	}
	TxSqliteDbmsService::uninitializeSQLiteServiceEnv();
	return 0;
}

