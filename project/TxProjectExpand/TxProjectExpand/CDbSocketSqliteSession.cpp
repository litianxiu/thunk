#define _Tx_SqliteExpand_dll_OutPut_
#include "CDbSocketSqliteSession.h"

CDbSocketSqliteSession::CDbSocketSqliteSession(TxIocpTcpApi::TxIocpService *_p_svc,int _ciMtu,const char *_ip,int _port)
	:p_svc(_p_svc),strIp(_ip),ciPort(_port),ciMtu(_ciMtu)
{
	this->eThreadRunning=e_thread_null;
	this->m_event.setEvent();
	this->mThread.join();
	//TxCppPlatform::shared_ptr<CDbSQLiteArchiveClient> spLcJournalSession=this->wpJournalSession.lock();
	//if(spLcJournalSession)
	//	spLcJournalSession->close();
}

CDbSocketSqliteSession::~CDbSocketSqliteSession()
{
	this->finish();
}

void CDbSocketSqliteSession::finish()
{
	if(this->eThreadRunning!=e_thread_end)
	{
		this->eThreadRunning=e_thread_end;
		this->m_event.setEvent();
		this->mThread.join();
	}
	//TxIocpCppPlatform::shared_ptr<CDbSQLiteArchiveClient> spLcJournalSession=wpJournalSession.lock();
	//if(spLcJournalSession) spLcJournalSession->close();
}

void CDbSocketSqliteSession::_static_call_back_(void *_arg1,void *_arg2)
{
	(void)_arg2;
	((CDbSocketSqliteSession*)_arg1)->_call_back_();
}

//bool CDbSocketSqliteSession::_send_log_(CNsTransmitInfo::tagRequestInsertJournal *_tem)
//{
//	TxIocpCppPlatform::shared_ptr<CDbSQLiteArchiveClient> spLcJournalSession=this->wpJournalSession.lock();
//	if(spLcJournalSession)
//	{
//		TxIocpCppPlatform::shared_ptr<CNsCommonTransmitInfo::tagRespondInsertJournal> res=spLcJournalSession->addJournal(_tem,3*1000);
//		if(res)
//			return true;
//	}
//	if(spLcJournalSession)
//		spLcJournalSession->close();
//	this->wpJournalSession.reset();
//	spLcJournalSession.reset(new CDbSQLiteArchiveClient(this->p_svc,this->ciMtu));
//	if(spLcJournalSession->blockConnect(this->strIp.c_str(),this->ciPort))
//	{
//		this->wpJournalSession=spLcJournalSession;
//		TxIocpCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondInsertJournal> res=spLcJournalSession->addJournal(_tem,3*1000);
//		if(res)
//			return true;
//	}
//	return false;
//}

void CDbSocketSqliteSession::_call_back_()
{
	CNsTransmitInfo::tagRequestInsertJournal tem;
	long bLcRunThread=1;
	long bLcHaveData=0;
	long bLcLoop=1;
	CSocketArchiveClient mLcJournalSession(this->p_svc,this->strIp.c_str(),this->ciPort,this->ciMtu);
	while(bLcLoop!=0)
	{
		this->m_event.waitEvent(bLcHaveData?(DWORD)10:(DWORD)(INFINITE));
		for(;;)
		{
			this->mutex.lock();
			if(!bLcHaveData)
			{
				if(this->listLogData.size()>0)
				{
					tem=this->listLogData.front();
					this->listLogData.pop_front();
					bLcHaveData=1;
				}
			}
			if(this->eThreadRunning!=e_thread_run)
				bLcRunThread=0;
			this->mutex.unlock();
			//if(bLcHaveData&&_force_send_log_(&tem))
			//{
			//	bLcHaveData=0;
			//}
			if(bLcHaveData&&!!mLcJournalSession.execute(&tem,3*1000))
			{
				bLcHaveData=0;
			}
			else
			{
				if(bLcRunThread==0)
					bLcLoop=0;
				break;
			}
		}
	}
}

void CDbSocketSqliteSession::addLog(const CNsTransmitInfo::tagRequestInsertJournal &_tem)
{
	int bLcSetEvent=0;
	this->mutex.lock();
	if(this->listLogData.size()<=0)
		bLcSetEvent=1;
	this->listLogData.push_back(_tem);
	if(this->listLogData.size()>1024*10)
		this->listLogData.pop_front();
	if(this->eThreadRunning==e_thread_null)
	{
		this->eThreadRunning=e_thread_run;
		this->mThread.create(_static_call_back_,this,NULL);
	}
	this->mutex.unlock();
	if(bLcSetEvent!=0)
		this->m_event.setEvent();
}
