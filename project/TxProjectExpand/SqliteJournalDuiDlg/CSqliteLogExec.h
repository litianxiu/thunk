#pragma once

#include "../common/TxIocpTcpApi.hpp"
#include "../common/TxBlobString.hpp"
#include "../include/CSqliteExpandDll.h"

class CSqliteLogExec
{
//private:
	//static long long CSqliteLogExec::__execNewestLogDataId_(CDbmsClientSessionDll *_pConObj,TxBlobString *_outError);
public :
	static long long CSqliteLogExec::execNewestLogDataId(CDbmsClientSessionDll *_pConObj,TxBlobString *_outError);
	//static TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute> inquireNewestLogData(const char *_ip,int _port,int _log_count);
	//static TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute> executeSql(const char *_ip,int _port,const char *_sql,int _i_time_msec);
	//static TxCppPlatform::shared_ptr<TxDbSQLiteArchiveClientSession> newDbSQLiteArchiveClientSession(const char *_ip,int _port);
};

