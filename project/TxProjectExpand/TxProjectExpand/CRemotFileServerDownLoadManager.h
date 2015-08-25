#pragma once


#include <list>
#include "../common/TxBlobString.hpp"
#include "../common/TxIocpTcpApi.hpp"
#include "CNsRemoteFileTransmitInfo.hpp"
#include "CWindowsWrapExt.h"
#include "CWindowsFileHandle.h"
#include "CNsCommonTransmitInfo.hpp"



//PathRelativePathTo两个绝对路径转成一个相对路径
class CRemotFileServerDownLoadManager
{
public :
	static void dealDownLoadDataStream(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,
		const CNsTransmitInfo::tagRequestRemoteFileReadDataStream &_arg,unsigned int _uiPkgNum);
};
