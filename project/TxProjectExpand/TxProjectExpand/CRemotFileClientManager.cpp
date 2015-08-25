#define _Tx_SqliteExpand_dll_OutPut_

#include <afx.h>
#include <shlwapi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "CRemotFileClientManager.h"

void CRemotFileClientManager::g_findRemoteFile(CSocketArchiveClient *_pSocket,const char *_strFilePath,const char *_strWildcard,int _i_time_msec,TSqliteCallBackDllFunc _cb,void *_userdata)
{
	CNsTransmitInfo::tagRequestRemoteFileFind mLcFindArgs;
	mLcFindArgs.initBasic(CNsTransmitInfo::e_type_remote_file_find_rqt);
	mLcFindArgs.set(_strFilePath,(_strWildcard==NULL?"":_strWildcard));
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondRemoteFileFind> lc_result;
	_pSocket->execute(&lc_result,&mLcFindArgs,_i_time_msec);
	if(lc_result)
	{
		if(lc_result->sErrorText.length()>0)
		{
			tagFindRemotFileRespondData tem={0};
			tem.iTotalCount=(int)lc_result->listFileInfo.size();
			tem.iCurRow=-1;
			tem.strErrorText=lc_result->sErrorText.c_str();
			_cb(_userdata,&tem);
		}
		std::list<CNsTransmitInfo::tagRespondRemoteFileFind::tagFileInfo>::iterator iter=lc_result->listFileInfo.begin();
		std::list<CNsTransmitInfo::tagRespondRemoteFileFind::tagFileInfo>::iterator iter_end=lc_result->listFileInfo.end();
		tagFindRemotFileRespondData res_arg={0};
		res_arg.strFilePath=lc_result->strFilePath.c_str();
		res_arg.strWildcard=lc_result->strWildcard.c_str();
		res_arg.iTotalCount=(int)lc_result->listFileInfo.size();
		for(;iter!=iter_end;iter++)
		{
			res_arg.filename=iter->filename.c_str();
			res_arg.filesize=iter->filesize;
			res_arg.time=iter->time;
			res_arg.permission=iter->permission;
			_cb(_userdata,&res_arg);
			res_arg.iCurRow++;
		}
	}
}

void CRemotFileClientManager::g_deleteRemoteFile(
	CSocketArchiveClient *_pSocket, const char *_strFileBasic,const char *_strFileName[],int _count,int _i_time_msec,TSqliteCallBackDllFunc _cb,void *_userdata)
{
	CNsTransmitInfo::tagRequestRemoteFileDeleteFile mLcDeleteArgs;
	mLcDeleteArgs.initBasic(CNsTransmitInfo::e_type_remote_file_delete_rqt);
	mLcDeleteArgs.set(_strFileBasic);
	for(int i=0;i<_count;i++)
	{
		if(_strFileName[i]!=NULL)
			mLcDeleteArgs.listName.push_back(TxBlobString(_strFileName[i]));
	}

	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondRemoteFileDeleteFile> lc_result;
	_pSocket->execute(&lc_result,&mLcDeleteArgs,_i_time_msec);
	if(lc_result)
	{
		if(lc_result->sErrorText.length()>0)
			_cb(_userdata,lc_result->sErrorText.c_str());
		else
			_cb(_userdata,NULL);
	}
	else _cb(_userdata,"无法连接服务器");
}

void CRemotFileClientManager::g_readRemoteFile(CSocketArchiveClient *_pSocket,const char *_strFileUri,int _i_time_msec,
											   long long _pos,int _size,TSqliteCallBackDllFunc _cb,void *_userdata)
{
	CNsTransmitInfo::tagRequestRemoteFileReadDataStream mLcReadArgs;
	mLcReadArgs.initBasic(CNsTransmitInfo::e_type_remote_file_readstream_rqt);
	mLcReadArgs.set(_strFileUri,_pos,_size);
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondRemoteFileReadDataStream> lc_result;
	_pSocket->execute(&lc_result,&mLcReadArgs,_i_time_msec);
	tagRemoteFileReadRespondData cb_args={0};
	if(lc_result)
	{
		if(lc_result->sErrorText.length()>0)
		{
			cb_args.error=lc_result->sErrorText.c_str();
		}
		else
		{
			cb_args.error=NULL;
			cb_args.data=(const unsigned char*)lc_result->mReadFileData.data();
			cb_args.size=lc_result->mReadFileData.size();
			cb_args.llTotalSize=lc_result->llFileSize;
		}
	}
	else cb_args.error="无法连接服务器";
	_cb(_userdata,&cb_args);
}


void CRemotFileClientManager::g_writeRemoteFile(CSocketArchiveClient *_pSocket,const char *_strFileUri,int _i_time_msec,
												const void *_buff,long long _pos,int _size,TSqliteCallBackDllFunc _cb,void *_userdata)
{
	CNsTransmitInfo::tagRequestRemoteFileWriteDataStream mLcWriteArgs;
	mLcWriteArgs.initBasic(CNsTransmitInfo::e_type_remote_file_writestream_rqt);
	mLcWriteArgs.set(_strFileUri,_pos,_buff,_size);
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondRemoteFileWriteDataStream> lc_result;
	_pSocket->execute(&lc_result,&mLcWriteArgs,_i_time_msec);
	tagRemoteFileWriteRespondData cb_args={0};
	if(lc_result)
	{
		if(lc_result->sErrorText.length()>0)
		{
			cb_args.error=lc_result->sErrorText.c_str();
		}
		else
		{
			cb_args.error=NULL;
		}
	}
	else cb_args.error="无法连接服务器";
	_cb(_userdata,&cb_args);
}

