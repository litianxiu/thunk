#define _Tx_SqliteExpand_dll_OutPut_
#include "../common/TxIocpTcpApi.hpp"
#include "../common/TxShellPackage.hpp"
#include "../common/TxBlobString.hpp"
#include "CDbmsSQLiteArchiveService.h"
#include "../include/CSqliteExpandDll.h"
#include "CSocketArchiveClient.h"
#include "CRemotFileClientManager.h"
#include "CRemotFileServerDownLoadManager.h"

struct tagDbmsSQLiteArchiveServiceInstance_
{
	TxSystemDependent::TxMutexWrap mutex;
	CDbmsSQLiteArchiveService *pDbmsSQLiteArchiveService;
	LONG lRefCount;
	tagDbmsSQLiteArchiveServiceInstance_()
	{
		this->pDbmsSQLiteArchiveService=NULL;
		this->lRefCount=0;
	}
	~tagDbmsSQLiteArchiveServiceInstance_()
	{
		DefAssertTextWrap(this->lRefCount==0,"");
		DefAssertTextWrap(this->pDbmsSQLiteArchiveService==NULL,"");
		if(this->pDbmsSQLiteArchiveService!=NULL)
		{
			delete this->pDbmsSQLiteArchiveService;
			this->pDbmsSQLiteArchiveService=NULL;
		}
	}
	static tagDbmsSQLiteArchiveServiceInstance_* getInstance()
	{
		static tagDbmsSQLiteArchiveServiceInstance_ _m_tem;
		return &_m_tem;
	}
	inline CDbmsSQLiteArchiveService* getDbmsSQLiteArchiveService()
	{
		DefAssertTextWrap(this->pDbmsSQLiteArchiveService!=NULL,"");
		return this->pDbmsSQLiteArchiveService;
	}
	void initialize(int _iThreadCount,int _mtu)
	{
		this->mutex.lock();
		if((++this->lRefCount)==1)
		{
			DefAssertTextWrap(this->pDbmsSQLiteArchiveService==NULL,"");
			if(this->pDbmsSQLiteArchiveService!=NULL)
				delete this->pDbmsSQLiteArchiveService;
			this->pDbmsSQLiteArchiveService=new CDbmsSQLiteArchiveService(_iThreadCount,_mtu);
		}
		this->mutex.unlock();
	}
	void uninitialize()
	{
		CDbmsSQLiteArchiveService *ptr=NULL;
		this->mutex.lock();
		if((--this->lRefCount)<=0)
		{
			DefAssertTextWrap(this->lRefCount>=0,"");
			ptr=this->pDbmsSQLiteArchiveService;
			this->lRefCount=0;
			this->pDbmsSQLiteArchiveService=NULL;
		}
		this->mutex.unlock();
		if(ptr)
		{
			delete ptr;
		}
	}
};

void initializeSQLiteSvcEnv(int _iThreadCount,int _mtu)
{
	tagDbmsSQLiteArchiveServiceInstance_::getInstance()->initialize(_iThreadCount,_mtu);
}

void uninitializeSQLiteSvcEnv()
{
	tagDbmsSQLiteArchiveServiceInstance_::getInstance()->uninitialize();
}

size_t createSocketArchiveClient(const char *_ip,int _port)
{
	CDbmsSQLiteArchiveService *g_svc=tagDbmsSQLiteArchiveServiceInstance_::getInstance()->getDbmsSQLiteArchiveService();
	return (size_t)(new CSocketArchiveClient(g_svc,_ip,_port,g_svc->getNetworkMtu()));
}

void releaseSocketArchiveClient(size_t _pThis)
{
	CSocketArchiveClient *pThis=(CSocketArchiveClient*)_pThis;
	delete pThis;
}

void executeSqliteClientSession(size_t _pThis,const char *_sql,int _i_time_msec,void *_userdata,TSqliteCallBackDllFunc _func)
{
	return CDbmsSQLiteArchiveService::g_executeSqliteClientSession((CSocketArchiveClient*)_pThis,_sql,_i_time_msec,_userdata,_func);
}

void executeFindRemoteFileSession(size_t _pThis,const char *_strFilePath,const char *_strWildcard,int _i_time_msec,void *_userdata,TSqliteCallBackDllFunc _cb)
{
	return CRemotFileClientManager::g_findRemoteFile((CSocketArchiveClient*)_pThis,_strFilePath,_strWildcard,_i_time_msec,_cb,_userdata);
}

void executeDeleteRemoteFileSession(size_t _pThis,const char *_strFileBasic,
									const char *_strFileName[],int _count,int _i_time_msec,TSqliteCallBackDllFunc _cb,void *_userdata)
{
	return CRemotFileClientManager::g_deleteRemoteFile((CSocketArchiveClient*)_pThis,_strFileBasic,_strFileName,_count,_i_time_msec,_cb,_userdata);
}

void executeReadRemoteFileSession(size_t _pThis,
								  const char *_strFileUri,int _i_time_msec, long long _pos,int _size,TSqliteCallBackDllFunc _cb,void *_userdata)
{
	return CRemotFileClientManager::g_readRemoteFile((CSocketArchiveClient*)_pThis,
		_strFileUri,_i_time_msec,_pos,_size,_cb,_userdata);
}

void executeWriteRemoteFileSession(size_t _pThis,
								  const char *_strFileUri,int _i_time_msec, const void *_buff,long long _pos,int _size,TSqliteCallBackDllFunc _cb,void *_userdata)
{
	return CRemotFileClientManager::g_writeRemoteFile((CSocketArchiveClient*)_pThis,
		_strFileUri,_i_time_msec,_buff,_pos,_size,_cb,_userdata);
}


//////////////////////////////一下是全局操作
void insertSQLiteClientJournal(const char *_msgText,const char *_sCodeFile,const char *_sCodeFunc,
	int _iCodeLine,const char *_msgTime,EnumSqliteJournalLevel _eLoglevel)
{
	tagDbmsSQLiteArchiveServiceInstance_::getInstance()->getDbmsSQLiteArchiveService()->insertClientJournal(_msgText,_sCodeFile,_sCodeFunc,
		_iCodeLine,_msgTime,_eLoglevel);
}

bool createSqliteDbmsServer(int _port,const char _sSQLiteDbFile[],bool _bIsLog)
{
	if(_bIsLog)
		return tagDbmsSQLiteArchiveServiceInstance_::getInstance()->getDbmsSQLiteArchiveService()->createJournalServer(_port,_sSQLiteDbFile);
	return tagDbmsSQLiteArchiveServiceInstance_::getInstance()->getDbmsSQLiteArchiveService()->createDbmsServer(_port,_sSQLiteDbFile);
}

void releaseSqliteDbmsServer(int _port)
{
	tagDbmsSQLiteArchiveServiceInstance_::getInstance()->getDbmsSQLiteArchiveService()->releaseDbmsServer(_port);
}
