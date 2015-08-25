#pragma once

#include "../common/TxIocpTcpApi.hpp"
#include "CNsTransmitShellPackInfo.h"

struct MyDbSQLiteArchiveTcpClientConnectCallBackInfo_
{
	TxSystemDependent::TxEventWrap m_event;
	unsigned int uiUserData;
	unsigned int uiCallBackData;
};

class MySocketArchiveClientR: public TxIocpTcpApi::TxIocpTcpClientSocket
{
public :
	unsigned int uiRecordCurrentPackageNumber;
	CNsTransmitInfo::CShellPackData m_ClientShellPackData;
	//long lAccumulativeCmdNum;
	//互斥量保护区域
	TxSystemDependent::TxMutexWrap mWpReadEventMutex;
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> spReadPkgObj;
	TxCppPlatform::weak_ptr<MyDbSQLiteArchiveTcpClientConnectCallBackInfo_> wpReadEvent;
	BOOL bZoeticSign;
	BOOL bExecReadOnceSign;
	//互斥量保护区域
private:
	virtual bool onClientConnectEvent(bool _bResult);
	virtual bool onClientReadExceptionEvent();
	virtual bool onClientReadBufferEvent(const unsigned char *_data,int _size);
	virtual bool onClientWriteBufferEvent(int _iSurplusSize) { (void)_iSurplusSize; return false; }
public:
	MySocketArchiveClientR(TxIocpTcpApi::TxIocpService *_pTxIocpService,int _mtu);
	~MySocketArchiveClientR();
	bool blockConnect(const char *_ip,int _port);
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> execute(CNsTransmitInfo::tagJournalTransBase *_data,int _i_time_msec);
	//TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute> execute_sql(const char *_sql,int _i_time_msec);
	//TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondInsertJournal> addJournal(
	//	CNsTransmitInfo::tagRequestInsertJournal *_tem,int _i_time_msec);
};

//class CSocketArchiveClient
//{
//private:
//	TxCppPlatform::weak_ptr<MySocketArchiveClientR> wpJournalSession;
//	TxIocpTcpApi::TxIocpService *const p_svc;
//	TxSystemDependent::TxMutexWrap mutex;
//	TxSystemDependent::TxThreadWrap mThread;
//	TxSystemDependent::TxEventWrap m_event;
//	enum { e_thread_null=1,e_thread_run ,e_thread_end, } eThreadRunning;
//	std::list<CNsTransmitInfo::tagRequestInsertJournal> listLogData;
//	std::string strIp;
//	const int ciPort;
//	const int ciMtu;
//private:
//	static void _static_call_back_(void *_arg1,void *_arg2);
//	bool _force_send_log_(CNsTransmitInfo::tagRequestInsertJournal *_tem);
//	void _call_back_();
//public :
//	CSocketArchiveClient(TxIocpTcpApi::TxIocpService *_p_svc,int _ciMtu,const char *_ip,int _port);
//	~CSocketArchiveClient();
//	void addLog(const CNsTransmitInfo::tagRequestInsertJournal &_tem);
//	void finish();
//};

class CSocketArchiveClient
{
private:
	TxCppPlatform::weak_ptr<MySocketArchiveClientR> wpSession;
	char sIp[64];
	int iPort;
	int iMtu;
	TxIocpTcpApi::TxIocpService *p_svc;
public :
	CSocketArchiveClient(TxIocpTcpApi::TxIocpService *_p_svc,const char *_ip,int _port,int _iMtu);
	~CSocketArchiveClient();
	bool connect();
	void disconnect();
	bool isSocketValid();
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> execute(CNsTransmitInfo::tagJournalTransBase *_data,int _i_time_msec);
	template<class TYPE>
	bool execute(TxCppPlatform::shared_ptr<TYPE> *_ret,CNsTransmitInfo::tagJournalTransBase *_data,int _i_time_msec)
	{
		*_ret=TxCppPlatform::dynamic_pointer_cast<TYPE>(this->execute(_data,_i_time_msec));
		return !!(*_ret);
	}
};



