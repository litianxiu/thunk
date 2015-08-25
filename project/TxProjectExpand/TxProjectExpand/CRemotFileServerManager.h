#pragma once

#include <list>
#include "../common/TxBlobString.hpp"
#include "../common/TxIocpTcpApi.hpp"
#include "CNsRemoteFileTransmitInfo.hpp"
#include "CWindowsWrapExt.h"

//PathRelativePathTo两个绝对路径转成一个相对路径
class CRemotFileServerManager
{
private:
	static void _visitFileInfo(std::list<CNsTransmitInfo::tagRespondRemoteFileFind::tagFileInfo> *_listFileInfo,
		TxBlobString *_retErr,const char *_pstr,const char *_strWildcard);
public :
	static void dealRemoteFileFind(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,
		const CNsTransmitInfo::tagRequestRemoteFileFind &_arg,unsigned int _uiPkgNum);
	static void dealRemoteFileDelete(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,
		const CNsTransmitInfo::tagRequestRemoteFileDeleteFile &_arg,unsigned int _uiPkgNum);
};

