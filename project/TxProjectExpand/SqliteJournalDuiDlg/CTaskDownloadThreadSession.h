#pragma once

#include "CCustomFileHandleRealize.h"
#include "../common/TxSystemDependent.hpp"
#include "../common/TxBlobString.hpp"
#include "CMainFrameDlgBasic.hpp"
#include "CDownUpLoadDuiDlg.h"

class CTaskDownloadThreadSession: public tagTaskThreadCommonSessionBasic
{
public :
private:
	TxSystemDependent::TxThreadWrap mThread;
	TxSystemDependent::TxMutexWrap mMutex;
	long long m_llTotalSize,m_llGainSize;
	TxBlobString strSaveFileUri;
	TxBlobString strSaveFileTempUri;
	TxBlobString strRemoteFileUri;
	TxBlobString strServerIp;
	int iServerPort;
	CDownUpLoadDuiDlg *pDownUpLoadDuiDlg;
	TxSystemDependent::TxEventWrap mEvent;
	BOOL bThreadRun;
private:
	static void _g_thread_cb_(void *_arg1,void *_arg2);
	void _thread_cb_(TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> &_spThis);
	tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase* newDoingUi_();
	tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase* newPauseUi_(const char *_text=NULL);
	tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase* newFinishUi_();
public :
	CTaskDownloadThreadSession(CDownUpLoadDuiDlg *_pDownUpLoadDuiDlg,const char *_ip,int _port,const char *_sRemoteFileUri,const char *_sLocalFileUri);
	//bool isThreadRunning();
	void createUi(tagTaskThreadCommonSessionBasic::E_UI _e);
	void start();
	void stop(bool _bWaitFinish);
	void getInformation(long long *_gainsize,long long *_totalsize);
	virtual void deletefile();
};
