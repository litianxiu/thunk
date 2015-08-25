#pragma once

#include <Windows.h>
#include <process.h>
#include <shellapi.h>
#include "TxGlobalConfig.h"
#include "TxAssertWrap.hpp"

namespace TxSystemDependent {

class TxThreadWrap: private TxForbidDirectCopyObject
{
private:
	HANDLE hThread;//线程句柄
	unsigned threadID;
	void *pThreadArg[2];
	void (*pThreadCallBack)(void *_arg1,void *_arg2);
private:
	static unsigned __stdcall _static_start_address( void* pArguments )
	{
		{
			TxThreadWrap* pThis=((TxThreadWrap*)pArguments);
			pThis->pThreadCallBack(pThis->pThreadArg[0],pThis->pThreadArg[1]);
		}
		::_endthreadex(0);
		return 0;
	}
public:
	TxThreadWrap():hThread(NULL),threadID(0) { }
	virtual ~TxThreadWrap() { this->release(); }
	inline DWORD getThreadId() const { return (DWORD)this->threadID; }
	void create(void (*_cb)(void*,void*),void *_arg1,void *_arg2)
	{
		this->release();
		this->pThreadArg[0]=_arg1;
		this->pThreadArg[1]=_arg2;
		this->pThreadCallBack=_cb;
		hThread = (HANDLE)::_beginthreadex( NULL,0,&_static_start_address, this, 0, &this->threadID );
		DefAssertTextWrap(hThread!=NULL,"_beginthreadex() failure!");
	}
	void release()
	{
		if(hThread!=NULL)
		{
			BOOL b1=TRUE,b2=TRUE;
			this->join();
			b2=CloseHandle(hThread);
			hThread= NULL;
			DefAssertTextWrap(b1&&b2,"CloseHandle(thread) failure!");
		}
	}
	void join()
	{
		if(this->hThread!=NULL)
			::WaitForSingleObject(hThread,INFINITE);
	}
};

class TxMutexWrap: private TxForbidDirectCopyObject
{
private:
	CRITICAL_SECTION m_cs;
public:
	TxMutexWrap()
	{
		::memset(&this->m_cs,0,sizeof(this->m_cs));
		::InitializeCriticalSection(&this->m_cs);
	}
	~TxMutexWrap()
	{
		::DeleteCriticalSection(&this->m_cs);
	}
	void lock()
	{
		::EnterCriticalSection(&this->m_cs);
	}
	void unlock()
	{
		::LeaveCriticalSection(&this->m_cs);
	}
};

class TxAutoLocker: private TxForbidDirectCopyObject
{
private:
	TxMutexWrap *const pMutex;
private:
	TxAutoLocker& operator=(const TxAutoLocker&) const
	{
		return *(TxAutoLocker*)NULL;
	}
public:
	TxAutoLocker(TxMutexWrap *_pMutex):pMutex(_pMutex)
	{
		this->pMutex->lock();
	}
	TxAutoLocker(TxMutexWrap &_mMutex):pMutex(&_mMutex)
	{
		this->pMutex->lock();
	}
	TxAutoLocker(TxAutoLocker &_tem):pMutex(_tem.pMutex)
	{
		this->pMutex->lock();
	}
	~TxAutoLocker()
	{
		this->pMutex->unlock();
	}
};

class TxEventWrap: private TxForbidDirectCopyObject
{
private:
	HANDLE hEvent;
public :
	TxEventWrap()
	{
		this->hEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
	}
	~TxEventWrap()
	{
		::CloseHandle (this->hEvent);
	}
	void resetEvent()
	{
		::ResetEvent(this->hEvent);
	}
	void setEvent()
	{
		::SetEvent(this->hEvent);
	}
	bool waitEvent(DWORD _dwMilliseconds=INFINITE)
	{
#ifndef NDEBUG
		DWORD tem=::WaitForSingleObject(this->hEvent,_dwMilliseconds);
		bool ret=false;
		switch(tem)
		{
		case WAIT_OBJECT_0:
			ret=true;
			tem=0;
			break;
		case WAIT_ABANDONED:
			tem=0;
			//DefAssertWrap(FALSE);
			break;
		case WAIT_TIMEOUT:
			tem=0;
			//DefAssertWrap(FALSE);
			break;
		default:
			tem=0;
			//DefAssertWrap(FALSE);
			break;
		}
		return ret;
#else
		return WAIT_OBJECT_0==::WaitForSingleObject(this->hEvent,_dwMilliseconds);
#endif
	}
};

template<bool B_ACTIVE_MUTEX>
class TxTimeSpan //时间段
{
private:
	long long curTime;
	long long lstTime;
	char buff_mutex__tem__[B_ACTIVE_MUTEX?sizeof(TxMutexWrap):sizeof(long long)];
private:
	inline void _lock_obj()
	{
		if(B_ACTIVE_MUTEX)
			((TxMutexWrap*)&this->buff_mutex__tem__[0])->lock();
	}
	inline void _unlock_obj()
	{
		if(B_ACTIVE_MUTEX)
			((TxMutexWrap*)&this->buff_mutex__tem__[0])->unlock();
	}
public:
	TxTimeSpan(long long _init_time=0)
	{
		if(B_ACTIVE_MUTEX)
			new ((TxMutexWrap*)&this->buff_mutex__tem__[0]) TxSystemDependent::TxMutexWrap();
		this->reset(_init_time);
	}
	~TxTimeSpan()
	{
		if(B_ACTIVE_MUTEX)
			((TxMutexWrap*)&this->buff_mutex__tem__[0])->~TxMutexWrap();
	}
	void reset(long long _curTime=0)
	{
		DWORD ttt=::GetTickCount();
		this->_lock_obj();
		this->lstTime=ttt;
		this->curTime=(long long)(_curTime);
		this->_unlock_obj();
	}
	inline long long getCurrentMillisecond()
	{
		long long tc=::GetTickCount();
		this->_lock_obj();
		this->curTime+=(tc>lstTime)?(tc-lstTime):0;
		this->lstTime=tc;
		long long ret=this->curTime;
		this->_unlock_obj();
		return ret;
	}
	bool dtctCurTimeResetSpanMsec(long long _outTime,bool _bClearSurplusTime=false)
	{
		bool ret=false;
		long long tc=::GetTickCount();
		this->_lock_obj();
		this->curTime+=(tc>lstTime)?(tc-lstTime):0;
		this->lstTime=tc;
		if(this->curTime>=(long long)(_outTime))
		{
			ret=true;
			if(_bClearSurplusTime) this->curTime=0;
			else this->curTime-=(long long)(_outTime);
		}
		this->_unlock_obj();
		return ret;
	}
};

class TxRuntimeWrap
{
public :
	static LPCTSTR getWorkingDirectory()
	{
		struct T_R
		{
			enum { e_arry_size=(FILENAME_MAX|MAX_PATH)+sizeof(long), };
			CHAR sWorkDir[e_arry_size];
			T_R()
			{
				::memset(this->sWorkDir,0,sizeof(this->sWorkDir));
				CHAR szLcPathTem_[e_arry_size]={0};
				::GetModuleFileName(NULL,szLcPathTem_,e_arry_size-1);
				CHAR szLcPath[e_arry_size]={0};
				::GetFullPathName(szLcPathTem_,e_arry_size-1,szLcPath,NULL);
				for(int i=0;szLcPath[i]!=NULL;i++)
				{
					if(szLcPath[i]=='/')
						szLcPath[i]='\\';
				}
				int pos=-1;
				for(int i=0;szLcPath[i]!=NULL;i++)
				{
					if(szLcPath[i]=='\\')
						pos=i;
				}
				if(pos>=1&&szLcPath[pos]=='\\')
				{
					szLcPath[pos+1]=0;
				}
				else
				{
					::StrCat(szLcPath,"\\");
				}
				::StrCpy(this->sWorkDir,szLcPath);
			}
		};
		static T_R _tem__;
		return _tem__.sWorkDir;
	}
	static void openExplorerSelectFile(LPCTSTR _strFilePath)
	{
		CString csLcDir(_strFilePath);
		for(int nn=(int)csLcDir.GetLength();nn--;)
		{
			if(csLcDir.GetAt(nn)==_T('/'))
				csLcDir.SetAt(nn,_T('\\'));
		}
		if(csLcDir.GetLength()<=0||csLcDir.GetAt(csLcDir.GetLength()-1)!=_T('\\'))
			csLcDir+=_T("\\");
		for(int yy=(int)csLcDir.GetLength();yy--;)
		{
			if(csLcDir.GetAt(yy)==_T('\\'))
			{
				CString sss=csLcDir.Left(yy);
				DWORD lc_dw_ret=::GetFileAttributes((LPCTSTR)sss);
				if(lc_dw_ret==INVALID_FILE_ATTRIBUTES)
				{
					continue;
				}
				else if(lc_dw_ret&FILE_ATTRIBUTE_DIRECTORY)
				{
					::ShellExecute(NULL,_T("open"),(LPCTSTR)sss,NULL,NULL,SW_SHOW);
					break;
				}
				else
				{
					CString szParam(_T("/e,/select, "));
					szParam.Append(sss);
					::ShellExecute(NULL,_T("open"),_T("explorer"),szParam,NULL,SW_SHOW);
					break;
				}
			}
		}
	}
	static void openDirectoryOrFile(LPCTSTR _strPath)
	{
		CString csLcPath(_strPath);
		for(int i=(int)csLcPath.GetLength();i--;)
		{
			if(csLcPath.GetAt(i)==_T('/'))
				csLcPath.SetAt(i,_T('\\'));
		}
		if(csLcPath.GetLength()<=0||csLcPath.GetAt(csLcPath.GetLength()-1)!=_T('\\'))
			csLcPath+=_T("\\");
		for(int yy=(int)csLcPath.GetLength();yy--;)
		{
			if(csLcPath.GetAt(yy)==_T('\\'))
			{
				CString sss=csLcPath.Left(yy);
				DWORD lc_dw_ret=::GetFileAttributes((LPCTSTR)sss);
				if(lc_dw_ret==INVALID_FILE_ATTRIBUTES)
				{
					continue;
				}
				else
				{
					::ShellExecute(NULL,_T("open"),(LPCTSTR)sss,NULL,NULL,SW_SHOW);
					break;
				}
			}
		}
	}
};

}
