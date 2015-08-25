#pragma once

#include <list>
#include "../common/TxBlobString.hpp"
#include "../common/TxIocpTcpApi.hpp"
#include "CNsRemoteFileTransmitInfo.hpp"
#include "CSocketArchiveClient.h"
#include "CWindowsWrapExt.h"
#include "../include/CSqliteExpandDll.h"

class CRemotFileClientManager
{
public :
	static void g_findRemoteFile(CSocketArchiveClient *_pSocket,
		const char *_strFilePath,const char *_strWildcard,int _i_time_msec,TSqliteCallBackDllFunc _cb,void *_userdata);
	static void g_deleteRemoteFile(CSocketArchiveClient *_pSocket,
		const char *_strFileBasic,const char *_strFileName[],int _count,int _i_time_msec,TSqliteCallBackDllFunc _cb,void *_userdata);
	static void g_readRemoteFile(CSocketArchiveClient *_pSocket,const char *_strFileUri,int _i_time_msec,
		long long _pos,int _size,TSqliteCallBackDllFunc _cb,void *_userdata);
	static void g_writeRemoteFile(CSocketArchiveClient *_pSocket,const char *_strFileUri,int _i_time_msec,
		const void *_buff,long long _pos,int _size,TSqliteCallBackDllFunc _cb,void *_userdata);
};

