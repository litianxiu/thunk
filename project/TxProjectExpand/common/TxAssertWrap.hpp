#pragma once

#include <Windows.h>
#include <atlstr.h>

class _My_AssertWrap_
{
public:
	static CString _chsToCString(const char *_str)
	{
		if(_str!=NULL)
		{
			WCHAR mBuff[1024];
			int len=MultiByteToWideChar(CP_ACP,0,_str,strlen(_str)+1,mBuff,  
				sizeof(mBuff)/sizeof(mBuff[0])-1);  
			if(len<=0) mBuff[0]=mBuff[1]=0;
			else mBuff[len]=0;
			return CString(mBuff);
		}
		return CString();
	}
	template<class T,class TTT>
	inline static T buildDynamicCast(TTT *_ptr)
	{
		assert(_ptr==NULL||(dynamic_cast<T>(_ptr))!=NULL);
		return _ptr!=NULL?(T)_ptr:(T)NULL;
	}
	static CString _getErrorTextByCode(DWORD _dwErrorCode)
	{
		CString ret;
		LPTSTR lpMsgBuf = NULL;
		FormatMessage(  
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, //dwFlags  
			NULL, //lpSource  
			_dwErrorCode, //dwMessageId  
			0, //GetUserDefaultLangID(),// MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),//dwLanguageId  
			(LPTSTR)&lpMsgBuf, //lpBuffer  
			0, //nSize  
			NULL //Arguments  
			);
		if(lpMsgBuf)
		{
			ret=lpMsgBuf;
			LocalFree(lpMsgBuf);
			lpMsgBuf=NULL;
		}
		return ret;
	}
	inline static void _messagebox(const char * _lpszText,const char *_file,int _line)
	{
		(LPCTSTR)_lpszText;
		(void*)_file;
		(void)_line;
		static BOOL bbbbb=TRUE;
		if(bbbbb)
		{
			bbbbb=FALSE;
			CString cs;
			DWORD dwWindowsErrorCode=::GetLastError();
			cs.Format(_T("[%s:%d]%s\r\nerror=[%d]--%s\r\n"),(LPCTSTR)_chsToCString(_file),(int)_line,(LPCTSTR)_chsToCString(_lpszText),
				(int)dwWindowsErrorCode,(LPCTSTR)_getErrorTextByCode(dwWindowsErrorCode));
			::MessageBox(NULL,cs,_T("error"),MB_OK);
			OutputDebugString(cs);
		}
	}
};

#ifndef NDEBUG
#define DefAssertTextWrap(_B,_text) ((void)((_B)||(_My_AssertWrap_::_messagebox(_text,__FILE__,__LINE__),TRUE)))
#define DefAssertWrap(_B) ((void)((_B)||(_My_AssertWrap_::_messagebox(#_B,__FILE__,__LINE__),TRUE)))
#else
#define DefAssertTextWrap(_B,_text) ((void)0)
#define DefAssertWrap(_B) ((void)0)
// #define DefAssertTextWrap(_B,_text) ((void)((_B)||(_My_IocpGlobalFunction_::_messagebox(_text,__FILE__,__LINE__),TRUE)))
#endif

