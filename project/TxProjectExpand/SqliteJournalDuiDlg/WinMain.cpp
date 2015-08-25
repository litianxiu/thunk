// SqliteJournalDuiDlg.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "CMainFrameDlg.h"
#include "../common/TxDuiMessageBox.hpp"

void g_KillOtherSameNameProcess()
{
	char pFileName[FILENAME_MAX+8]={0};
	GetModuleFileName(NULL,pFileName,FILENAME_MAX);
	char *p1=strrchr(pFileName,'\\'),*p2=strrchr(pFileName,'/');
	if(p1==NULL||(size_t)p1<(size_t)p2)
		p1=p2;
	if(p1!=NULL)
	{
		p1++;
		HANDLE hProcess;
		PROCESSENTRY32 pe;
		HANDLE hShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		pe.dwSize=sizeof(PROCESSENTRY32);
		DWORD dwCurPgsId=::GetCurrentProcessId();
		if(Process32First(hShot,&pe))
		{
			do
			{
				if(strcmp(pe.szExeFile,p1)==0&&pe.th32ProcessID!=dwCurPgsId)
				{
					hProcess=OpenProcess(PROCESS_TERMINATE,FALSE,pe.th32ProcessID);
					TerminateProcess(hProcess,0);
					WaitForSingleObject(hProcess,INFINITE);
					DefAssertWrap(FALSE);
				}
			}
			while(Process32Next(hShot,&pe));
		}
		CloseHandle(hShot);
	}
}

void g_main_init_server(const std::vector<tagConfigSettingSqliteServerPort> &_data)
{
	for(int i=0;i<(int)_data.size();i++)
	{
		tagConfigSettingSqliteServerPort tem=_data[i];
		if(!TxSqliteDbmsService::createDbmsServer(tem.port,tem.filename.c_str(),tem.bJournal))
		{
			TxBlobString sss;
			sss.format("�޷�������������\n�˿ں�=%d��\n���ݿ��ļ�=%s��\n��־=%s��\n������鿴���ò�����",
				(int)tem.port,(const char*)tem.filename.c_str(),(const char*)(tem.bJournal?"��":"��"));
			TxDuiMessageBox::run(NULL,_T("����"),sss.c_str(),_T("ȷ��"));
		}
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	::CoInitialize(NULL);
	DuiLib::CPaintManagerUI::SetInstance(hInstance);// ����XML��ʱ����Ҫʹ�øþ��ȥ��λEXE��·�������ܼ���XML��·��

	tagConfigSettingData mLcConfigSettingData;
	const char* lc_err=mLcConfigSettingData.loadConfig();
	if(lc_err==NULL
		||TxDuiMessageBox::run(NULL,_T("�������ô���"),(TxBlobString(lc_err)+"\n�Ƿ�ȷ��Ҫ����ǰ��?").c_str(),_T("ȷ��"),_T("ȡ��"))==0)
	{
		if(mLcConfigSettingData.bKillOtherSameNameProcess)
			g_KillOtherSameNameProcess();
		TxSqliteDbmsService::initializeSQLiteServiceEnv(mLcConfigSettingData.iSvcThreadCount,mLcConfigSettingData.iNetWork_Mtu);

		g_main_init_server(mLcConfigSettingData.mServerInfo.listSqliteServer);

		CMainFrameDlg *wnd=new CMainFrameDlg(mLcConfigSettingData); // ���ɶ���//UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE
		wnd->Create(NULL,NULL,UI_WNDSTYLE_FRAME,0); // ����DLG����
		wnd->CenterWindow(); // ���ھ���

		DuiLib::CPaintManagerUI::MessageLoop();
		delete wnd;

		TxSqliteDbmsService::uninitializeSQLiteServiceEnv();
	}
	::CoUninitialize();
	return 0;
}
