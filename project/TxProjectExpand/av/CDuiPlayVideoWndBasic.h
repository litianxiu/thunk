#pragma once

#include <queue>
#include <map>
#include "CDuiPlayToolCtrlWnd.h"
#include "CGuiDirectDrawDisplay.hpp"
#include "../common/TxGlobalConfig.h"
#include "../common/TxBlobString.hpp"
#include "../common/TxSystemDependent.hpp"


class CDuiPlayVideoWndBasic: public DuiLib::WindowImplBase, public TxForbidDirectCopyObject, public CDuiPlayToolCtrlWnd::I_CALLBACK
{
public :
	class IPlayVideoHandle
	{
	public :
		virtual bool vfCtrlPrevFrame()=0;
		virtual bool vfCtrlNextFrame()=0;
		virtual bool vfCtrlPlayResume()=0;
		virtual bool vfCtrlPlayPause()=0;
		virtual void vfCtrlPlayStop()=0;
		virtual void vfCtrlAvAttr()=0;
		virtual void vfCtrlSetProgress(float _r)=0;
		virtual void vfCtrlOpenUri(const char *_uri)=0;
		virtual bool vfCtrlSetVolume(float _r)=0;
	};
private:
	CDuiPlayToolCtrlWnd m_DuiPlayToolCtrlWnd;
	//std::list<TxCppPlatform::shared_ptr<CDirectDrawFrameFormat>> mListFreeFrame;
	//std::map<long long,tagUnitInfo> mapThreadFrame;
	TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> spCurrentFrame;
	float fPlayProgress;
	TxSystemDependent::TxMutexWrap mFrameMutex;
	CGuiDirectDrawDisplay *pGuiDirectDrawDisplay;
	HWND mParentWnd;
	int iVideoAttrDlgShowCount;
	//TxSystemDependent::TxThreadWrap mThread;
	//TxSystemDependent::TxEventWrap mEvent;
	//TxSystemDependent::TxMutexWrap mMutex;
	//TxSystemDependent::TxMutexWrap mListFreeFrameMutex;
	//TxSystemDependent::TxTimeSpan<true> mTimeSpan;
	UINT uiTimerId_1000;
	RECT mWndScreenRect;
	//BOOL bThreadRunning;
	IPlayVideoHandle *const p_IPlayVideoHandle;
	enum { e_img_count=128, e_post_paint_id=(WM_USER|WM_APP)+100, e_post_task_id, };
	struct {
		std::list<TxCppPlatform::function<void(void)>> mListTask;
		TxSystemDependent::TxMutexWrap mTaskMutex;
	} mPostTaskOpCs;
private:
	void _relase_resc_();
	//static void _static_call_back_(void *_arg1,void *_arg2);
	//void _call_back_();
	void _moveWindow_ext_(HWND _hParentWnd,int _x,int _y,int _w,int _h);
	void _addPostTask_(TxCppPlatform::function<void(void)> _spFunc);
private:
	virtual void clickFullScreen(bool _bFull);
	virtual bool clickPrevFrame();
	virtual bool clickNextFrame();
	virtual bool clickPlayResume();
	virtual bool clickPlayPause();
	virtual void clickPlayStop();
	virtual void clickAvAttr();
	virtual void clickSetProgress(float _r);
	virtual void clickOpenUri();
	virtual bool clickSetVolume(float _r);
public :
	CDuiPlayVideoWndBasic(CDuiPlayVideoWndBasic::IPlayVideoHandle *_p_IPlayVideoHandle);
	~CDuiPlayVideoWndBasic();
	virtual LPCTSTR GetWindowClassName() const;
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled);
	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam);
	virtual void OnFinalMessage(HWND hWnd);
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	//bool dtctThreadRunningStatus() const;
	//void start();
	//void stop();
	bool create(HWND _parentWnd);
	void moveWindow(int _x,int _y,int _w,int _h);
	void myDoDropEvent(HDROP _hDrop);
	//TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> newDataCell();
	//void refresh_frame_gui();
	void showFullScreen(bool _bFull);
	void showWindowVisible(bool _bShow);
	void postPaintFrame(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> &_spLcDdFrame,float _fPlayProgress);
	void postAudioFrame(TxCppPlatform::shared_ptr<TxBlobString> &_spAudioFrame,float _fPlayProgress) { }
	void postStopVideoEvent();
	void postToolSelectPlayPauseCtrBtnVisual(bool _bShowPlayBtn);
	void postEnableToolBarCtrlWindow(bool _bEnable);
};
