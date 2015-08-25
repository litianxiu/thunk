#define _Tx_SqliteExpand_dll_OutPut_

#include <afx.h>
#include <shlwapi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "CRemotFileServerManager.h"

void CRemotFileServerManager::_visitFileInfo(std::list<CNsTransmitInfo::tagRespondRemoteFileFind::tagFileInfo> *_listFileInfo,
	TxBlobString *_retErr,const char *_pstr,const char *_strWildcard)
{
	CFileFind finder;
	TxBlobString strPathWildcard(_pstr);
	strPathWildcard.replaceSelf('\\','/');
	if(strPathWildcard.backChar()!='\\') strPathWildcard.append("\\");
	std::list<std::string> lcListFileName;
	std::string lc_error;
	if(CAfxFileFindExt::gFindAllFile(&lcListFileName,&lc_error,strPathWildcard.c_str(),_strWildcard))
	{
		std::list<std::string>::iterator iter=lcListFileName.begin();
		std::list<std::string>::iterator iter_end=lcListFileName.end();
		for(;iter!=iter_end;iter++)
		{
			struct _stat64 lc_file_info={0};
			if(0==::_stat64((strPathWildcard+(*iter)).c_str(),&lc_file_info))
			{
				CNsTransmitInfo::tagRespondRemoteFileFind::tagFileInfo lc_elem;
				//文件名
				lc_elem.filename.assign(*iter);
				//文件大小
				if(lc_file_info.st_mode&S_IFDIR) lc_elem.filesize=-1;
				else lc_elem.filesize=(long long)lc_file_info.st_size;
				//文件权限
				lc_elem.permission=eFilePermissionMode_Null;
				if(lc_file_info.st_mode&S_IREAD)
				{
					lc_elem.permission=(EnumFilePermissionMode)
						(lc_elem.permission|eFilePermissionMode_Read);
				}
				if(lc_file_info.st_mode&S_IWRITE)
				{
					lc_elem.permission=(EnumFilePermissionMode)
						(lc_elem.permission|eFilePermissionMode_Write);
				}
				if(lc_file_info.st_mode&S_IEXEC)
				{
					lc_elem.permission=(EnumFilePermissionMode)
						(lc_elem.permission|eFilePermissionMode_Exec);
				}
				//文件时间
				lc_elem.time=(long long)lc_file_info.st_mtime;
				_listFileInfo->push_back(lc_elem);
			}
		}
	}
	_retErr->assign(lc_error);
}

void CRemotFileServerManager::dealRemoteFileFind(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,
	const CNsTransmitInfo::tagRequestRemoteFileFind &_arg,unsigned int _uiPkgNum)
{
	CNsTransmitInfo::tagRespondRemoteFileFind _tem;
	_tem.initBasic(CNsTransmitInfo::e_type_remote_file_find_rpd);
	char lc_full_dir[4096+MAX_PATH]; lc_full_dir[0]=0;
	if(::GetFullPathName(_arg.strFilePath.c_str(),sizeof(lc_full_dir)-1,lc_full_dir,NULL)<=0)
	{//错误
		_tem.strFilePath=_arg.strFilePath;
		_tem.strWildcard=_arg.strWildcard;
		CWindowsWrapExt::getLastErrorText(&_tem.sErrorText);
	}
	else
	{//正确
		_tem.strFilePath.assign(lc_full_dir);
		_tem.strFilePath.replaceSelf('\\','/');
		if(_tem.strFilePath.backChar()!='\\')
			_tem.strFilePath.append("\\");
		_tem.strWildcard=_arg.strWildcard;
		_visitFileInfo(&_tem.listFileInfo,&_tem.sErrorText,_tem.strFilePath.c_str(),_tem.strWildcard.c_str());
		_tem.listFileInfo.sort();
	}
	CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&_tem,_uiPkgNum);
}


void CRemotFileServerManager::dealRemoteFileDelete(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_spSocket,
												   const CNsTransmitInfo::tagRequestRemoteFileDeleteFile &_arg,unsigned int _uiPkgNum)
{
	CNsTransmitInfo::tagRespondRemoteFileDeleteFile _tem;
	_tem.initBasic(CNsTransmitInfo::e_type_remote_file_delete_rpd);

	std::list<TxBlobString>::const_iterator iter=_arg.listName.begin();
	std::list<TxBlobString>::const_iterator iter_end=_arg.listName.end();
	TxBlobString file;
	TxBlobString strLcFilePath=_arg.strFilePath;
	strLcFilePath.replaceSelf('\\','/');
	if(strLcFilePath.backChar()!='\\')
		strLcFilePath.append("\\");
	std::string lcErrorText;
	for(;iter!=iter_end;iter++)
	{
		if(!CAfxFileFindExt::gDeleteAllFile(&lcErrorText,(strLcFilePath+(*iter)).c_str())&&_tem.sErrorText.length()<=0)
		{
			_tem.sErrorText.format("未能完全删除所有文件,删除【%s】等等文件失败!",
				(const char*)(strLcFilePath+(*iter)).c_str());
			if(lcErrorText.length()>0)
				_tem.sErrorText.appendFormat("\n原因: %s!",lcErrorText.c_str());
		}
	}
	CNsTransmitInfo::CShellPackData::pack_and_send(_spSocket,&_tem,_uiPkgNum);
}

