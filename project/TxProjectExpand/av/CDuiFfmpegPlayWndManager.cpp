#include "stdafx.h"

#include "CDuiFfmpegPlayWndManager.h"
#include "../av/CDuiPlayVideoFfmpegAttrb.hpp"

CDuiFfmpegPlayWndManager::CDuiFfmpegPlayWndManager(HWND _hParentWnd,IThreadEvent *_pIThreadEvent):mVideoWnd(this)
{
	this->pIThreadEvent=_pIThreadEvent;
	this->lAvPlayRatio=0;
	this->atllPauseTime.setValue(-1);
	this->mVideoWnd.create(_hParentWnd);
	this->mVideoWnd.postStopVideoEvent();
}

CDuiFfmpegPlayWndManager::~CDuiFfmpegPlayWndManager()
{
	this->stop(false);
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
		else if(iii==1)
		{
			pThis->pIThreadEvent->vfStartPlayEvent(true);
			pThis->mVideoWnd.postToolSelectPlayPauseCtrBtnVisual(false);
			pThis->mVideoWnd.postEnableToolBarCtrlWindow(true);
			break;
		}
		else if(iii=-1)
		{
			pThis->pIThreadEvent->vfStartPlayEvent(false);
			pThis->mVideoWnd.postToolSelectPlayPauseCtrBtnVisual(true);
			pThis->mVideoWnd.postEnableToolBarCtrlWindow(false);
			pThis->bThreadRunning=FALSE;
			break;
		}
		else
		{
			assert(0);
		}
	}
	pThis->_thread_call_back_();
	pThis->mVideoWnd.postToolSelectPlayPauseCtrBtnVisual(true);
	pThis->mVideoWnd.postEnableToolBarCtrlWindow(false);
}

void CDuiFfmpegPlayWndManager::_thread_call_back_()
{
	TxCppPlatform::shared_ptr<CDirectSoundOutput> spLCDirectSoundOutput;
	TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> spLcDdFrame;
	TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> spLcAudioFrame;
	const long long ll_frame_tag_max_time=2*1000;
	long long lc_frame_time=0;
	float fLcPlayRatio=0;
	while(this->bThreadRunning)
	{
		if(this->atllPauseTime.getValue()>=0)
		{
			spLcDdFrame.reset();
			spLcAudioFrame.reset();
			this->mEvent.waitEvent();
		}
		else
		{
			this->mEvent.waitEvent(10);
			if(!this->bThreadRunning) break;
			if(!spLcDdFrame&&!spLcAudioFrame)
			{
				this->mDecoderOpCs.mDecoderMutex.lock();
				TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic> lc_spDecoderDev=this->mDecoderOpCs.spDecoderDev;
				CDuiFfmpegPlayWndBasic::EnumResultStatus eLcStatus=lc_spDecoderDev->readFrame(&fLcPlayRatio,&lc_frame_time,&spLcDdFrame,&spLcAudioFrame); (void)eLcStatus;
				this->mDecoderOpCs.mDecoderMutex.unlock();
				if(eLcStatus!=CDuiFfmpegPlayWndBasic::eResultSuccess)
				{
					lc_frame_time=(long long)(((unsigned long long)-1)>>2)-1;
					spLcDdFrame.reset();
					spLcAudioFrame.reset();
				}
			}
			if(spLcDdFrame||spLcAudioFrame)
			{
				long long lcCurTime=this->mTimeSpan.getCurrentMillisecond();
				if(lc_frame_time-lcCurTime>ll_frame_tag_max_time||lcCurTime-lc_frame_time>ll_frame_tag_max_time)
				{
					this->mTimeSpan.reset(lc_frame_time);
					lcCurTime=lc_frame_time;
				}
				if(lcCurTime>=lc_frame_time)
				{
					if(spLcDdFrame)
					{
						this->mVideoWnd.postPaintFrame(spLcDdFrame,fLcPlayRatio);
						lc_frame_time=(long long)(((unsigned long long)-1)>>2)-1;
						spLcDdFrame.reset();
					}
					if(spLcAudioFrame)
					{
						if(!spLCDirectSoundOutput)
						{
							spLCDirectSoundOutput.reset(new CDirectSoundOutput(this->mVideoWnd.GetHWND()));
							this->wpAudioWaveOutput=spLCDirectSoundOutput;
						}
						spLCDirectSoundOutput->push(spLcAudioFrame);
						lc_frame_time=(long long)(((unsigned long long)-1)>>2)-1;
						spLcAudioFrame.reset();
					}
				}
			}
		}
	}
	this->lAvPlayRatio=0;
}

bool CDuiFfmpegPlayWndManager::vfCtrlPrevFrame()
{
	return false;
}

bool CDuiFfmpegPlayWndManager::vfCtrlNextFrame()
{
	bool res=false;
	TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> spLcDdFrame;
	long long lc_frame_time=0;
	float fLcPlayRatio=0;
	this->mDecoderOpCs.mDecoderMutex.lock();

	TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic> lc_spDecoderDev=this->mDecoderOpCs.spDecoderDev;
	CDuiFfmpegPlayWndBasic::EnumResultStatus eLcStatus=lc_spDecoderDev->readFrame(&fLcPlayRatio,&lc_frame_time,&spLcDdFrame,NULL);
	if(spLcDdFrame&&eLcStatus==CDuiFfmpegPlayWndBasic::eResultSuccess)
	{
		this->atllPauseTime.setValue(lc_frame_time);
		this->mVideoWnd.postPaintFrame(spLcDdFrame,fLcPlayRatio);

		{
			TxCppPlatform::shared_ptr<CDirectSoundOutput> sp_lc_audio=this->wpAudioWaveOutput.lock();
			if(sp_lc_audio)
			{
				TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> lc_sp_elem_tem(new CDirectSoundOutput::tagAudioFrame());
				sp_lc_audio->push(lc_sp_elem_tem);
			}
		}
		res=true;

	}
	this->mDecoderOpCs.mDecoderMutex.unlock();
	return res;
}

bool CDuiFfmpegPlayWndManager::vfCtrlPlayResume()
{
	bool res=false;
	if(this->atllPauseTime.getValue()>0)
	{
		res=true;
		this->mTimeSpan.reset(this->atllPauseTime.getValue());
		this->atllPauseTime.setValue(-1);
		this->mEvent.setEvent();
	}
	return res;
}

bool CDuiFfmpegPlayWndManager::vfCtrlPlayPause()
{
	this->atllPauseTime.setValue(this->mTimeSpan.getCurrentMillisecond());
	return true;
}

void CDuiFfmpegPlayWndManager::vfCtrlPlayStop()
{
	this->stop();
}

void CDuiFfmpegPlayWndManager::vfCtrlAvAttr()
{
	CDuiPlayVideoFfmpegAttrb mLcDlg;
	mLcDlg.Create(this->mVideoWnd.GetHWND(),NULL,UI_WNDSTYLE_FRAME,0);
	mLcDlg.CenterWindow();
	{
		this->mDecoderOpCs.mDecoderMutex.lock();
		TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic> spLcDecoderDev=this->mDecoderOpCs.spDecoderDev;
		AVFormatContext *pLcAVFormatContext=spLcDecoderDev->getAVFormatContext();
		mLcDlg.setParameter(pLcAVFormatContext);
		this->mDecoderOpCs.mDecoderMutex.unlock();
	}
	mLcDlg.ShowModal();
}

void CDuiFfmpegPlayWndManager::vfCtrlSetProgress(float _r)
{
	this->mDecoderOpCs.mDecoderMutex.lock();
	TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic> spLcDecoderDev=this->mDecoderOpCs.spDecoderDev;
	if(spLcDecoderDev)
	{
		spLcDecoderDev->setPlayProgress(_r);
	}
	this->mDecoderOpCs.mDecoderMutex.unlock();
}

void CDuiFfmpegPlayWndManager::vfCtrlOpenUri(const char *_uri)
{
	class T_LocalFile_Read: public CDuiFfmpegPlayWndBasic::IThreadReadFile
	{
	private:
		HANDLE hFileHandle;
		std::string strFileName;
	public :
		T_LocalFile_Read(const char *_file)
		{
			this->strFileName.assign(_file);
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

bool CDuiFfmpegPlayWndManager::vfCtrlSetVolume(float _r)
{
	TxCppPlatform::shared_ptr<CDirectSoundOutput> sp(this->wpAudioWaveOutput.lock());
	if(sp)
	{
		sp->setVolumeRatio(_r);
		return true;
	}
	return false;
}

void CDuiFfmpegPlayWndManager::vfPlayAvInitialize(bool _bSuccess)
{
	if(_bSuccess) this->iThreadInitVideoMark=1;
	else this->iThreadInitVideoMark=-1;
	this->mEvent.setEvent();
}

void CDuiFfmpegPlayWndManager::start(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> &_spVideoWnd)
{
	this->stop(false);
	this->mDecoderOpCs.mDecoderMutex.lock();
	this->mDecoderOpCs.spDecoderDev.reset(new CDuiFfmpegPlayWndBasic(this,_spVideoWnd));
	this->mDecoderOpCs.mDecoderMutex.unlock();
	this->bThreadRunning=TRUE;
	this->iThreadInitVideoMark=0;
	this->lAvPlayRatio=0;
	this->mThread.create(_static_thread_call_back_,this,NULL);
}

void CDuiFfmpegPlayWndManager::stop(bool _bWndValid)
{
	this->bThreadRunning=FALSE;
	this->mEvent.setEvent();
	this->mThread.join();
	this->mDecoderOpCs.mDecoderMutex.lock();
	this->mDecoderOpCs.spDecoderDev.reset();
	this->mDecoderOpCs.mDecoderMutex.unlock();
	if(_bWndValid)
		this->mVideoWnd.postStopVideoEvent();
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

