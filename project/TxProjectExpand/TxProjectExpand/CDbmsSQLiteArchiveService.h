#pragma once

#include "CDbSQLiteArchiveServerSocket.h"
#include "CDbSocketSqliteSession.h"

class CDbmsSqliteGlobalFunction
{
public:
	static TxBlobString getCurrentTimeString();
	static const char* getModuleFileName();
};

class CDbmsSQLiteArchiveService: public TxIocpTcpApi::TxIocpService
{
private:
	TxCppPlatform::shared_ptr<CDbSocketSqliteSession> spDbSocketSqliteSession;
	std::map<int,TxCppPlatform::weak_ptr<CDbSQLiteArchiveServerSocket>> mapRecordServerSocket;
	TxSystemDependent::TxMutexWrap m_MapMutex;
	const int iNetworkMtu;
private:
	virtual void onStartThreadEvent();
	virtual void onFinishThreadEvent();
public :
	CDbmsSQLiteArchiveService(int _iThreadCount,int _mtu);
	~CDbmsSQLiteArchiveService();
	inline int getNetworkMtu() const;
	void join();
	bool initClientJournal(const char *_ip,int _port);
	void insertClientJournal(const char *_msgText,const char *_sCodeFile,const char *_sCodeFunc,int _iCodeLine,const char *_msgTime,
		EnumSqliteJournalLevel _eLoglevel);
	bool createDbmsServer(int _port,const char _sSQLiteDbFile[]);
	bool createJournalServer(int _port,const char _sSQLiteDbFile[]);
	void clearDbmsServer();
	void releaseDbmsServer(int _port);
	void triggerStop();
	static void g_executeSqliteClientSession(CSocketArchiveClient *_pThis,const char *_sql,int _i_time_msec,void *_userdata,TSqliteCallBackDllFunc _func);
};

