#define _Tx_SqliteExpand_dll_OutPut_
#include "CDbmsSQLiteArchiveService.h"
#include "CDbSocketSqliteSession.h"
#include <random>

class MyGlobalMt19937Rand
{
private:
	TxCppPlatform::mt19937 m_generator;
	TxSystemDependent::TxMutexWrap mutex;
	MyGlobalMt19937Rand():m_generator(::GetTickCount()) { }
	unsigned int _getRandValue()
	{
		this->mutex.lock();
		unsigned int ret=(unsigned int)m_generator();
		this->mutex.unlock();
		return ret;
	}
public:
	static unsigned int getRandValue()
	{
		static MyGlobalMt19937Rand _tem__;
		return _tem__._getRandValue();
	}
};

class MyGetModuleFile_Name_
{
private:
	char softName[FILENAME_MAX+sizeof(long)];
public:
	MyGetModuleFile_Name_()
	{
		char szPath[FILENAME_MAX+sizeof(long)]={0};
		GetModuleFileNameA(NULL,szPath,FILENAME_MAX);
		int pos=0;
		for(int i=0;szPath[i]!=NULL;i++)
		{
			if(szPath[i]=='/'||szPath[i]=='\\')
				pos=i+1;
		}
		memset(this->softName,0,sizeof(this->softName));
		strcpy_s(this->softName,&szPath[pos]);
	}
	static const char* MyGetModuleFile_Name_::staticGetModuleFileName()
	{
		static MyGetModuleFile_Name_ _tem__;
		return _tem__.softName;
	}
};

class MyGetModuleFileName_INIT
{
public:
	MyGetModuleFileName_INIT()
	{
		MyGetModuleFile_Name_::staticGetModuleFileName();
	}
} m_MyGetModuleFileName_INIT;

TxBlobString CDbmsSqliteGlobalFunction::getCurrentTimeString()
{
	TxBlobString ret;
	time_t lc_t=0;
	::time(&lc_t);
	struct tm lc_tm={0};
	localtime_s(&lc_tm,&lc_t);
	ret.format("%04d%02d%02d%02d%02d%02d",
		(int)(lc_tm.tm_year+1900),
		(int)(lc_tm.tm_mon+1),
		(int)(lc_tm.tm_mday),
		(int)(lc_tm.tm_hour),
		(int)(lc_tm.tm_min),
		(int)(lc_tm.tm_sec));
	return ret;
}

const char* CDbmsSqliteGlobalFunction::getModuleFileName()
{
	return MyGetModuleFile_Name_::staticGetModuleFileName();
}

CDbmsSQLiteArchiveService::CDbmsSQLiteArchiveService(int _iThreadCount,int _mtu):iNetworkMtu(_mtu)
{
	CDbmsSqliteGlobalFunction::getModuleFileName();
	if(_iThreadCount<=0) _iThreadCount=4;
	TxIocpTcpApi::TxIocpService::start(_iThreadCount);
}

CDbmsSQLiteArchiveService::~CDbmsSQLiteArchiveService()
{
	this->triggerStop();
	this->join();
}

void CDbmsSQLiteArchiveService::onStartThreadEvent()
{
	this->getCurrentThreadIndex();
}

void CDbmsSQLiteArchiveService::onFinishThreadEvent()
{
}

inline int CDbmsSQLiteArchiveService::getNetworkMtu() const
{
	return (this->iNetworkMtu<64||this->iNetworkMtu>1024*1024)?1440:this->iNetworkMtu;
}

void CDbmsSQLiteArchiveService::join() { __super::join(); }

bool CDbmsSQLiteArchiveService::createDbmsServer(int _port,const char _sSQLiteDbFile[])
{
	TxCppPlatform::shared_ptr<CDbSQLiteArchiveServerSocket> sp(new CDbSQLiteArchiveServerSocket(this,this->getNetworkMtu()));
	if(!sp->listen(_port,_sSQLiteDbFile))
	{
		sp->close();
		return false;
	}
	this->m_MapMutex.lock();
	this->mapRecordServerSocket[_port]=TxCppPlatform::weak_ptr<CDbSQLiteArchiveServerSocket>(sp);
	this->m_MapMutex.unlock();
	return true;
}

bool CDbmsSQLiteArchiveService::createJournalServer(int _port,const char _sSQLiteDbFile[])
{
	bool bGood=false;
	if(this->createDbmsServer(_port,_sSQLiteDbFile))
	{
		CSocketArchiveClient mLcClient6000(this,"127.0.0.1",_port,this->iNetworkMtu);
		enum { e_delay_time=3000, };

		CNsTransmitInfo::tagRequestSqlExecute lc_arg;
		lc_arg.initBasic(CNsTransmitInfo::e_type_sql_execute_rqt);
		lc_arg.set("SELECT * FROM journal LIMIT 1;");
		TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute> lc_res;
		mLcClient6000.execute(&lc_res,&lc_arg,e_delay_time);
		if(lc_res)
		{
			if(lc_res->sErrorText.length()==0)
				bGood=true;
			else
			{
				CNsTransmitInfo::tagRequestSqlExecute lc_arg2;
				lc_arg2.initBasic(CNsTransmitInfo::e_type_sql_execute_rqt);
				lc_arg2.set(
					"CREATE TABLE journal("
					"	id				INTEGER PRIMARY KEY	AUTOINCREMENT,"
					"	message			TEXT				NOT NULL,"
					"	program_name	TEXT,"
					"	src_file		TEXT,"
					"	function		CHAR(50),"
					"	line			INT,"
					"	msg_time		TEXT,"
					"	os_time			TEXT,"
					"	svr_time		TEXT,"
					"	log_level		INT,"
					"	rand			INT"
					"	);"
					);
				mLcClient6000.execute(&lc_res,&lc_arg2,e_delay_time);
				if(lc_res&&lc_res->sErrorText.length()==0)
					bGood=true;
			}
		}
	}
	if(!bGood)
		this->releaseDbmsServer(_port);
	return bGood;
}

void CDbmsSQLiteArchiveService::clearDbmsServer()
{
	std::list<TxCppPlatform::shared_ptr<CDbSQLiteArchiveServerSocket>> lcListServer;
	{
		this->m_MapMutex.lock();
		std::map<int,TxCppPlatform::weak_ptr<CDbSQLiteArchiveServerSocket>>::iterator iter=this->mapRecordServerSocket.begin();
		std::map<int,TxCppPlatform::weak_ptr<CDbSQLiteArchiveServerSocket>>::iterator iter_end=this->mapRecordServerSocket.end();
		for(;iter!=iter_end;iter++)
			lcListServer.push_back(iter->second.lock());
		this->mapRecordServerSocket.clear();
		this->m_MapMutex.unlock();
	}
	for(std::list<TxCppPlatform::shared_ptr<CDbSQLiteArchiveServerSocket>>::iterator iter=lcListServer.begin();
		iter!=lcListServer.end();iter++)
	{
		if((*iter))
			(*iter)->close();
	}
}

void CDbmsSQLiteArchiveService::releaseDbmsServer(int _port)
{
	TxCppPlatform::shared_ptr<CDbSQLiteArchiveServerSocket> spLcServer;
	this->m_MapMutex.lock();
	std::map<int,TxCppPlatform::weak_ptr<CDbSQLiteArchiveServerSocket>>::iterator iter=this->mapRecordServerSocket.find(_port);
	if(iter!=this->mapRecordServerSocket.end())
	{
		spLcServer=iter->second.lock();
		this->mapRecordServerSocket.erase(iter);
	}
	this->m_MapMutex.unlock();
	if(spLcServer)
		spLcServer->close();
}

void CDbmsSQLiteArchiveService::triggerStop()
{
	if(this->spDbSocketSqliteSession)
	{
		this->spDbSocketSqliteSession->finish();
		this->spDbSocketSqliteSession.reset();
	}
	this->clearDbmsServer();
	__super::triggerStop();
}

bool CDbmsSQLiteArchiveService::initClientJournal(const char *_ip,int _port)
{
	if(!this->spDbSocketSqliteSession)
	{
		this->spDbSocketSqliteSession.reset(new CDbSocketSqliteSession(this,this->getNetworkMtu(),_ip,_port));
		return true;
	}
	return false;
}

void CDbmsSQLiteArchiveService::insertClientJournal(
	const char *_msgText,const char *_sCodeFile,const char *_sCodeFunc,int _iCodeLine,const char * _msgTime,
	EnumSqliteJournalLevel _eLoglevel)
{
	if(this->spDbSocketSqliteSession)
	{
		CNsTransmitInfo::tagRequestInsertJournal tem;
		tem.initBasic(CNsTransmitInfo::e_type_insert_journal_rqt);
		tem.set(_msgText,CDbmsSqliteGlobalFunction::getModuleFileName(),_sCodeFile,_sCodeFunc,_iCodeLine,_msgTime,
			CDbmsSqliteGlobalFunction::getCurrentTimeString().c_str(),_eLoglevel,
			MyGlobalMt19937Rand::getRandValue()>>1);
		this->spDbSocketSqliteSession->addLog(tem);
	}
}

void CDbmsSQLiteArchiveService::g_executeSqliteClientSession(CSocketArchiveClient *_pThis,const char *_sql,int _i_time_msec,void *_userdata,TSqliteCallBackDllFunc _func)
{
	CNsTransmitInfo::tagRequestSqlExecute lc_arg;
	lc_arg.initBasic(CNsTransmitInfo::e_type_sql_execute_rqt);
	lc_arg.set(_sql);

	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute> lc_data;
	_pThis->execute(&lc_data,&lc_arg,_i_time_msec);
	if(!lc_data)
	{
		tagSqliteExecuteRespondData rrr={0};
		rrr.iRowCount=(int)0;
		rrr.iCurRow=-1;
		rrr.iColCount=1;
		const char *pLcErrorName="error";
		const char *pLcErrorValue="无法从服务器获取数据";
		rrr.name=&pLcErrorName;
		rrr.value=&pLcErrorValue;
		_func(_userdata,&rrr);
	}
	else
	{
		if(lc_data->sErrorText.length()>0)
		{
			tagSqliteExecuteRespondData rrr={0};
			rrr.iRowCount=(int)lc_data->listGroupInfo.size();
			rrr.iCurRow=-1;
			rrr.iColCount=1;
			const char *pLcErrorName="error";
			const char *pLcErrorValue=lc_data->sErrorText.c_str();
			rrr.name=&pLcErrorName;
			rrr.value=&pLcErrorValue;
			_func(_userdata,&rrr);
		}
		std::list<CNsTransmitInfo::tagRespondSqlExecute::tagGroupInfo>::iterator iter=lc_data->listGroupInfo.begin(),
			iter_end=lc_data->listGroupInfo.end();
		std::vector<const char*> vvv;
		const char *lc_arry[2][512];
		tagSqliteExecuteRespondData rrr={0};
		rrr.name=lc_arry[0];
		rrr.value=lc_arry[1];
		rrr.iCurRow=0;
		rrr.iRowCount=(int)lc_data->listGroupInfo.size();
		for(;iter!=iter_end;iter++)
		{
			CNsTransmitInfo::tagRespondSqlExecute::tagGroupInfo::tagItemInfo *pLcArryItem=&iter->vArryItem.front();
			rrr.iColCount=(int)iter->vArryItem.size();
			if(rrr.iColCount>sizeof(lc_arry[0])/sizeof(lc_arry[0][0]))
			{
				vvv.resize(rrr.iColCount*2);
				rrr.name=&vvv.front();
				rrr.value=&vvv.front()+rrr.iColCount;
			}
			for(int i=(int)iter->vArryItem.size();i--;)
			{
				rrr.name[i]=pLcArryItem[i].sName.c_str();
				rrr.value[i]=pLcArryItem[i].sValue.c_str();
			}
			_func(_userdata,&rrr);
			rrr.iCurRow++;
		}
	}
}

