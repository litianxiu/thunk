#pragma once

#include <list>
#include "../common/TxIocpTcpApi.hpp"
#include "CNsCommonTransmitInfo.hpp"
#include "CNsRemoteFileTransmitInfo.hpp"
#include "CNsTransmitShellPackInfo.h"
#include "../include/CSqliteWrapDll.h"

class CSQLiteDataBaseManager
{
private:
	long __fdsfds___0;
	long bThreadRunning;
	long __fdsfds___1;
	CSqliteWrapDll m_db;
	enum { e_list_insert_max_count=1000, };
	TxSystemDependent::TxThreadWrap mThread;
	TxSystemDependent::TxEventWrap mEvent;
	TxSystemDependent::TxMutexWrap mMutex;
	struct tagSelectInfo
	{
		TxBlobString sql;
		TxCppPlatform::weak_ptr<TxIocpTcpApi::TxIocpSocketBasic> wpSocket;
		unsigned int numPkg;
	};
	std::list<TxBlobString> listSqlInsertString;
	std::list<tagSelectInfo> listSqlSelectElem;
private:
	void _gWriteSystemLog(const char *_msg);
	static int __stdcall _staticSQLiteExecCallBack( void * para, int n_column, char ** column_value, char ** column_name );
	void _thread_callback_();
	static void _static_thread_callback_(void *_arg1,void *_arg2);
	static TxBlobString _getSqlCallbackElem(const CNsTransmitInfo::tagRespondSqlExecute &_elem,const char *_chUpSimple);
	void _timerDeleteLogLastElem(char **_perrmsg);
	void _addInsertJournal__(const CNsTransmitInfo::tagRequestInsertJournal &_m_data);
public :
	CSQLiteDataBaseManager();
	~CSQLiteDataBaseManager();
	void close();
	bool open(const char *_db_file);
	void execute_sql(const CNsTransmitInfo::tagRequestSqlExecute &_m_data,
		TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,unsigned int _num_pkg);
	void dealInsertJournal(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,
		CNsTransmitInfo::tagJournalTransBase *_data,unsigned int _uiPkgNum);
};
