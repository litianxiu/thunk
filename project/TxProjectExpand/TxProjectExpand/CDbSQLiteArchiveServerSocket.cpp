#define _Tx_SqliteExpand_dll_OutPut_
#include "CDbSQLiteArchiveServerSocket.h"
#include "CRemotFileServerDownLoadManager.h"


CDbSQLiteArchiveTcpPeerSocket::CDbSQLiteArchiveTcpPeerSocket(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spServerSocket,int _mtu,CSQLiteDataBaseManager *_pSqlDataBaseMgr)
	:TxIocpTcpApi::TxIocpTcpPeerSocket(_spServerSocket,_mtu),b_iter(FALSE),pSqlDataBaseMgr(_pSqlDataBaseMgr)
{
}

CDbSQLiteArchiveTcpPeerSocket::~CDbSQLiteArchiveTcpPeerSocket()
{
}

void CDbSQLiteArchiveTcpPeerSocket::pushData(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,const unsigned char *_data,int _size)
{
	this->bDoSomeThing[0]=1;
	this->bDoSomeThing[1]=1;
	this->m_ShellPackage.pushData((const char*)_data,_size);
	std::string mLcPkgString;
	for(;;)
	{
		std::string lc_pkg_data;
		unsigned int uiLcPkgNum=(unsigned int)-1;
		TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> spPkgObj=this->m_ShellPackage.popResult(&uiLcPkgNum);
		if(!spPkgObj) break;
		switch(spPkgObj->eType)
		{
		case CNsTransmitInfo::e_type_sql_execute_rqt:
			this->pSqlDataBaseMgr->execute_sql(*spPkgObj->getOrgThisPointer<CNsTransmitInfo::tagRequestSqlExecute>(),_sp,uiLcPkgNum);
			break;
		case CNsTransmitInfo::e_type_insert_journal_rqt:
			//{
			//	CNsTransmitInfo::tagRespondInsertJournal mLcRespondInsertJournal;
			//	mLcRespondInsertJournal.initBasic(CNsTransmitInfo::e_type_insert_journal_rpd,spPkgObj->uiPkgNum);
			//	mLcRespondInsertJournal.set("");
			//	CNsTransmitInfo::CShellPackData::pack_and_send(_sp,mLcRespondInsertJournal);
			//	this->pSqlDataBaseMgr->addInsertJournal(*spPkgObj->getOrgThisPointer<CNsTransmitInfo::tagRequestInsertJournal>());
			//}
			this->pSqlDataBaseMgr->dealInsertJournal(_sp,spPkgObj.get(),uiLcPkgNum);
			break;
		case CNsTransmitInfo::e_type_remote_file_find_rqt:
			{
				CNsTransmitInfo::tagRequestRemoteFileFind *p_lc_args=spPkgObj->getOrgThisPointer<CNsTransmitInfo::tagRequestRemoteFileFind>();
				if(p_lc_args!=NULL)
					CRemotFileServerManager::dealRemoteFileFind(_sp,*p_lc_args,uiLcPkgNum);
			}
			break;
		case CNsTransmitInfo::e_type_remote_file_delete_rqt:
			{
				CNsTransmitInfo::tagRequestRemoteFileDeleteFile *p_lc_args=spPkgObj->getOrgThisPointer<CNsTransmitInfo::tagRequestRemoteFileDeleteFile>();
				if(p_lc_args!=NULL)
					CRemotFileServerManager::dealRemoteFileDelete(_sp,*p_lc_args,uiLcPkgNum);
			}
			break;
		case CNsTransmitInfo::e_type_remote_file_readstream_rqt:
			{
				CNsTransmitInfo::tagRequestRemoteFileReadDataStream *p_lc_args=spPkgObj->getOrgThisPointer<CNsTransmitInfo::tagRequestRemoteFileReadDataStream>();
				if(p_lc_args!=NULL)
					CRemotFileServerDownLoadManager::dealDownLoadDataStream(_sp,*p_lc_args,uiLcPkgNum);
			}
			break;
		default:
			DefAssertWrap(FALSE);
			break;
		}
	}
}

CDbSQLiteArchiveServerSocket::CDbSQLiteArchiveServerSocket(TxIocpTcpApi::TxIocpService *_svc,int _mtu)
:TxIocpTcpApi::TxIocpTcpServerSocket(_svc),mtu(_mtu)
{
	this->bValid=TRUE;
}

CDbSQLiteArchiveServerSocket::~CDbSQLiteArchiveServerSocket()
{
	this->close();
}

bool CDbSQLiteArchiveServerSocket::listen(int _port,const char *_sSQLiteDbFile)
{
	if(this->mSqlDataBaseMgr.open(_sSQLiteDbFile)
		&&TxIocpTcpApi::TxIocpTcpServerSocket::listen(_port))
	{
		return true;
	}
	this->close();
	this->mSqlDataBaseMgr.close();
	return false;
}

TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> CDbSQLiteArchiveServerSocket::onNewSocketEvent()
{
	return TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic>(new CDbSQLiteArchiveTcpPeerSocket(this->shared_from_this(),this->mtu,&mSqlDataBaseMgr));
}

void CDbSQLiteArchiveServerSocket::close()
{
	this->listPeerSocket_Mutex.lock();
	if(this->bValid) this->bValid=FALSE;
	__super::close();
	std::list<TxCppPlatform::weak_ptr<TxIocpTcpApi::TxIocpSocketBasic>>::iterator iter=this->listPeerSocket.begin();
	std::list<TxCppPlatform::weak_ptr<TxIocpTcpApi::TxIocpSocketBasic>>::iterator iter_end=this->listPeerSocket.end();
	while(iter!=iter_end)
	{
		TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> sp(*iter);
		if(sp)
		{
			sp->close();
			iter++;
		}
		else
		{
			iter=this->listPeerSocket.erase(iter);
		}
	}
	this->listPeerSocket_Mutex.unlock();
}

bool CDbSQLiteArchiveServerSocket::onAcceptPeerSocketEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,const SOCKADDR_IN *_addr)
{
	(void)&_sp; (void)_addr;
	CDbSQLiteArchiveTcpPeerSocket *pPeerSocket=(CDbSQLiteArchiveTcpPeerSocket*)(_sp.get());
	std::list<TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic>> m_lc_sp_peer;
	{
		this->listPeerSocket_Mutex.lock();
		if(!this->bValid)
		{
			_sp->close();
		}
		else
		{
			listPeerSocket.push_front(_sp);
			pPeerSocket->m_iter=listPeerSocket.begin();
			pPeerSocket->b_iter=TRUE;
			pPeerSocket->bDoSomeThing[0]=1;
			pPeerSocket->bDoSomeThing[1]=1;
			if(this->mRecordTimeSpan.dtctCurTimeResetSpanMsec(60*1000,true))
			{
				std::list<TxCppPlatform::weak_ptr<TxIocpTcpApi::TxIocpSocketBasic>>::iterator iter=this->listPeerSocket.begin();
				std::list<TxCppPlatform::weak_ptr<TxIocpTcpApi::TxIocpSocketBasic>>::iterator iter_end=this->listPeerSocket.end();
				while(iter!=iter_end)
				{
					m_lc_sp_peer.push_back(iter->lock());
					if(m_lc_sp_peer.back()) iter++;
					else iter=this->listPeerSocket.erase(iter);
				}
			}
		}
		this->listPeerSocket_Mutex.unlock();
	}
	{
		std::list<TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic>>::iterator iter=m_lc_sp_peer.begin();
		std::list<TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic>>::iterator iter_end=m_lc_sp_peer.end();
		while(iter!=iter_end)
		{
			CDbSQLiteArchiveTcpPeerSocket *ptr=(CDbSQLiteArchiveTcpPeerSocket*)iter->get();
			if(ptr!=NULL)
			{
				if(ptr->bDoSomeThing[1]==0&&ptr->bDoSomeThing[0]==0)
				{
					ptr->close();
				}
				else
				{
					ptr->bDoSomeThing[0]=0;
					ptr->bDoSomeThing[1]=0;
				}
			}
			iter++;
		}
	}
	return true;
}

bool CDbSQLiteArchiveServerSocket::onPeerSocketReadExceptionEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp)
{
	CDbSQLiteArchiveTcpPeerSocket *pPeerSocket=(CDbSQLiteArchiveTcpPeerSocket*)(_sp.get());
	this->listPeerSocket_Mutex.lock();
	if(pPeerSocket->b_iter)
	{
		listPeerSocket.erase(pPeerSocket->m_iter);
		pPeerSocket->b_iter=FALSE;
	}
	this->listPeerSocket_Mutex.unlock();
	return true;
}

bool CDbSQLiteArchiveServerSocket::onPeerSocketReadBufferEvent(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,const unsigned char *_data,int _size)
{
	CDbSQLiteArchiveTcpPeerSocket *pPeerSocket=(CDbSQLiteArchiveTcpPeerSocket*)(_sp.get());
	pPeerSocket->pushData(_sp,_data,_size);
	return true;
}

bool CDbSQLiteArchiveServerSocket::onPeerSocketWriteBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,int _iSurplusSize)
{
	return true;
}
