#pragma once

#include "../common/TxGlobalConfig.h"
#include "CNsCommonTransmitInfo.hpp"
#include "CSocketArchiveClient.h"

class CDbSocketSqliteSession
{
private:
	TxSystemDependent::TxMutexWrap mutex;
	TxSystemDependent::TxThreadWrap mThread;
	TxSystemDependent::TxEventWrap m_event;
	enum { e_thread_null=1,e_thread_run ,e_thread_end, } eThreadRunning;
	std::list<CNsTransmitInfo::tagRequestInsertJournal> listLogData;
	std::string strIp;
	const int ciPort;
	const int ciMtu;
	TxIocpTcpApi::TxIocpService *const p_svc;
private:
	static void _static_call_back_(void *_arg1,void *_arg2);
	//bool _send_log_(CNsTransmitInfo::tagRequestInsertJournal *_tem);
	void _call_back_();
public :
	CDbSocketSqliteSession(TxIocpTcpApi::TxIocpService *_p_svc,int _ciMtu,const char *_ip,int _port);
	~CDbSocketSqliteSession();
	void addLog(const CNsTransmitInfo::tagRequestInsertJournal &_tem);
	void finish();
};
