
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

	// ����·���Ի���
	memset(&bi, 0, sizeof(BROWSEINFO));

	bi.hwndOwner   =  NULL;
	bi.pidlRoot   =   pList;   
	bi.pszDisplayName   =   szPath;   

	bi.lpszTitle   =  "ѡ��·��";
	bi.ulFlags   =   BIF_EDITBOX;   
	bi.lpfn   =   BrowseCallbackProc;   
	bi.lParam   =   (LPARAM)szPath; 
	bi.iImage   =   0;

	//����ѡ��Ŀ¼�Ի��� 
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
	//��ʼ����ڲ���bi��ʼ
	bi.hwndOwner = NULL;
	bi.pidlRoot =NULL;//��ʼ���ƶ���rootĿ¼�ܲ�����
	bi.pszDisplayName = Buffer;//�˲�����ΪNULL������ʾ�Ի���
	bi.lpszTitle = "ѡ��SisĿ���ļ�·��";
	bi.ulFlags = BIF_EDITBOX|BIF_RETURNONLYFSDIRS;//���༭��ķ��
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage=0;
	//��ʼ����ڲ���bi����
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//������ʾѡ��Ի���
	if(pIDList)
	{
		SHGetPathFromIDList(pIDList, Buffer);
		//ȡ���ļ���·����Buffer��
		CString m_cSisDes = Buffer;//��·��������һ��CString������
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
		mmm.pushFile("F:\\�½��ļ���\\�½��ļ���\\trunk.rar");
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
			printf("��ʼ�����ݿ�ʧ��!!!!!!!!!!!!!!!!!!\n");
		}
		else
		{
			printf("��ʼ�����ݿ�ɹ�\n");
		}
		for(int bLoopRun=1;bLoopRun;)
		{
			char sss[1024];
			printf("�������������С�����\n�����루q��ʾ�˳�����");
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
		printf("�����˳�\n");
	}
	TxSqliteDbmsService::uninitializeSQLiteServiceEnv();
	return 0;
}

