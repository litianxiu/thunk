#pragma once

#include <list>
#include <map>
#include <string>
#include "../common/TxBlobString.hpp"
#include "../common/TxIocpTcpApi.hpp"
#include "../common/TxSystemDependent.hpp"
#include "CNsCommonTransmitInfo.hpp"
#include "CNsRemoteFileTransmitInfo.hpp"
#include "CNsTransmitShellPackInfo.h"
#include "CSQLiteDataBaseManager.h"
#include "CRemotFileServerManager.h"
#include "CNsRemoteFileTransmitInfo.hpp"

class CDbSQLiteArchiveTcpPeerSocket: public TxIocpTcpApi::TxIocpTcpPeerSocket
{
public :
	std::list<TxCppPlatform::weak_ptr<TxIocpTcpApi::TxIocpSocketBasic>>::iterator m_iter;
	BOOL b_iter;
	LONG bDoSomeThing[2];
	CNsTransmitInfo::CShellPackData m_ShellPackage;
	CSQLiteDataBaseManager *const pSqlDataBaseMgr;
public:
	CDbSQLiteArchiveTcpPeerSocket(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spServerSocket,int _mtu,CSQLiteDataBaseManager *_pSqlDataBaseMgr);
	~CDbSQLiteArchiveTcpPeerSocket();
	void pushData(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,const unsigned char *_data,int _size);
};

class CDbSQLiteArchiveServerSocket: public TxIocpTcpApi::TxIocpTcpServerSocket
{
public :
	TxSystemDependent::TxMutexWrap listPeerSocket_Mutex;
	CSQLiteDataBaseManager mSqlDataBaseMgr;
	std::list<TxCppPlatform::weak_ptr<TxIocpTcpApi::TxIocpSocketBasic>> listPeerSocket;
	const int mtu;
	BOOL bValid;
	TxSystemDependent::TxTimeSpan<true> mRecordTimeSpan;
public :
	CDbSQLiteArchiveServerSocket(TxIocpTcpApi::TxIocpService *_svc,int _mtu);
	~CDbSQLiteArchiveServerSocket();
	bool listen(int _port,const char *_sSQLiteDbFile);
	virtual TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> onNewSocketEvent();
	void close();
	virtual bool onAcceptPeerSocketEvent(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,const SOCKADDR_IN *_addr);
	virtual bool onPeerSocketReadExceptionEvent(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp);
	virtual bool onPeerSocketReadBufferEvent(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,const unsigned char *_data,int _size);
	virtual bool onPeerSocketWriteBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,int _iSurplusSize);
};

