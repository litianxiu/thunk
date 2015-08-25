#define _Tx_SqliteExpand_dll_OutPut_
#include "CRemotFileServerDownLoadManager.h"
#include "CDbSQLiteArchiveServerSocket.h"

//
//void CRemotFileServerDownLoadManager::dealGainFileInfo(
//	TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,
//	const CNsTransmitInfo::tagRequestRemoteFileGainFileInfo &_arg,unsigned int _uiPkgNum)
//{
//	CDbSQLiteArchiveTcpPeerSocket* pLcOrgSocket=dynamic_cast<CDbSQLiteArchiveTcpPeerSocket*>(_spSocket.get());
//	if(pLcOrgSocket!=NULL)
//	{
//		HANDLE pfile = ::CreateFileA(
//			pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->filename.c_str(),
//			GENERIC_READ,
//			FILE_SHARE_READ,
//			NULL,
//			OPEN_EXISTING,
//			FILE_ATTRIBUTE_NORMAL,
//			NULL
//			);
//
//		if(pfile!=INVALID_HANDLE_VALUE)
//		{
//			CNsTransmitInfo::tagRespondRemoteFileGainFileInfo tem;
//			tem.initBasic(CNsTransmitInfo::e_type_remote_file_fileinfo_rpd);
//			tem.set("无法找到相应的下载文件",0,NULL,0);
//			CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&tem,_uiPkgNum);
//		}
//		else
//		{
//			LARGE_INTEGER lcFileSize={0};
//			if(!::GetFileSizeEx(pfile,&lcFileSize))
//			{
//				CNsTransmitInfo::tagRespondRemoteFileGainFileInfo tem;
//				tem.initBasic(CNsTransmitInfo::e_type_remote_file_fileinfo_rpd);
//				tem.set("无法获取文件大小",0,NULL,0);
//				CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&tem,_uiPkgNum);
//			}
//			else
//			{
//				CNsTransmitInfo::tagRespondRemoteFileGainFileInfo tem;
//				tem.initBasic(CNsTransmitInfo::e_type_remote_file_fileinfo_rpd);
//				tem.set("",lcFileSize.QuadPart,NULL,0);
//				CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&tem,_uiPkgNum);
//			}
//			::CloseHandle(pfile);
//		}
//	}
//}
//
//void CRemotFileServerDownLoadManager::sendSomeDataStream(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,BOOL *_bSendData)
//{
//	BOOL bbbbttttemp;
//	if(_bSendData==NULL) _bSendData=&bbbbttttemp;
//	*_bSendData=FALSE;
//	CDbSQLiteArchiveTcpPeerSocket* pLcOrgSocket=dynamic_cast<CDbSQLiteArchiveTcpPeerSocket*>(_spSocket.get());
//	if(pLcOrgSocket!=NULL&&pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo!=NULL)
//	{
//		std::list<TxCppPlatform::shared_ptr<tagRemotFileServerDownLoadUserDataInfo::tagUnitFileData>> lc_send_some_data;
//		_spSocket->lockObject();
//		int iLcRecordTotalTemSize=0;
//		while(pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->mFileData.size()>0&&iLcRecordTotalTemSize<=32*1024)
//		{
//			lc_send_some_data.push_back(pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->mFileData.front());
//			iLcRecordTotalTemSize+=pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->mFileData.front()->data.size();
//			pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->mFileData.pop_front();
//		}
//		int iLcSurCount=pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->mFileData.size();
//		_spSocket->unlockObject();
//		if(lc_send_some_data.size()>0)
//		{
//			std::list<TxCppPlatform::shared_ptr<tagRemotFileServerDownLoadUserDataInfo::tagUnitFileData>>::iterator iter=lc_send_some_data.begin();
//			std::list<TxCppPlatform::shared_ptr<tagRemotFileServerDownLoadUserDataInfo::tagUnitFileData>>::iterator iter_end=lc_send_some_data.end();
//			for(;iter!=iter_end;iter++)
//			{
//				CNsTransmitInfo::tagRespondRemoteFileDownLoadFileStream tem;
//				tem.initBasic(CNsTransmitInfo::e_type_remote_file_readstream_rpd);
//				tem.set("",(*iter)->pos,false,(*iter)->data.data(),(*iter)->data.size());
//				CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&tem,0);
//				*_bSendData=TRUE;
//			}
//		}
//		if(iLcSurCount<=0)
//		{
//			if(!_readFileDataStream(_spSocket,_bSendData))
//			{
//				CNsTransmitInfo::tagRespondRemoteFileDownLoadFileStream tem;
//				tem.initBasic(CNsTransmitInfo::e_type_remote_file_readstream_rpd);
//				tem.set("",0,true,0,0);
//				CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&tem,0);
//			}
//		}
//	}
//}
//
//BOOL CRemotFileServerDownLoadManager::_readFileDataStream(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,BOOL *_bSendData)
//{
//	CDbSQLiteArchiveTcpPeerSocket* pLcOrgSocket=dynamic_cast<CDbSQLiteArchiveTcpPeerSocket*>(_spSocket.get());
//	std::list<TxCppPlatform::shared_ptr<tagRemotFileServerDownLoadUserDataInfo::tagUnitFileData>> mLcRecordFileDataMid;
//	if(pLcOrgSocket!=NULL&&pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo!=NULL)
//	{
//		HANDLE pfile = ::CreateFileA(
//				pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->filename.c_str(),
//				GENERIC_READ,
//				FILE_SHARE_READ,
//				NULL,
//				OPEN_EXISTING,
//				FILE_ATTRIBUTE_NORMAL,
//				NULL
//				);
//
//		if(pfile!=INVALID_HANDLE_VALUE)
//		{
//			CNsTransmitInfo::tagRespondRemoteFileDownLoadFileStream tem;
//			tem.initBasic(CNsTransmitInfo::e_type_remote_file_readstream_rpd);
//			tem.set("无法找到相应的下载文件",0,false,NULL,0);
//			*_bSendData=TRUE;
//			CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&tem,0);
//		}
//		else
//		{
//			LARGE_INTEGER lcFileSize={0};
//			if(!::GetFileSizeEx(pfile,&lcFileSize))
//			{
//				CNsTransmitInfo::tagRespondRemoteFileDownLoadFileStream tem;
//				tem.initBasic(CNsTransmitInfo::e_type_remote_file_readstream_rpd);
//				tem.set("无法获取文件大小",0,false,NULL,0);
//				*_bSendData=TRUE;
//				CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&tem,0);
//			}
//			else
//			{
//				int iLcUnitCount=(2*1024*1024+pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->iUnitSize-1)/pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->iUnitSize;
//				if(iLcUnitCount>0)
//				{
//					std::vector<long long> vLcFilePos;
//					vLcFilePos.reserve(iLcUnitCount);
//					std::list<long long> *pLcListFilePos=&pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->listFilePos;
//					while(pLcListFilePos->size()>0)
//					{
//						if((--iLcUnitCount)<0) break;
//						vLcFilePos.push_back(pLcListFilePos->front());
//						pLcListFilePos->pop_front();
//					}
//
//					int iLcUnitStartPosX=0;
//					int iLcMaxPosSize=0;
//					unsigned char *lc_read_buff=new unsigned char[iLcMaxPosSize];
//
//					for(int yyy=1;yyy<(int)vLcFilePos.size();yyy++)
//					{
//						if(vLcFilePos[yyy]-vLcFilePos[yyy-1]!=pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->iUnitSize
//							||yyy==(int)vLcFilePos.size()-1)
//						{
//							DWORD dwCount=0;
//							OVERLAPPED lv_ovlpd={0};
//							lv_ovlpd.Offset=(DWORD)vLcFilePos[iLcUnitStartPosX];
//							lv_ovlpd.OffsetHigh=(DWORD)(vLcFilePos[iLcUnitStartPosX]>>32);
//							if(iLcMaxPosSize<(int)(vLcFilePos[yyy]-vLcFilePos[iLcUnitStartPosX]))
//							{
//								iLcMaxPosSize=(int)(vLcFilePos[yyy]-vLcFilePos[iLcUnitStartPosX]);
//								if(lc_read_buff!=NULL)
//									delete[] lc_read_buff;
//								lc_read_buff=new unsigned char[iLcMaxPosSize];
//							}
//							if(::ReadFile(pfile,lc_read_buff,(DWORD)(vLcFilePos[yyy]-vLcFilePos[iLcUnitStartPosX]),&dwCount,&lv_ovlpd)
//								&&(DWORD)(vLcFilePos[yyy]-vLcFilePos[iLcUnitStartPosX])==dwCount&&dwCount!=0)
//							{
//								unsigned char *lc_read_buff_end_=lc_read_buff+(int)dwCount;
//								for(int nnn=yyy;nnn<(int)vLcFilePos.size();nnn++)
//								{
//									TxCppPlatform::shared_ptr<tagRemotFileServerDownLoadUserDataInfo::tagUnitFileData> spLcArgs(
//										new tagRemotFileServerDownLoadUserDataInfo::tagUnitFileData());
//									spLcArgs->pos=vLcFilePos[nnn];
//									unsigned char *lc_read_buff_start_=lc_read_buff+(int)(vLcFilePos[nnn]-vLcFilePos[iLcUnitStartPosX]);
//									int lc_buff_valid_size=(int)(lc_read_buff_end_-lc_read_buff_start_);
//									spLcArgs->data.assign(lc_read_buff_start_,
//										pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->iUnitSize<lc_buff_valid_size?
//										pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->iUnitSize:lc_buff_valid_size);
//									mLcRecordFileDataMid.push_back(spLcArgs);
//								}
//							}
//						}
//					}
//					if(lc_read_buff!=NULL)
//						delete[] lc_read_buff;
//					pLcOrgSocket->lockObject();
//					pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->mFileData.insert(
//						pLcOrgSocket->pRemotFileServerDownLoadUserDataInfo->mFileData.end(),
//						mLcRecordFileDataMid.begin(),mLcRecordFileDataMid.end());
//					pLcOrgSocket->unlockObject();
//					sendSomeDataStream(_spSocket,_bSendData);
//				}
//			}
//			::CloseHandle(pfile);
//		}
//	}
//	return mLcRecordFileDataMid.size()>0;
//}

void CRemotFileServerDownLoadManager::dealDownLoadDataStream(
	TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,
	const CNsTransmitInfo::tagRequestRemoteFileReadDataStream &_arg,unsigned int _uiPkgNum)
{
	CDbSQLiteArchiveTcpPeerSocket* pLcOrgSocket=dynamic_cast<CDbSQLiteArchiveTcpPeerSocket*>(_spSocket.get());
	if(pLcOrgSocket!=NULL)
	{
		CNsTransmitInfo::tagRespondRemoteFileReadDataStream lc_send_obj;
		lc_send_obj.initBasic(CNsTransmitInfo::e_type_remote_file_readstream_rpd);
		//if(_arg.bFileCrc32)
		//{
		//	long long llLcFileSize=0;
		//	unsigned char lc_crc32[4];
		//	CWindowsFileHandle::g_build_crc32(lc_crc32,&llLcFileSize,_arg.strFilePath.c_str());
		//	lc_send_obj.set("",llLcFileSize,lc_crc32,sizeof(lc_crc32),_arg.llFileDataPos,NULL,0);
		//}
		if(_arg.llFileDataPos>=0&&_arg.iReadDataSize>0)
		{
			char *lc_p_buff=new char[_arg.iReadDataSize];
			int lc_buff_size=0;
			std::string s_lc_err;
			long long llLcFileSize=0;
			CWindowsFileHandle::g_read_file(lc_p_buff,&lc_buff_size,&s_lc_err,&llLcFileSize,_arg.strFilePath.c_str(),_arg.llFileDataPos,_arg.iReadDataSize);
			lc_send_obj.set(s_lc_err.c_str(),llLcFileSize,lc_p_buff,lc_buff_size);
			delete lc_p_buff;
		}
		CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&lc_send_obj,_uiPkgNum);
	}
}

