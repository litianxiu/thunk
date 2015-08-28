#pragma once

#include <UIlib.h>
#include <assert.h>

#include "../common/TxSystemDependent.hpp"

class CDuiPlayToolCtrlWnd: public DuiLib::WindowImplBase
{
public :
	class I_CALLBACK
	{
	public :
		virtual void clickFullScreen(bool _bFull)=0;
		virtual bool clickPrevFrame()=0;
		virtual bool clickNextFrame()=0;
		virtual bool clickPlayResume()=0;
		virtual bool clickPlayPause()=0;
		virtual void clickPlayStop()=0;
		virtual void clickAvAttr()=0;
		virtual void clickSetProgress(float _r)=0;
		//virtual void clickSetVolume(float _r)=0;
		//virtual void clickEnableVolume(bool _b)=0;
		virtual void clickOpenUri()=0;
	};
private:
	BOOL bInitControlUI;
	DuiLib::CButtonUI *p_btnAvAttr;
	DuiLib::CButtonUI *p_btnStop;
	DuiLib::CButtonUI *p_btnPrevFrame;
	DuiLib::CButtonUI *p_btnPlay;
	DuiLib::CButtonUI *p_btnPause;
	DuiLib::CButtonUI *p_btnNextFrame;
	DuiLib::CSliderUI *p_sliderAvPgr;
	DuiLib::CButtonUI *p_btnOpenUri;
	DuiLib::CButtonUI *p_btnScreenFull;
	DuiLib::CButtonUI *p_btnNoScreenFull;
	I_CALLBACK *p_cb_func;
	BOOL bMouseEnterWndShow;
	BOOL bProgressSliderDown;
	BOOL bBtnProgressSliderClick;
	POINT mLstMouseGlbPos;
	TxSystemDependent::TxTimeSpan<false> mRecordShowTimeSpan;
private:
	template<class T>
	void buildSaftWidgetPtr(T **_obj,LPCTSTR _str)
	{
		DuiLib::CControlUI* p_ui=m_PaintManager.FindControl(_str);
		*_obj=NULL;
		if(p_ui!=NULL)
		{
			*_obj=dynamic_cast<T*>(p_ui);
		}
		if((*_obj)==NULL)
			assert(0);
	}
public :
	CDuiPlayToolCtrlWnd(I_CALLBACK *_cbf);
	virtual LPCTSTR GetWindowClassName() const;
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual void InitWindow();
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void timerHitShow();
	bool onTheAvPgrChangeEvent(void *_arg);
	void setPlayProgress(float _fPlayProgress);
	void setPlayPauseCtrBtnVisual(bool _bShowPlayBtn);
	void setEnableAnyControl(bool _bEnableCtrl);
	void resetProgressSliderUi();
};
