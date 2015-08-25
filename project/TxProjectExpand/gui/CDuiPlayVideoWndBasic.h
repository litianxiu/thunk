#pragma once

#include <queue>
#include <map>
#include "CDuiPlayToolCtrlWnd.h"
#include "CGuiDirectDrawDisplay.hpp"
#include "../common/TxGlobalConfig.h"
#include "../common/TxBlobString.hpp"
#include "../common/TxSystemDependent.hpp"


class CDuiPlayVideoWndBasic: protected DuiLib::WindowImplBase, public TxForbidDirectCopyObject, public CDuiPlayToolCtrlWnd::I_CALLBACK
{
public :
	struct tagUnitInfo {
		TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> spDdFrame;
		tagUnitInfo(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> &_spDdFrame):spDdFrame(_spDdFrame) { }
	};
	class IPlayVideoHandle
	{
	public :
		virtual void vfCtrlPrevFrame()=0;
		virtual void vfCtrlNextFrame()=0;
		virtual void vfCtrlPlayStart()=0;
		virtual void vfCtrlPlayPause()=0;
		virtual void vfCtrlPlayStop()=0;
		virtual void vfCtrlAvAttr()=0;
		virtual void vfCtrlSetVolume(float _r)=0;
		virtual void vfCtrlEnableVolume(bool _b)=0;
		virtual void vfCtrlOpenUri(const char *_uri)=0;
	};
private:
	CDuiPlayToolCtrlWnd m_DuiPlayToolCtrlWnd;
	//std::list<TxCppPlatform::shared_ptr<CDirectDrawFrameFormat>> mListFreeFrame;
	std::map<long long,tagUnitInfo> mapThreadFrame;
	TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> spCurrentFrame;
	CGuiDirectDrawDisplay *pGuiDirectDrawDisplay;
	HWND mParentWnd;
	TxSystemDependent::TxThreadWrap mThread;
	TxSystemDependent::TxEventWrap mEvent;
	TxSystemDependent::TxMutexWrap mMutex;
	//TxSystemDependent::TxMutexWrap mListFreeFrameMutex;
	TxSystemDependent::TxTimeSpan<true> mTimeSpan;
	RECT mWndScreenRect;
	BOOL bThreadRunning;
	UINT uiTimerId_1000;
	IPlayVideoHandle *const p_IPlayVideoHandle;
	enum { e_img_count=128, e_post_paint_id=(WM_USER|WM_APP)+100, };
private:
	void _relase_resc_();
	static void _static_call_back_(void *_arg1,void *_arg2);
	void _call_back_();
	void _moveWindow_ext_(HWND _hParentWnd,int _x,int _y,int _w,int _h);
	virtual void vfThreadReadFrame(std::list<std::pair<long long,tagUnitInfo>> *_listSpFrame,int _iListSize)=0;
private:
	virtual void clickFullScreen(bool _bFull);
	virtual void clickPrevFrame();
	virtual void clickNextFrame();
	virtual void clickPlayStart();
	virtual void clickPlayPause();
	virtual void clickPlayStop();
	virtual void clickAvAttr();
	virtual void clickSetVolume(float _r);
	virtual void clickEnableVolume(bool _b);
	virtual void clickOpenUri();
public :
	CDuiPlayVideoWndBasic(CDuiPlayVideoWndBasic::IPlayVideoHandle *_p_IPlayVideoHandle);
	~CDuiPlayVideoWndBasic();
	virtual LPCTSTR GetWindowClassName() const;
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	bool dtctThreadRunningStatus() const;
	void start();
	void stop();
	bool create(HWND _parentWnd);
	void moveWindow(int _x,int _y,int _w,int _h);
	//TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> newDataCell();
	void refresh_frame_gui();
	void showFullScreen(bool _bFull);
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled);
	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam);
	virtual void OnFinalMessage(HWND hWnd);
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	void showWindowVisible(bool _bShow);
	void postPaintFrame(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> &_spLcDdFrame) { }
};
