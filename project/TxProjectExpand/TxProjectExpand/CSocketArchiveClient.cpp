#define _Tx_SqliteExpand_dll_OutPut_
#include "CSocketArchiveClient.h"
#include <windows.h>
#include <string.h>

MySocketArchiveClientR::MySocketArchiveClientR(TxIocpTcpApi::TxIocpService *_pTxIocpService,int _mtu)
	: TxIocpTcpApi::TxIocpTcpClientSocket(_pTxIocpService,_mtu)
{
	this->bZoeticSign=FALSE;
	this->bExecReadOnceSign=FALSE;
	this->uiRecordCurrentPackageNumber=0x0f0f;
}

MySocketArchiveClientR::~MySocketArchiveClientR()
{
	this->bZoeticSign=FALSE;
}

bool MySocketArchiveClientR::onClientConnectEvent(bool _bResult)
{
	this->mWpReadEventMutex.lock();
	this->bZoeticSign=!!_bResult;
	TxCppPlatform::shared_ptr<MyDbSQLiteArchiveTcpClientConnectCallBackInfo_> spLcEvent=this->wpReadEvent.lock();
	if(_bResult&&spLcEvent) spLcEvent->uiUserData=1;
	this->mWpReadEventMutex.unlock();
	if(spLcEvent) spLcEvent->m_event.setEvent();
	return true;
}

bool MySocketArchiveClientR::onClientReadExceptionEvent()
{
	this->mWpReadEventMutex.lock();
	this->bZoeticSign=FALSE;
	TxCppPlatform::shared_ptr<MyDbSQLiteArchiveTcpClientConnectCallBackInfo_> spLcEvent=this->wpReadEvent.lock();
	if(spLcEvent) spLcEvent->uiUserData=0;
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> __spArchiveDocTem_=this->spReadPkgObj; (void)&__spArchiveDocTem_;
	this->spReadPkgObj.reset();
	this->mWpReadEventMutex.unlock();
	if(spLcEvent) spLcEvent->m_event.setEvent();
	return true;
}

bool MySocketArchiveClientR::onClientReadBufferEvent(const unsigned char *_data,int _size)
{
	this->bExecReadOnceSign=TRUE;
	this->m_ClientShellPackData.pushData(_data,_size);
	for(;;)
	{
		unsigned int uiLcPkgNum=(this->uiRecordCurrentPackageNumber^0x55555555);
		TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> spLcPkgObj=this->m_ClientShellPackData.popResult(&uiLcPkgNum);
		if(!spLcPkgObj) break;
		TxCppPlatform::shared_ptr<MyDbSQLiteArchiveTcpClientConnectCallBackInfo_> sp;
		this->mWpReadEventMutex.lock();
		sp=this->wpReadEvent.lock();
		if(sp&&sp->uiUserData==uiLcPkgNum)
		{
			this->spReadPkgObj=spLcPkgObj;
			sp->uiCallBackData=uiLcPkgNum;
		}
		this->mWpReadEventMutex.unlock();
		if(sp) sp->m_event.setEvent();
	}
	return true;
}

bool MySocketArchiveClientR::blockConnect(const char *_ip,int _port)
{
	BOOL ret=FALSE;
	TxCppPlatform::shared_ptr<MyDbSQLiteArchiveTcpClientConnectCallBackInfo_> spLcEvent(new MyDbSQLiteArchiveTcpClientConnectCallBackInfo_());
	this->mWpReadEventMutex.lock();
	this->wpReadEvent=spLcEvent;
	spLcEvent->uiUserData=0;
	this->mWpReadEventMutex.unlock();
	__super::asyncConnect(_ip,_port);
	spLcEvent->m_event.waitEvent();
	this->mWpReadEventMutex.lock();
	if(spLcEvent->uiUserData)
		ret=TRUE;
	this->mWpReadEventMutex.unlock();
	return !!ret;
}

TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> MySocketArchiveClientR::execute(CNsTransmitInfo::tagJournalTransBase *_data,int _i_time_msec)
{
	DefAssertWrap(_data!=NULL);
	if(_data==NULL||!this->bZoeticSign||this->getRiskSocket()==INVALID_SOCKET)
		return TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase>();

	TxCppPlatform::shared_ptr<MyDbSQLiteArchiveTcpClientConnectCallBackInfo_> spLcEvent(new MyDbSQLiteArchiveTcpClientConnectCallBackInfo_());
	this->mWpReadEventMutex.lock();
	this->wpReadEvent=spLcEvent;
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> __sp_tem=this->spReadPkgObj;
	this->spReadPkgObj.reset();
	unsigned int uiLcCmdNum=(++this->uiRecordCurrentPackageNumber);
	spLcEvent->uiUserData=uiLcCmdNum;
	spLcEvent->uiCallBackData=~uiLcCmdNum;
	this->mWpReadEventMutex.unlock();

	__sp_tem.reset();

	CNsTransmitInfo::CShellPackData::pack_and_send(this->shared_from_this(),_data,uiLcCmdNum);
	for (;;)
	{
		this->bExecReadOnceSign=FALSE;
		spLcEvent->m_event.waitEvent(_i_time_msec);
		if(!this->bExecReadOnceSign)
			break;

		this->mWpReadEventMutex.lock();
		__sp_tem=this->spReadPkgObj;
		this->spReadPkgObj.reset();
		this->mWpReadEventMutex.unlock();

		if(__sp_tem&&spLcEvent->uiCallBackData==uiLcCmdNum)
			return __sp_tem;
	}

	return TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase>();
}









//
//TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute> MySocketArchiveClientR::execute_sql(const char *_sql,int _i_time_msec)
//{
//	TxCppPlatform::shared_ptr<MyDbSQLiteArchiveTcpClientConnectCallBackInfo_> spLcEvent(new MyDbSQLiteArchiveTcpClientConnectCallBackInfo_());
//	this->mWpReadEventMutex.lock();
//	this->wpReadEvent=spLcEvent;
//	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> __sp_tem=this->spReadPkgObj;
//	this->spReadPkgObj.reset();
//	unsigned int uiLcCmdNum=(++this->uiPackageNumber);
//	spLcEvent->uiUserData=uiLcCmdNum;
//	this->mWpReadEventMutex.unlock();
//
//	__sp_tem.reset();
//	CNsTransmitInfo::tagRequestSqlExecute m_lc_SqlExecute;
//	m_lc_SqlExecute.initBasic(CNsTransmitInfo::e_type_sql_execute_rqt,uiLcCmdNum);
//	m_lc_SqlExecute.set(_sql);
//	CNsTransmitInfo::CShellPackData::pack_and_send(this->shared_from_this(),m_lc_SqlExecute);
//	spLcEvent->m_event.waitEvent(_i_time_msec);
//
//	this->mWpReadEventMutex.lock();
//	__sp_tem=this->spReadPkgObj;
//	this->spReadPkgObj.reset();
//	this->mWpReadEventMutex.unlock();
//
//	if(__sp_tem&&__sp_tem->uiPkgNum==uiLcCmdNum)
//		return TxCppPlatform::dynamic_pointer_cast<CNsTransmitInfo::tagRespondSqlExecute>(__sp_tem);
//	return TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute>();
//}
//
//TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondInsertJournal> MySocketArchiveClientR::addJournal(
//	CNsTransmitInfo::tagRequestInsertJournal *_tem,int _i_time_msec)
//{
//	TxCppPlatform::shared_ptr<MyDbSQLiteArchiveTcpClientConnectCallBackInfo_> spLcEvent(new MyDbSQLiteArchiveTcpClientConnectCallBackInfo_());
//	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> __sp_tem;
//	this->mWpReadEventMutex.lock();
//	this->wpReadEvent=spLcEvent;
//	__sp_tem=this->spReadPkgObj;
//	this->spReadPkgObj.reset();
//	unsigned int uiLcCmdNum=(++this->uiPackageNumber);
//	spLcEvent->uiUserData=uiLcCmdNum;
//	this->mWpReadEventMutex.unlock();
//
//	__sp_tem.reset();
//	_tem->uiPkgNum=uiLcCmdNum;
//	CNsTransmitInfo::CShellPackData::pack_and_send(this->shared_from_this(),*_tem);
//
//	spLcEvent->m_event.waitEvent(_i_time_msec);
//
//	this->mWpReadEventMutex.lock();
//	__sp_tem=this->spReadPkgObj;
//	this->spReadPkgObj.reset();
//	this->mWpReadEventMutex.unlock();
//
//	if(__sp_tem&&__sp_tem->uiPkgNum==uiLcCmdNum)
//		return TxCppPlatform::dynamic_pointer_cast<CNsTransmitInfo::tagRespondInsertJournal>(__sp_tem);
//	return TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondInsertJournal>();
//}


//CSocketArchiveClient::CSocketArchiveClient(TxIocpTcpApi::TxIocpService *_p_svc,int _ciMtu,const char *_ip,int _port):p_svc(_p_svc),ciMtu(_ciMtu),strIp(_ip),ciPort(_port)
//{
//	this->eThreadRunning=e_thread_null;
//	this->m_event.setEvent();
//	this->mThread.join();
//	TxCppPlatform::shared_ptr<MySocketArchiveClientR> spLcJournalSession=this->wpJournalSession.lock();
//	if(spLcJournalSession)
//		spLcJournalSession->close();
//}
//
//CSocketArchiveClient::~CSocketArchiveClient()
//{
//	this->finish();
//}
//
//void CSocketArchiveClient::finish()
//{
//	if(this->eThreadRunning!=e_thread_end)
//	{
//		this->eThreadRunning=e_thread_end;
//		this->m_event.setEvent();
//		this->mThread.join();
//	}
//	TxCppPlatform::shared_ptr<MySocketArchiveClientR> spLcJournalSession=wpJournalSession.lock();
//	if(spLcJournalSession) spLcJournalSession->close();
//}
//
//void CSocketArchiveClient::_static_call_back_(void *_arg1,void *_arg2)
//{
//	(void)_arg2;
//	((CSocketArchiveClient*)_arg1)->_call_back_();
//}
//
//bool CSocketArchiveClient::_force_send_log_(CNsTransmitInfo::tagRequestInsertJournal *_tem)
//{
//	TxCppPlatform::shared_ptr<MySocketArchiveClientR> spLcJournalSession=this->wpJournalSession.lock();
//	if(spLcJournalSession)
//	{
//		TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondInsertJournal> res=spLcJournalSession->addJournal(_tem,3*1000);
//		if(res)
//			return true;
//	}
//	if(spLcJournalSession)
//		spLcJournalSession->close();
//	this->wpJournalSession.reset();
//	spLcJournalSession.reset(new MySocketArchiveClientR(this->p_svc,this->ciMtu));
//	if(spLcJournalSession->blockConnect(this->strIp.c_str(),this->ciPort))
//	{
//		this->wpJournalSession=spLcJournalSession;
//		TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondInsertJournal> res=spLcJournalSession->addJournal(_tem,3*1000);
//		if(res)
//			return true;
//	}
//	return false;
//}
//
//void CSocketArchiveClient::_call_back_()
//{
//	CNsTransmitInfo::tagRequestInsertJournal tem;
//	long bLcRunThread=1;
//	long bLcHaveData=0;
//	long bLcLoop=1;
//	while(bLcLoop!=0)
//	{
//		this->m_event.waitEvent(bLcHaveData?(DWORD)10:(DWORD)(INFINITE));
//		for(;;)
//		{
//			this->mutex.lock();
//			if(!bLcHaveData)
//			{
//				if(this->listLogData.size()>0)
//				{
//					tem=this->listLogData.front();
//					this->listLogData.pop_front();
//					bLcHaveData=1;
//				}
//			}
//			if(this->eThreadRunning!=e_thread_run)
//				bLcRunThread=0;
//			this->mutex.unlock();
//			if(bLcHaveData&&_force_send_log_(&tem))
//			{
//				bLcHaveData=0;
//			}
//			else
//			{
//				if(bLcRunThread==0)
//					bLcLoop=0;
//				break;
//			}
//		}
//	}
//}
//
//void CSocketArchiveClient::addLog(const CNsTransmitInfo::tagRequestInsertJournal &_tem)
//{
//	int bLcSetEvent=0;
//	this->mutex.lock();
//	if(this->listLogData.size()<=0)
//		bLcSetEvent=1;
//	this->listLogData.push_back(_tem);
//	if(this->listLogData.size()>1024*10)
//		this->listLogData.pop_front();
//	if(this->eThreadRunning==e_thread_null)
//	{
//		this->eThreadRunning=e_thread_run;
//		this->mThread.create(_static_call_back_,this,NULL);
//	}
//	this->mutex.unlock();
//	if(bLcSetEvent!=0)
//		this->m_event.setEvent();
//}



CSocketArchiveClient::CSocketArchiveClient(TxIocpTcpApi::TxIocpService *_p_svc,const char *_ip,int _port,int _iMtu)
{
	this->p_svc=_p_svc;
	TxBlobString::txStrNCopy(this->sIp,_ip);
	this->sIp[sizeof(this->sIp)-1]=0;
	this->iPort=_port;
	this->iMtu=_iMtu;
}

CSocketArchiveClient::~CSocketArchiveClient()
{
	this->disconnect();
}

bool CSocketArchiveClient::connect()
{
	TxCppPlatform::shared_ptr<MySocketArchiveClientR> spClient(this->wpSession.lock());
	if(spClient&&!!spClient->bZoeticSign&&spClient->getRiskSocket()!=INVALID_SOCKET)
		return true;
	if(spClient)
	{
		this->wpSession.reset();
		spClient->close();
	}
	spClient.reset(new MySocketArchiveClientR(this->p_svc,this->iMtu));
	if(!spClient->blockConnect(this->sIp,this->iPort))
	{
		spClient->close();
		spClient.reset();
	}
	if(spClient)
		this->wpSession=spClient;
	return !!spClient;
}

void CSocketArchiveClient::disconnect()
{
	TxCppPlatform::shared_ptr<MySocketArchiveClientR> spClient(this->wpSession.lock());
	if(spClient)
		spClient->close();
	this->wpSession.reset();
}

TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> CSocketArchiveClient::execute(CNsTransmitInfo::tagJournalTransBase *_data,int _i_time_msec)
{
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> ret;
	{
		TxCppPlatform::shared_ptr<MySocketArchiveClientR> spClient1(this->wpSession.lock());
		if(spClient1)
		{
			ret=spClient1->execute(_data,_i_time_msec);
			if(ret) return ret;
			ret=spClient1->execute(_data,_i_time_msec);
			if(ret) return ret;
			this->wpSession.reset();
			spClient1->close();
		}
	}
	{
		TxCppPlatform::shared_ptr<MySocketArchiveClientR> spClient2(new MySocketArchiveClientR(this->p_svc,this->iMtu));
		this->wpSession=spClient2;
		if(spClient2->blockConnect(this->sIp,this->iPort))
		{
			ret=spClient2->execute(_data,_i_time_msec);
			if(ret) return ret;
			ret=spClient2->execute(_data,_i_time_msec);
			if(ret) return ret;
		}
		spClient2->close();
		this->wpSession.reset();
	}
	return ret;
}

bool CSocketArchiveClient::isSocketValid()
{
	return !this->wpSession.expired();
}
