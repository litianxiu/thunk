#pragma once

#include "../common/TxSystemDependent.hpp"
#include "CDuiPlayVideoWndBasic.h"
#include "CDuiFfmpegPlayWndBasic.h"
#include "../common/TxAtomicInteger.h"

class CDuiFfmpegPlayWndManager: public CDuiPlayVideoWndBasic::IPlayVideoHandle, CDuiFfmpegPlayWndBasic::IThreadCallBack
{
private:
	CDuiPlayVideoWndBasic mVideoWnd;
	TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic> spDecoderDev;
	TxSystemDependent::TxThreadWrap mThread;
	TxSystemDependent::TxEventWrap mEvent;
	TxSystemDependent::TxTimeSpan<true> mTimeSpan;
	TxAtomicInteger<long long> atllPauseTime;
	LONG bThreadRunning;
	LONG iThreadInitVideoMark;
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
	CDuiFfmpegPlayWndManager(HWND _hParentWnd);
	~CDuiFfmpegPlayWndManager();
	void start(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> &_spVideoWnd);
	void stop();
	void moveWindow(int _x,int _y,int _w,int _h);
	void showFullScreen(bool _bFull);
	void showWindowVisible(bool _bShow);
};
