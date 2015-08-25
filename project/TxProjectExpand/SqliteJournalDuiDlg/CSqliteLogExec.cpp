#include "stdafx.h"
#include "CSqliteLogExec.h"

long long CSqliteLogExec::execNewestLogDataId(CDbmsClientSessionDll *_pConObj,TxBlobString *_outError)
{
	struct T_R
	{
		long long ll_ret;
		TxBlobString *pError;
	public:
		static void CALLBACK g_call_back(void *_userdata,const void *_data)
		{
			T_R *pThis=(T_R*)_userdata;
			tagSqliteExecuteRespondData *p_data=(tagSqliteExecuteRespondData*)_data;
			if(p_data->iCurRow<0)
			{
				if(p_data->iColCount==1&&pThis->pError!=NULL)
					pThis->pError->assign(p_data->value[0]);
			}
			else
			{
				for(int i=p_data->iColCount;i--;)
				{
					if(strcmp(p_data->name[i],"seq")==0
						&&(pThis->ll_ret=TxBlobString(p_data->value[i]).toDatumRet<long long>(-1))>0)
					{
						break;
					}
				}
			}
		}
	};
	T_R tem;
	tem.ll_ret=-1;
	tem.pError=_outError;
	_pConObj->executeSql("select seq from sqlite_sequence where name = \'journal\'",3*1000,&tem,&T_R::g_call_back);
	return tem.ll_ret;
}

//long long CSqliteLogExec::execNewestLogDataId(const char *_ip,int _port,TxBlobString *_outError)
//{
//	CDbmsClientSessionDll mConObj(_ip,_port);
//	return __execNewestLogDataId_(&mConObj,_outError);
//}
//
//TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute> CSqliteLogExec::inquireNewestLogData(const char *_ip,int _port,int _log_count)
//{
//	TxDbSQLiteArchiveClientSession mConObj(CDbmsSQLiteArchiveService::getInstance(),_ip,_port);
//	long long ll_id=__execNewestLogDataId_(&mConObj);
//	if(ll_id<0) return TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute>();
//	return mConObj.force_execute_sql((TxBlobString("select * from journal where id>")
//		+TxBlobString::fromDatum(ll_id-_log_count)
//		+" limit "+TxBlobString::fromDatum(_log_count+100)+";").c_str(),30*1000);
//}
//
//TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute> CSqliteLogExec::executeSql(const char *_ip,int _port,const char *_sql,int _i_time_msec)
//{
//	TxDbSQLiteArchiveClientSession mConObj(CDbmsSQLiteArchiveService::getInstance(),_ip,_port);
//	return mConObj.force_execute_sql(_sql,_i_time_msec);
//}
//
//TxCppPlatform::shared_ptr<TxDbSQLiteArchiveClientSession> CSqliteLogExec::newDbSQLiteArchiveClientSession(const char *_ip,int _port)
//{
//	return TxCppPlatform::shared_ptr<TxDbSQLiteArchiveClientSession>(new TxDbSQLiteArchiveClientSession(CDbmsSQLiteArchiveService::getInstance(),_ip,_port));
//}
//
