#define _Tx_SqliteExpand_dll_OutPut_
#include "CSQLiteDataBaseManager.h"
#include "CDbmsSQLiteArchiveService.h"

class MyWindowsMgrWriteSystemLog_
{
private:
	HANDLE log_handle;
public :
	MyWindowsMgrWriteSystemLog_()
	{
		this->log_handle = RegisterEventSourceA(NULL,
			CDbmsSqliteGlobalFunction::getModuleFileName());
	}
	~MyWindowsMgrWriteSystemLog_()
	{
		if(this->log_handle!=NULL)
		{
			DeregisterEventSource(this->log_handle);
			this->log_handle=NULL;
		}
	}
	void writeSystemLog(const char *_msg)
	{
		if(this->log_handle!=NULL&&_msg!=NULL&&_msg[0]!=0)
		{
			if (!ReportEventA(this->log_handle,	// Event log handle. 
				EVENTLOG_ERROR_TYPE,		// Event type. 
				NULL,						// Event category.  
				FACILITY_NULL,				// Event identifier. 
				NULL,						// No user security identifier. 
				1,							// Number of substitution strings. 
				0,							// No data. 
				&_msg,						// Pointer to strings. 
				NULL))						// No data. 
			{
				printf("error:ReportEventA() failure!\n");
			}
		}
	}
};

struct tagSqliteExecCallBackUserdata_
{
	CNsTransmitInfo::tagRespondSqlExecute *pRespondSqlExecute;
	int iTotalSize;
	int iItemCount;
};

int CSQLiteDataBaseManager::_staticSQLiteExecCallBack( void * para, int n_column, char ** column_value, char ** column_name )
{
	tagSqliteExecCallBackUserdata_ *pLcUserData=(tagSqliteExecCallBackUserdata_*)para;
	pLcUserData->iTotalSize+=pLcUserData->pRespondSqlExecute->push(n_column,column_name,column_value);
	pLcUserData->iItemCount++;
	if(pLcUserData->iTotalSize>10*1024*1024||pLcUserData->iItemCount>=10000+100)
	{
		pLcUserData->pRespondSqlExecute->sErrorText.assign("由于数据量太大，未能完全获取所有数据。");
		return -1;
	}
	return 0;
}

TxBlobString CSQLiteDataBaseManager::_getSqlCallbackElem(const CNsTransmitInfo::tagRespondSqlExecute &_elem,const char *_chUpSimple)
{
	std::list<CNsTransmitInfo::tagRespondSqlExecute::tagGroupInfo>::const_iterator iter=_elem.listGroupInfo.begin(),
		iter_end=_elem.listGroupInfo.end();
	for(;iter!=iter_end;iter++)
	{
		for(int x=(int)iter->vArryItem.size();(--x)>=0;)
		{
			if(iter->vArryItem[x].sName.equalNoCase(_chUpSimple))
				return iter->vArryItem[x].sValue;
		}
	}
	return TxBlobString();
}

void CSQLiteDataBaseManager::_timerDeleteLogLastElem(char **_perrmsg)
{
	CNsTransmitInfo::tagRespondSqlExecute mLcRespondSqlExecute;
	tagSqliteExecCallBackUserdata_ m_lc_tem;
	m_lc_tem.iTotalSize=0;
	m_lc_tem.iItemCount=0;
	m_lc_tem.pRespondSqlExecute=&mLcRespondSqlExecute;
	//if(SQLITE_OK==sqlite3_exec(this->p_db,"select COUNT(*) from journal;",_staticSQLiteExecCallBack,&m_lc_tem,_perrmsg))
	if(this->m_db.execute("select COUNT(*) from journal;",_staticSQLiteExecCallBack,&m_lc_tem,_perrmsg))
	{
		unsigned long lc_count=std::strtoul(_getSqlCallbackElem(mLcRespondSqlExecute,"COUNT(*)").c_str(),NULL,0);
		enum { e_lc_log_item_max_size=(int)(1e5), };
		if(lc_count>e_lc_log_item_max_size*2)
		{
			m_lc_tem.iTotalSize=0;
			m_lc_tem.iItemCount=0;
			mLcRespondSqlExecute.clear();
			//if(SQLITE_OK==sqlite3_exec(this->p_db,"select max(id) from journal;",_staticSQLiteExecCallBack,&m_lc_tem,_perrmsg))
			if(this->m_db.execute("select max(id) from journal;",_staticSQLiteExecCallBack,&m_lc_tem,_perrmsg))
			{
				unsigned long lc_max_id=std::strtoul(_getSqlCallbackElem(mLcRespondSqlExecute,"max(id)").c_str(),NULL,0);
				if(lc_max_id>e_lc_log_item_max_size)
				{
					unsigned long lc_min_id=lc_max_id-e_lc_log_item_max_size;
					if(lc_count>e_lc_log_item_max_size*2)
					{
						char lc_sql_buff_[256];
						sprintf(lc_sql_buff_,"DELETE FROM journal WHERE id < %lld;",(long long)lc_min_id); 
						//sqlite3_exec(this->p_db,lc_sql_buff_,NULL,NULL,_perrmsg);
						this->m_db.execute(lc_sql_buff_,NULL,NULL,_perrmsg);
					}
				}
			}
		}
	}
}

void CSQLiteDataBaseManager::_thread_callback_()
{
	MyWindowsMgrWriteSystemLog_ mLcWindowLog;
	TxSystemDependent::TxTimeSpan<false> mLcRecordTimeSpan;
	mLcRecordTimeSpan.reset(3600*24*1000);
	BOOL bLoopSign=TRUE;
	BOOL blcEnd=FALSE;
	int iLcInsertSumCount=10000000;
	while(bLoopSign)
	{
		if(!blcEnd) this->mEvent.waitEvent(6*1000);
		std::list<TxBlobString> lcData;
		tagSelectInfo mLcListData;
		this->mMutex.lock();
		if(this->listSqlInsertString.size()>=e_list_insert_max_count||blcEnd||mLcRecordTimeSpan.getCurrentMillisecond()>5*1000)
		{
			lcData.swap(this->listSqlInsertString);
			mLcRecordTimeSpan.reset(0);
			iLcInsertSumCount++;
		}
		if(this->listSqlSelectElem.size()>0)
		{
			mLcListData=listSqlSelectElem.front();
			this->listSqlSelectElem.pop_front();
		}
		if(this->bThreadRunning==0)
		{
			blcEnd=TRUE;
			if(lcData.size()==0&&this->listSqlInsertString.size()==0&&mLcListData.wpSocket.expired())
				bLoopSign=0;
		}
		this->mMutex.unlock();
		if(!mLcListData.wpSocket.expired())
		{
			CNsTransmitInfo::tagRespondSqlExecute mLcRespondSqlExecute;
			char *perrmsg=NULL;
			mLcRespondSqlExecute.initBasic(CNsTransmitInfo::e_type_sql_execute_rpd);

			tagSqliteExecCallBackUserdata_ m_lc_tem;
			m_lc_tem.iTotalSize=0;
			m_lc_tem.iItemCount=0;
			m_lc_tem.pRespondSqlExecute=&mLcRespondSqlExecute;

			bool bLcGood=(this->m_db.execute(mLcListData.sql.c_str(),_staticSQLiteExecCallBack,&m_lc_tem,&perrmsg)); (void)bLcGood;
			if(perrmsg!=NULL) mLcRespondSqlExecute.sErrorText=perrmsg;
			TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> spSocket(mLcListData.wpSocket.lock());
			if(spSocket)
			{
				CNsTransmitInfo::CShellPackData::pack_and_send(spSocket,&mLcRespondSqlExecute,mLcListData.numPkg);
			}
			if(!bLcGood)
			{
				mLcWindowLog.writeSystemLog(perrmsg);
			}
		}
		if(lcData.size()>0)
		{
			TxBlobString lc_sql;
			if(lcData.size()==1)
			{
				lc_sql=lcData.front();
			}
			else
			{
				lc_sql+="begin;\n";
				std::list<TxBlobString>::iterator iter=lcData.begin();
				std::list<TxBlobString>::iterator iter_end=lcData.end();
				for(;iter!=iter_end;iter++)
				{
					lc_sql+=(*iter);
					lc_sql+=";\n";
				}
				lc_sql+="commit;\n";
			}
			assert(this->m_db.isValid());
			char *perrmsg=NULL;
			//if(SQLITE_OK!=sqlite3_exec(this->p_db,lc_sql.c_str(), NULL,NULL,&perrmsg))
			if(this->m_db.execute(lc_sql.c_str(), NULL,NULL,&perrmsg))
			{
				mLcWindowLog.writeSystemLog(perrmsg);
			}
		}
		if(iLcInsertSumCount>30)
		{
			iLcInsertSumCount=0;
			char *perrmsg=NULL;
			this->_timerDeleteLogLastElem(&perrmsg);
			if(perrmsg!=NULL)
				mLcWindowLog.writeSystemLog(perrmsg);
		}
	}
}

void CSQLiteDataBaseManager::_static_thread_callback_(void *_arg1,void *_arg2)
{
	(void)_arg2;
	((CSQLiteDataBaseManager*)_arg1)->_thread_callback_();
}

CSQLiteDataBaseManager::CSQLiteDataBaseManager()
{
}

CSQLiteDataBaseManager::~CSQLiteDataBaseManager()
{
	this->close();
}

void CSQLiteDataBaseManager::close()
{
	if(this->m_db.isValid())
	{
		this->mMutex.lock();
		this->bThreadRunning=0;
		this->mEvent.setEvent();
		this->mMutex.unlock();
		this->mThread.join();
		this->mMutex.lock();
		this->m_db.close();
		this->mMutex.unlock();
	}
}

bool CSQLiteDataBaseManager::open(const char *_db_file)
{
	bool ret=false;
	this->mMutex.lock();
	//if(SQLITE_OK==sqlite3_open(_db_file,&this->p_db)
	//	||SQLITE_OK==sqlite3_exec(this->p_db,"PRAGMA synchronous = OFF; PRAGMA temp_store = MEMORY; ", 0,0,0))
	if(this->m_db.open(_db_file)
		||this->m_db.execute("PRAGMA synchronous = OFF; PRAGMA temp_store = MEMORY; ",NULL,NULL,NULL))
	{
		this->bThreadRunning=1;
		this->mThread.create(_static_thread_callback_,this,NULL);
		ret=true;
	}
	this->mMutex.unlock();
	if(!ret)
		this->close();
	return ret;
}

void CSQLiteDataBaseManager::execute_sql(const CNsTransmitInfo::tagRequestSqlExecute &_m_data,
	TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,unsigned int _num_pkg)
{
	tagSelectInfo _tem;
	_tem.numPkg=_num_pkg;
	_tem.sql=_m_data.sql;
	_tem.wpSocket=_spSocket;
	this->mMutex.lock();
	this->listSqlSelectElem.push_back(_tem);
	this->mMutex.unlock();
	this->mEvent.setEvent();
}

void CSQLiteDataBaseManager::_addInsertJournal__(const CNsTransmitInfo::tagRequestInsertJournal &_m_data)
{
	//INSERT INTO Persons (LastName, Address) VALUES ('Wilson', 'Champs-Elysees')
	char chsLcLine[64],chsLogLevel[64],chsLogRand[64];
	sprintf(chsLcLine,"%u",(int)_m_data.iCodeLine);
	sprintf(chsLogLevel,"%u",(int)_m_data.eLogLevel);
	sprintf(chsLogRand,"%u",(int)_m_data.uiRand);
	//											char[]	char[]	char[]	int	char[]	long long long long long long
	TxBlobString str_sql="INSERT INTO journal (message,program_name,src_file,function,line,msg_time,os_time,svr_time,log_level,rand) VALUES (\'";
	str_sql+=_m_data.msgText;
	str_sql+="\',\'";
	str_sql+=_m_data.sProgramName;
	str_sql+="\',\'";
	str_sql+=_m_data.sCodeFile;
	str_sql+="\',\'";
	str_sql+=_m_data.sCodeFunc;
	str_sql+="\',";
	str_sql+=chsLcLine;
	str_sql+=",\'";
	str_sql+=_m_data.msgTime;
	str_sql+="\',\'";
	str_sql+=_m_data.osTime;
	str_sql+="\',\'";
	str_sql+=CDbmsSqliteGlobalFunction::getCurrentTimeString();
	str_sql+="\',";
	str_sql+=chsLogLevel;
	str_sql+=",";
	str_sql+=chsLogRand;
	str_sql+=")";
	this->mMutex.lock();
	this->listSqlInsertString.push_back(str_sql);
	int iLcListInsertCount=(int)this->listSqlInsertString.size();
	this->mMutex.unlock();
	if(iLcListInsertCount>e_list_insert_max_count)
		this->mEvent.setEvent();
}

void CSQLiteDataBaseManager::dealInsertJournal(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,
	CNsTransmitInfo::tagJournalTransBase *_data,unsigned int _uiPkgNum)
{
	CNsTransmitInfo::tagRequestInsertJournal *pLcData=_data->getOrgThisPointer<CNsTransmitInfo::tagRequestInsertJournal>();
	CNsTransmitInfo::tagRespondInsertJournal mLcRespondInsertJournal;
	mLcRespondInsertJournal.initBasic(CNsTransmitInfo::e_type_insert_journal_rpd);
	mLcRespondInsertJournal.set("");
	CNsTransmitInfo::CShellPackData::pack_and_send(_sp,&mLcRespondInsertJournal,_uiPkgNum);
	this->_addInsertJournal__(*pLcData);
}

