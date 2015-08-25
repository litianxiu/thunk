#pragma once

#include <Windows.h>
#include <string.h>
#include <stdlib.h>

enum EnumSqliteJournalLevel
{
	eSqliteJournalLevelNull=0,
	eSqliteJournalLevelInfo,
	eSqliteJournalLevelWarn,
	eSqliteJournalLevelError,
	eSqliteJournalLevelFatal,
};

enum EnumFilePermissionMode
{
	eFilePermissionMode_Null=0,
	eFilePermissionMode_Read=1,
	eFilePermissionMode_Write=2,
	eFilePermissionMode_Exec=4,
};

typedef void CALLBACK TSqliteCallBackDllFunc(void *_userdata,const void *_data);

#pragma pack(push)//保存对齐状态
#pragma pack(8)//设定为4字节对齐

struct tagSqliteExecuteRespondData
{
	const char **name;
	const char **value;
	int iColCount;
	int iCurRow;//-1代表错误，错误字符串保存在value里面
	int iRowCount;
	void reset() { ::memset(this,0,sizeof(*this)); }
};
struct tagFindRemotFileRespondData
{
	const char *filename;
	long long filesize;//-1代表文件夹
	long long time;
	EnumFilePermissionMode permission;
	const char *strFilePath;
	const char *strWildcard;
	const char *strErrorText;
	int iCurRow;//-1代表错误，错误字符串保存在value里面
	int iTotalCount;
	void reset() { ::memset(this,0,sizeof(*this)); }
};

struct tagRemoteFileReadRespondData
{
	long long llTotalSize;
	const char *error;
	const unsigned char *data;
	int size;
};
	
struct tagRemoteFileWriteRespondData
{
	const char *error;
};

//struct tagDownLoadServerFileRespondInfo
//{
//	long long llTotalSize;
//	long long llGainSize;
//	const void *data;
//	int size;
//	const char *error;//如果该参数不为空NULL，其他参数一律无效。
//};
//
//struct tagDownLoadServerFileRespondData
//{
//	enum
//	{
//		e_op_null=0,
//		e_op_init,
//		e_op_finish,
//		e_op_connect,
//		e_op_error,
//		e_op_exit,
//	} cmd;
//	union
//	{
//		struct
//		{
//			long long llFileSize;
//		} mInit;
//		struct
//		{
//		} mFinish;
//		struct
//		{
//		} mExitThread;
//		struct
//		{
//			int bSuccess;
//		} mConnect;
//		struct
//		{
//			const char *sErrorText;
//		} mError;
//	};
//};

#pragma pack(pop)//恢复对齐状态

#ifndef _Tx_SqliteExpand_dll_OutPut_
#pragma comment(lib,"../bin/TxSqliteExpand.lib")
#define TX_DLL_TYPE extern "C" __declspec(dllimport)
#else
#define TX_DLL_TYPE extern "C" __declspec(dllexport)
#endif

TX_DLL_TYPE void initializeSQLiteSvcEnv(int _iThreadCount,int _mtu);
TX_DLL_TYPE void uninitializeSQLiteSvcEnv();
TX_DLL_TYPE size_t createSocketArchiveClient(const char *_ip,int _port);
TX_DLL_TYPE void releaseSocketArchiveClient(size_t _pThis);
TX_DLL_TYPE void executeSqliteClientSession(size_t _pThis,const char *_sql,int _i_time_msec,void *_userdata,TSqliteCallBackDllFunc _func);
TX_DLL_TYPE void executeFindRemoteFileSession(size_t _pThis,
											  const char *_strFilePath,const char *_strWildcard,int _i_time_msec,void *_userdata,TSqliteCallBackDllFunc _cb);
TX_DLL_TYPE void executeDeleteRemoteFileSession(size_t _pThis,const char *_strFileBasic,
												const char *_strFileName[],int _count,int _i_time_msec,TSqliteCallBackDllFunc _cb,void *_userdata);
TX_DLL_TYPE void executeReadRemoteFileSession(size_t _pThis,
											  const char *_strFileUri,int _i_time_msec, long long _pos,int _size,TSqliteCallBackDllFunc _cb,void *_userdata);
TX_DLL_TYPE void executeWriteRemoteFileSession(size_t _pThis,
											   const char *_strFileUri,int _i_time_msec, const void *_buff,long long _pos,int _size,TSqliteCallBackDllFunc _cb,void *_userdata);

TX_DLL_TYPE void insertSQLiteClientJournal(const char *_msgText,const char *_sCodeFile,const char *_sCodeFunc,
	int _iCodeLine,const char *_msgTime,EnumSqliteJournalLevel _eLoglevel);
TX_DLL_TYPE bool createSqliteDbmsServer(int _port,const char _sSQLiteDbFile[],bool _bIsLog);
TX_DLL_TYPE void releaseSqliteDbmsServer(int _port);

#undef TX_DLL_TYPE


class CDbmsClientSessionDll
{
private:
	size_t pThis;
private:
	CDbmsClientSessionDll(const CDbmsClientSessionDll&) { }
	CDbmsClientSessionDll& operator=(const CDbmsClientSessionDll&) const { return *(CDbmsClientSessionDll*)NULL; }
public :
	CDbmsClientSessionDll(const char *_ip,int _port)
	{
		this->pThis=::createSocketArchiveClient(_ip,_port);
	}
	~CDbmsClientSessionDll()
	{
		::releaseSocketArchiveClient(this->pThis);
	}
	void executeSql(const char *_sql,int _i_time_msec,void *_userdata,TSqliteCallBackDllFunc _func)
	{
		::executeSqliteClientSession(this->pThis,_sql,_i_time_msec,_userdata,_func);
	}
	void executeFindRemoteFile(
		const char *_strFilePath,const char *_strWildcard,int _i_time_msec,void *_userdata,TSqliteCallBackDllFunc _cb)
	{
		return ::executeFindRemoteFileSession(this->pThis,_strFilePath,_strWildcard,_i_time_msec,_userdata,_cb);
	}
	void executeDeleteRemoteFile(const char *_strFileBasic,
		const char *_strFileName[],int _count,int _i_time_msec,TSqliteCallBackDllFunc _cb,void *_userdata)
	{
		return ::executeDeleteRemoteFileSession(this->pThis,_strFileBasic,_strFileName,_count,_i_time_msec,_cb,_userdata);
	}
	void executeReadRemoteFile(const char *_strFileUri,int _i_time_msec,long long _pos,int _size,
		void *_userdata,TSqliteCallBackDllFunc _cb)
	{
		return ::executeReadRemoteFileSession(this->pThis,_strFileUri,_i_time_msec,_pos,_size,_cb,_userdata);
	}
	void executeWriteRemoteFile(const char *_strFileUri,int _i_time_msec,const void *_buff,long long _pos,int _size,
		void *_userdata,TSqliteCallBackDllFunc _cb)
	{
		return ::executeWriteRemoteFileSession(this->pThis,_strFileUri,_i_time_msec,_buff,_pos,_size,_cb,_userdata);
	}
};

class TxSqliteDbmsService
{
public :
	static void initializeSQLiteServiceEnv(int _iThreadCount,int _mtu)
	{
		::initializeSQLiteSvcEnv(_iThreadCount,_mtu);
	}
	static void uninitializeSQLiteServiceEnv()
	{
		::uninitializeSQLiteSvcEnv();
	}
	static void insertJournal(const char *_msgText,const char *_sCodeFile,const char *_sCodeFunc,
		int _iCodeLine,const char *_msgTime,EnumSqliteJournalLevel _eLoglevel)
	{
		return ::insertSQLiteClientJournal(_msgText,_sCodeFile,_sCodeFunc,_iCodeLine,_msgTime,_eLoglevel);
	}
	static bool createDbmsServer(int _port,const char _sSQLiteDbFile[],bool _bIsLog)
	{
		return ::createSqliteDbmsServer(_port,_sSQLiteDbFile,_bIsLog);
	}
	static void releaseDbmsServer(int _port)
	{
		return ::releaseSqliteDbmsServer(_port);
	}
};
