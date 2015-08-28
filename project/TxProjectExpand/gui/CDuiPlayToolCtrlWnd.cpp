#include "stdafx.h"
#include "CDuiPlayToolCtrlWnd.h"


CDuiPlayToolCtrlWnd::CDuiPlayToolCtrlWnd(I_CALLBACK *_cbf):p_cb_func(_cbf)
{
	this->bInitControlUI=FALSE;
	this->bMouseEnterWndShow=FALSE;
	this->bProgressSliderDown=FALSE;
	this->bBtnProgressSliderClick=FALSE;
	this->mLstMouseGlbPos.x=this->mLstMouseGlbPos.y=1000000;
}

LPCTSTR CDuiPlayToolCtrlWnd::GetWindowClassName() const
{
	return _T("CDuiPlayToolCtrlWnd");
}

DuiLib::CDuiString CDuiPlayToolCtrlWnd::GetSkinFolder()
{
	return DefDuiSkin;
}

DuiLib::CDuiString CDuiPlayToolCtrlWnd::GetSkinFile()
{
	return _T("dui_play_tool_ctrl.xml");
}

void CDuiPlayToolCtrlWnd::InitWindow()
{
	assert(this->bInitControlUI==FALSE);
	if(this->bInitControlUI==FALSE)
	{
		this->buildSaftWidgetPtr(&this->p_btnAvAttr,_T("btnAvAttr"));
		this->buildSaftWidgetPtr(&this->p_btnStop,_T("btnStop"));
		this->buildSaftWidgetPtr(&this->p_btnPrevFrame,_T("btnPrevFrame"));
		this->buildSaftWidgetPtr(&this->p_btnPlay,_T("btnPlay"));
		this->buildSaftWidgetPtr(&this->p_btnPause,_T("btnPause"));
		this->buildSaftWidgetPtr(&this->p_btnNextFrame,_T("btnNextFrame"));
		this->buildSaftWidgetPtr(&this->p_sliderAvPgr,_T("sliderAvPgr"));
		this->buildSaftWidgetPtr(&this->p_btnOpenUri,_T("btnOpenUri"));
		this->buildSaftWidgetPtr(&this->p_btnScreenFull,_T("btnScreenFull"));
		this->buildSaftWidgetPtr(&this->p_btnNoScreenFull,_T("btnNoScreenFull"));
		this->bInitControlUI=TRUE;
		p_sliderAvPgr->OnEvent+=DuiLib::MakeDelegate(this,&CDuiPlayToolCtrlWnd::onTheAvPgrChangeEvent);
	}
	return __super::InitWindow();
}

LRESULT CDuiPlayToolCtrlWnd::MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled)
{
	switch(_uMsg)
	{
	case WM_KEYDOWN:
		if(_wParam==VK_ESCAPE||_wParam==VK_CANCEL||_wParam==VK_RETURN)
		{
			_bHandled=true;
			this->p_cb_func->clickFullScreen(false);
			return S_OK;
		}
		break;
	default:
		break;
	}
	return __super::MessageHandler(_uMsg, _wParam, _lParam,_bHandled);
}

LRESULT CDuiPlayToolCtrlWnd::HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch(_uMsg)
	{
	case WM_MOUSEMOVE:
		break;
	case WM_MOUSELEAVE:
		break;
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		break;
	default:
		break;
	}
	return __super::HandleMessage(_uMsg,_wParam,_lParam);
}

bool CDuiPlayToolCtrlWnd::onTheAvPgrChangeEvent(void *_arg)
{
	DuiLib::TEventUI *lcEvent=(DuiLib::TEventUI*)_arg;
	switch(lcEvent->Type)
	{
	case DuiLib::UIEVENT_BUTTONDOWN:
	case DuiLib::UIEVENT_RBUTTONDOWN:
		this->bProgressSliderDown=TRUE;
		this->bBtnProgressSliderClick=TRUE;
		break;
	case DuiLib::UIEVENT_DBLCLICK:
	case DuiLib::UIEVENT_BUTTONUP:
		this->bProgressSliderDown=FALSE;
		break;
	default:
		break;
	}
	return true;
}

void CDuiPlayToolCtrlWnd::timerHitShow()
{
	POINT lc_mouse_pt={0};
	POINT lc_wnd_pt={0};
	RECT lc_wnd_rect={0};
	if(::GetCursorPos(&lc_mouse_pt))
	{
		if(::GetWindowRect(this->GetHWND(),&lc_wnd_rect))
		{
			int lc_wnd_width=lc_wnd_rect.right-lc_wnd_rect.left;
			int lc_wnd_height=lc_wnd_rect.bottom-lc_wnd_rect.top;
			if(::ClientToScreen(this->GetHWND(),&lc_wnd_pt))
			{
				if(lc_wnd_pt.x<=lc_mouse_pt.x&&lc_mouse_pt.x<=lc_wnd_pt.x+lc_wnd_width
					&&lc_wnd_pt.y<=lc_mouse_pt.y&&lc_mouse_pt.y<=lc_wnd_pt.y+lc_wnd_height)
				{//鼠标在里面
					if(lc_mouse_pt.x!=this->mLstMouseGlbPos.x||lc_mouse_pt.y!=this->mLstMouseGlbPos.y)
					{
						this->mRecordShowTimeSpan.reset(0);
					}
				}
				else
				{//鼠标在外面
					if(this->mRecordShowTimeSpan.getCurrentMillisecond()<4*1000)
						this->mRecordShowTimeSpan.reset(4*1000);
				}
			}
		}
		this->mLstMouseGlbPos=lc_mouse_pt;
	}
	if(this->mRecordShowTimeSpan.getCurrentMillisecond()>=(int)(4.5*1000))
	{
		if(this->bMouseEnterWndShow)
		{
			this->bMouseEnterWndShow=FALSE;
			this->ShowWindow(false);
		}
	}
	else
	{
		if(!this->bMouseEnterWndShow)
		{
			this->bMouseEnterWndShow=TRUE;
			this->ShowWindow(true);
		}
	}
}

void CDuiPlayToolCtrlWnd::setPlayProgress(float _fPlayProgress)
{
	if(!this->bProgressSliderDown)
	{
		int iLcPlayProgress=(int)(_fPlayProgress*(this->p_sliderAvPgr->GetMaxValue()-this->p_sliderAvPgr->GetMinValue()+1));
		if(iLcPlayProgress!=this->p_sliderAvPgr->GetValue())
			this->p_sliderAvPgr->SetValue(iLcPlayProgress);
	}
}

void CDuiPlayToolCtrlWnd::Notify(DuiLib::TNotifyUI& _msg)
{
	this->mRecordShowTimeSpan.reset();
	if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0)
	{
		if(_msg.pSender==p_btnAvAttr)
		{
			this->p_cb_func->clickAvAttr();
		}
		else if(_msg.pSender==p_btnStop)
		{
			this->p_cb_func->clickPlayStop();
		}
		else if(_msg.pSender==p_btnPrevFrame)
		{
			if(this->p_cb_func->clickPrevFrame())
			{
				p_btnPlay->SetVisible(true);
				p_btnPause->SetVisible(false);
			}
		}
		else if(_msg.pSender==p_btnNextFrame)
		{
			if(this->p_cb_func->clickNextFrame())
			{
				p_btnPlay->SetVisible(true);
				p_btnPause->SetVisible(false);
			}
		}
		else if(_msg.pSender==p_btnPlay)
		{
			if(this->p_cb_func->clickPlayResume())
			{
				p_btnPlay->SetVisible(false);
				p_btnPause->SetVisible(true);
			}
		}
		else if(_msg.pSender==p_btnPause)
		{
			if(this->p_cb_func->clickPlayPause())
			{
				p_btnPlay->SetVisible(true);
				p_btnPause->SetVisible(false);
			}
		}
		else if(_msg.pSender==p_btnOpenUri)
		{
			this->p_cb_func->clickOpenUri();
		}
		else if(_msg.pSender==p_btnScreenFull)
		{
			this->p_cb_func->clickFullScreen(true);
			p_btnScreenFull->SetVisible(false);
			p_btnNoScreenFull->SetVisible(true);
		}
		else if(_msg.pSender==p_btnNoScreenFull)
		{
			this->p_cb_func->clickFullScreen(false);
			p_btnScreenFull->SetVisible(true);
			p_btnNoScreenFull->SetVisible(false);
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_WINDOWINIT)==0)
	{
		//this->ShowWindow(false);
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_VALUECHANGED)==0)
	{
		if(_msg.pSender==this->p_sliderAvPgr&&this->bBtnProgressSliderClick)
		{
			this->bBtnProgressSliderClick=FALSE;
			float lc_f=(float)this->p_sliderAvPgr->GetValue()/(float)(this->p_sliderAvPgr->GetMaxValue()-this->p_sliderAvPgr->GetMinValue());
			const float lc_lim=(float)0.999999999;
			if(lc_f>=lc_lim)
				lc_f=lc_lim;
			this->p_cb_func->clickSetProgress(lc_f);
		}
	}
	return __super::Notify(_msg);
}

void CDuiPlayToolCtrlWnd::setPlayPauseCtrBtnVisual(bool _bShowPlayBtn)
{
	this->p_btnPlay->SetVisible(_bShowPlayBtn);
	this->p_btnPause->SetVisible(!_bShowPlayBtn);
}

void CDuiPlayToolCtrlWnd::setEnableAnyControl(bool _bEnableCtrl)
{
	bool lc_bEnableCtrl=_bEnableCtrl;
	this->p_btnAvAttr->SetEnabled(lc_bEnableCtrl);
	this->p_btnStop->SetEnabled(lc_bEnableCtrl);
	this->p_btnPrevFrame->SetEnabled(lc_bEnableCtrl);
	this->p_btnPlay->SetEnabled(lc_bEnableCtrl);
	this->p_btnPause->SetEnabled(lc_bEnableCtrl);
	this->p_btnNextFrame->SetEnabled(lc_bEnableCtrl);
	this->p_sliderAvPgr->SetEnabled(lc_bEnableCtrl);
}

void CDuiPlayToolCtrlWnd::resetProgressSliderUi()
{
	this->p_sliderAvPgr->SetValue(0);
}
