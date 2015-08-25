
#include <sys/types.h>
#include <sys/stat.h>
#include "CWindowsWrapExt.h"
#include "../common/TxBlobString.hpp"
#include <afx.h>
#include <string>


void CWindowsWrapExt::getLastErrorText(TxBlobString *_retErrText)
{
	std::string ret;
	getLastErrorText(&ret);
	_retErrText->assign(ret);
}

void CWindowsWrapExt::getLastErrorText(std::string *_retErrText) 
{
	_retErrText->clear();
	DWORD dwError=::GetLastError();
	if(dwError!=ERROR_SUCCESS)
	{
		LPVOID lpMsgBuf=NULL;
		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
		if(lpMsgBuf!=NULL)
		{
			_retErrText->assign((const char*)lpMsgBuf);
			while(_retErrText->length()>0)
			{
				int ch=_retErrText->at(_retErrText->size()-1);
				if(ch=='\n'||ch=='\r')
					_retErrText->resize(_retErrText->size()-1);
				else break;
			}
			LocalFree(lpMsgBuf);
		}
	}
}

bool CAfxFileFindExt::gFindAllFile(std::list<std::string> *_ret,std::string *_error,const char *_pstr,const char *_strWildcard)
{
	BOOL ret=FALSE;
	CFileFind finder;
	TxBlobString strWildcard(_pstr);
	strWildcard.replaceSelf('\\','/');
	if(strWildcard.backChar()!='\\') strWildcard.append("\\");
	strWildcard.append((_strWildcard!=NULL&&_strWildcard[0]!=0)?_strWildcard:"*");
	_ret->clear();
	if(finder.FindFile(strWildcard.c_str()))
	{
		BOOL bLcLoop=TRUE;
		while (bLcLoop)
		{
			bLcLoop=!!finder.FindNextFile();
			TxBlobString r_s((LPCTSTR)finder.GetFilePath());
			while(r_s.backChar()=='\\')
				r_s.pop_back(1);
			if(r_s.length()<=0) continue;
			const char *ppp=::strrchr(r_s.c_str(),'\\');
			if(ppp!=NULL&&ppp[0]=='\\'&&ppp[1]!=0&&(ppp[1]!='.'||ppp[2]!=0))
			{
				_ret->push_back(ppp+1);
			}
		}
		ret=TRUE;
	}
	else
	{
		if(_error!=NULL)
			CWindowsWrapExt::getLastErrorText(_error);
	}
	finder.Close();
	if(!ret&&_error!=NULL&&_error->length()<=0)
		_error->assign("查找文件时发生未知错误!");
	return !!ret;
}

bool CAfxFileFindExt::gDeleteAllFile(std::string *_errText,const char *_filename,int _iDir1File2)
{
	bool ret=true;
	switch(_iDir1File2)
	{
	case 1://路径
		{
			CFileFind finder;
			TxBlobString strWildcard(_filename);
			strWildcard.replaceSelf('\\','/');
			if(strWildcard.backChar()!='\\')
				strWildcard.append("\\");
			strWildcard.append("*");
			if(finder.FindFile(strWildcard.c_str()))
			{
				BOOL bLcLoop=TRUE;
				while (bLcLoop)
				{
					bLcLoop=finder.FindNextFile();
					if (finder.IsDots()) continue;
					TxBlobString r_s((LPCTSTR)finder.GetFilePath());
					if(r_s.length()<=0) continue;
					if(!gDeleteAllFile(_errText,r_s.c_str(),finder.IsDirectory()?1:2))
						ret=false;
				}
			}
			else
				ret=false;
			finder.Close();
			if(!::RemoveDirectoryA(_filename))
				ret=false;
		}
		break;
	case 2://文件
		if(::DeleteFileA(_filename)||(::SetFileAttributesA(_filename,FILE_ATTRIBUTE_NORMAL)&&::DeleteFileA(_filename)))
		{
		}
		else
		{
			if(_errText!=NULL)
				CWindowsWrapExt::getLastErrorText(_errText);
			ret=false;
		}
		break;
	default://未知
		if(::GetFileAttributesA(_filename)&FILE_ATTRIBUTE_DIRECTORY)
		{
			if(!gDeleteAllFile(_errText,_filename,1))
				ret=false;
		}
		else
		{
			if(!gDeleteAllFile(_errText,_filename,2))
				ret=false;
		}
		break;
	}
	return ret;
}

