#include "stdafx.h"

#include "../gui/CDuiPlayVideoAttrb.h"
#include "CDuiFfmpegPlayWndManager.h"

CDuiFfmpegPlayWndManager::CDuiFfmpegPlayWndManager(HWND _hParentWnd):mVideoWnd(this)
{
	this->lAvPlayRatio=0;
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
	CDuiFfmpegPlayWndManager *pThis=(CDuiFfmpegPlayWndManager*)_arg1;
	for(;;)
	{
		int iii=(int)(pThis->iThreadInitVideoMark);
		if(iii==0)
		{
			pThis->mEvent.waitEvent();
		}
		else if(iii=1)
		{
			break;
		}
		else if(iii=-1)
		{
			pThis->bThreadRunning=FALSE;
			break;
		}
		else
		{
			assert(0);
		}
	}
	pThis->_thread_call_back_();
}

void CDuiFfmpegPlayWndManager::_thread_call_back_()
{
	TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> spLcDdFrame;
	const long long ll_frame_tag_max_time=2*1000;
	long long lc_frame_time=0;
	float fLcPlayRatio=0;
	//EnumResultStatus readFrame(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,long long *_ll_time);
	while(this->bThreadRunning)
	{
		if(this->atllPauseTime.getValue()>=0)
		{
			this->mEvent.waitEvent();
		}
		else
		{
			this->mEvent.waitEvent(10);
			if(!this->bThreadRunning) break;
			if(!spLcDdFrame)
			{
				CDuiFfmpegPlayWndBasic::EnumResultStatus eLcStatus=this->spDecoderDev->readFrame(&spLcDdFrame,&lc_frame_time,&fLcPlayRatio);
				if(eLcStatus!=CDuiFfmpegPlayWndBasic::eResultSuccess)
				{
					lc_frame_time=(long long)(((unsigned long long)-1)>>2)-1;
					spLcDdFrame.reset();
				}
			}
			if(spLcDdFrame)
			{
				long long lcCurTime=this->mTimeSpan.getCurrentMillisecond();
				if(lc_frame_time-lcCurTime>ll_frame_tag_max_time||lcCurTime-lc_frame_time>ll_frame_tag_max_time)
				{
					this->mTimeSpan.reset(lc_frame_time);
					lcCurTime=lc_frame_time;
				}
				if(lcCurTime>=lc_frame_time)
				{
					this->mVideoWnd.postPaintFrame(spLcDdFrame,fLcPlayRatio);
					lc_frame_time=(long long)(((unsigned long long)-1)>>2)-1;
					spLcDdFrame.reset();
				}
			}
		}
	}
	this->lAvPlayRatio=0;
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
	mLcDlg.CenterWindow();
	mLcDlg.ShowModal();
}

void CDuiFfmpegPlayWndManager::vfCtrlSetProgress(float _r)
{
	TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic> spLcDecoderDev=this->spDecoderDev;
	if(spLcDecoderDev)
	{
		spLcDecoderDev->setPlayProgress(_r);
	}
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

void CDuiFfmpegPlayWndManager::vfPlayAvInitialize(bool _bSuccess)
{
	if(_bSuccess) this->iThreadInitVideoMark=1;
	else this->iThreadInitVideoMark=-1;
	this->mEvent.setEvent();
}

void CDuiFfmpegPlayWndManager::start(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> &_spVideoWnd)
{
	this->stop();
	this->spDecoderDev.reset(new CDuiFfmpegPlayWndBasic(this,_spVideoWnd));
	this->bThreadRunning=TRUE;
	this->iThreadInitVideoMark=0;
	this->lAvPlayRatio=0;
	this->mThread.create(_static_thread_call_back_,this,NULL);
}

void CDuiFfmpegPlayWndManager::stop()
{
	this->bThreadRunning=FALSE;
	this->mEvent.setEvent();
	this->mThread.join();
	this->spDecoderDev.reset();
	this->iThreadInitVideoMark=0;
	this->lAvPlayRatio=0;
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

