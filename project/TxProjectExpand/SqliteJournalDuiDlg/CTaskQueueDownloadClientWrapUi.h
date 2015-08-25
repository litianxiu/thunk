#pragma once

#include "CDownUpLoadDuiDlg.h"
#include "../include/CSqliteExpandDll.h"
#include "../common/TxBlobString.hpp"
#include "../common/TxIocpTcpApi.hpp"
#include "CTaskDownloadThreadSession.h"

//class CTaskQueueDownloadClientWrapUi: public CDownUpLoadDuiDlg::tagDuiListTaskDoingQueueElemBase
//{
//private:
//	DuiLib::CLabelUI *p_label_filename;
//	DuiLib::CLabelUI *p_label_datasize;
//	DuiLib::CProgressUI *p_progress_task;
//	DuiLib::CLabelUI *p_label_percent;
////private:
//	//TxCppPlatform::weak_ptr<CTaskDownloadThreadSession> wpClient;
//	//TxCppPlatform::shared_ptr<CTaskDownloadThreadSession> _spClient__;
//	CDownUpLoadDuiDlg *pDownUpLoadDuiDlg;
//	CTaskDownloadThreadSession m_TaskDownloadThreadSession;
////private:
////	static void CALLBACK g_DownLoadCallBackDllFunc(void *_userdata,const void *_data);
////	void msgCallBack(const tagDownLoadServerFileRespondInfo &_msg);
//public :
//	CTaskQueueDownloadClientWrapUi(const char *_ip,int _port,CDownUpLoadDuiDlg *_pDownUpLoadDuiDlg,
//		const char *_localFileUri,const char *_remoteFileUri);
//	~CTaskQueueDownloadClientWrapUi();
//	void initUi();
//	//void initArgs();
//	//void threadFlushFile(const void *_buff,int _size,bool _bFinish);
//	void setDownUpLoadContent(long long _gainsize,long long _totalsize,LPCTSTR _filename=NULL);
//	virtual void startTaskThread();
//	virtual void stopTaskThread();
//	virtual e_status getRunStatus();
//	virtual void onTimerRefresh();
//	//virtual bool isExpired();
//	//virtual void destroy();
//};
