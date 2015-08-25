#include "stdafx.h"

#include "../gui/CDuiPlayVideoAttrb.h"
#include "CDuiFfmpegPlayWndManager.h"

CDuiFfmpegPlayWndManager::CDuiFfmpegPlayWndManager(HWND _hParentWnd):mVideoWnd(this)
{
	this->atllPauseTime.setValue(-1);
	this->mVideoWnd.create(_hParentWnd);
}

CDuiFfmpegPlayWndManager::~CDuiFfmpegPlayWndManager()
{
	this->stop();
}

void CDuiFfmpegPlayWndManager::_static_thread_call_back_(void *_arg1,void *_arg2)
{
	(void)_arg1;
	(void)_arg2;
	((CDuiFfmpegPlayWndManager*)_arg1)->_thread_call_back_();
}

void CDuiFfmpegPlayWndManager::_thread_call_back_()
{
	TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> spLcDdFrame;
	long long lc_time=0;
	//EnumResultStatus readFrame(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,long long *_ll_time);
	while(this->bThreadRunning)
	{
		if(this->atllPauseTime.getValue()<0)
		{
			this->mEvent.waitEvent();
		}
		else
		{
			this->mEvent.waitEvent(10);
			if(!this->bThreadRunning) break;
			CDuiFfmpegPlayWndBasic::EnumResultStatus eLcStatus=this->spDecoderDev->readFrame(&spLcDdFrame,&lc_time);
			if(eLcStatus!=CDuiFfmpegPlayWndBasic::eResultOffline)
			{
				lc_time=(long long)(((unsigned long long)-1)>>2)-1;
			}
			if(spLcDdFrame&&this->mTimeSpan.getCurrentMillisecond()>=lc_time)
			{
				this->mVideoWnd.postPaintFrame(spLcDdFrame);
				lc_time=(long long)(((unsigned long long)-1)>>2)-1;
				spLcDdFrame.reset();
			}
		}
	}
}

void CDuiFfmpegPlayWndManager::vfCtrlPrevFrame()
{
}

void CDuiFfmpegPlayWndManager::vfCtrlNextFrame()
{
}

void CDuiFfmpegPlayWndManager::vfCtrlPlayStart()
{
	if(this->atllPauseTime.getValue()>0)
	{
		this->mTimeSpan.reset(this->atllPauseTime.getValue());
		this->atllPauseTime.setValue(-1);
		this->mEvent.setEvent();
	}
}

void CDuiFfmpegPlayWndManager::vfCtrlPlayPause()
{
	this->atllPauseTime.setValue(this->mTimeSpan.getCurrentMillisecond());
}

void CDuiFfmpegPlayWndManager::vfCtrlPlayStop()
{
}

void CDuiFfmpegPlayWndManager::vfCtrlAvAttr()
{
	CDuiPlayVideoAttrb mLcDlg;
	mLcDlg.Create(this->mVideoWnd.GetHWND(),NULL,UI_WNDSTYLE_FRAME,0);
	mLcDlg.ShowModal();
}

void CDuiFfmpegPlayWndManager::vfCtrlSetVolume(float _r)
{
}

void CDuiFfmpegPlayWndManager::vfCtrlEnableVolume(bool _b)
{
}

void CDuiFfmpegPlayWndManager::vfCtrlOpenUri(const char *_uri)
{
	class T_LocalFile_Read: public CDuiFfmpegPlayWndBasic::IThreadReadFile
	{
	private:
		HANDLE hFileHandle;
	public :
		T_LocalFile_Read(const char *_file)
		{
			this->hFileHandle=::CreateFileA(
				_file,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL
				);
			assert(this->hFileHandle!=INVALID_HANDLE_VALUE);
		}
		~T_LocalFile_Read()
		{
			assert(this->hFileHandle!=INVALID_HANDLE_VALUE);
			if(this->hFileHandle!=INVALID_HANDLE_VALUE)
			{
				::CloseHandle(this->hFileHandle);
				this->hFileHandle=INVALID_HANDLE_VALUE;
			}
		}
		virtual int vfReadStream(void *_buff,int _size,long long _llFilePos)
		{
			assert(this->hFileHandle!=INVALID_HANDLE_VALUE);
			int ret=0;
			if(this->hFileHandle!=INVALID_HANDLE_VALUE)
			{
				DWORD dwCount=0;
				OVERLAPPED lv_ovlpd={0};
				lv_ovlpd.Offset=(DWORD)_llFilePos;
				lv_ovlpd.OffsetHigh=(DWORD)(((unsigned long long)_llFilePos)>>(8*sizeof(DWORD)));
				if(::ReadFile(this->hFileHandle,_buff,(DWORD)_size,&dwCount,&lv_ovlpd)||dwCount>0)
					ret=(int)dwCount;
			}
			return ret;
		}
	};
	this->start(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile>(new T_LocalFile_Read(_uri)));
}

void CDuiFfmpegPlayWndManager::start(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> &_spVideoWnd)
{
	this->stop();
	this->spDecoderDev.reset(new CDuiFfmpegPlayWndBasic(_spVideoWnd));
}

void CDuiFfmpegPlayWndManager::stop()
{
	this->bThreadRunning=FALSE;
	this->mEvent.setEvent();
	this->mThread.join();
	this->spDecoderDev.reset();
}

void CDuiFfmpegPlayWndManager::moveWindow(int _x,int _y,int _w,int _h)
{
	return this->mVideoWnd.moveWindow(_x,_y,_w,_h);
}

void CDuiFfmpegPlayWndManager::showFullScreen(bool _bFull)
{
	return this->mVideoWnd.showFullScreen(_bFull);
}

void CDuiFfmpegPlayWndManager::showWindowVisible(bool _bShow)
{
	return this->mVideoWnd.showWindowVisible(_bShow);
}

