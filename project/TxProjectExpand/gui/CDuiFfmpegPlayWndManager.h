#pragma once

#include "../common/TxSystemDependent.hpp"
#include "CDuiPlayVideoWndBasic.h"
#include "CDuiFfmpegPlayWndBasic.h"
#include "../common/TxAtomicInteger.h"

class CDuiFfmpegPlayWndManager: public CDuiPlayVideoWndBasic::IPlayVideoHandle, CDuiFfmpegPlayWndBasic::IThreadCallBack
{
public :
	class IThreadEvent
	{
	public:
		virtual void vfStartPlayEvent(bool _bGood)=0;
	};
private:
	CDuiPlayVideoWndBasic mVideoWnd;
	struct {
		TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic> spDecoderDev;
		TxSystemDependent::TxMutexWrap mDecoderMutex;
	} mDecoderOpCs;
	TxSystemDependent::TxThreadWrap mThread;
	TxSystemDependent::TxEventWrap mEvent;
	TxSystemDependent::TxTimeSpan<true> mTimeSpan;
	TxAtomicInteger<long long> atllPauseTime;
	LONG lAvPlayRatio;
	LONG bThreadRunning;
	LONG iThreadInitVideoMark;
	IThreadEvent *pIThreadEvent;
private:
	static void _static_thread_call_back_(void *_arg1,void *_arg2);
	void _thread_call_back_();
public :
	virtual void vfCtrlPrevFrame();
	virtual void vfCtrlNextFrame();
	virtual void vfCtrlPlayStart();
	virtual void vfCtrlPlayPause();
	virtual void vfCtrlPlayStop();
	virtual void vfCtrlAvAttr();
	virtual void vfCtrlSetProgress(float _r);
	virtual void vfCtrlOpenUri(const char *_uri);
private:
	virtual void vfPlayAvInitialize(bool _bSuccess);
public :
	CDuiFfmpegPlayWndManager(HWND _hParentWnd,IThreadEvent *_pIThreadEvent);
	~CDuiFfmpegPlayWndManager();
	void start(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> &_spVideoWnd);
	void stop(bool _bWndValid=true);
	void moveWindow(int _x,int _y,int _w,int _h);
	void showFullScreen(bool _bFull);
	void showWindowVisible(bool _bShow);
	void dealDestroyWindow();
};
