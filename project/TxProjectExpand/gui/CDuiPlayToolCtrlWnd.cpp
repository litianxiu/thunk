#include "stdafx.h"
#include "CDuiPlayToolCtrlWnd.h"


CDuiPlayToolCtrlWnd::CDuiPlayToolCtrlWnd(I_CALLBACK *_cbf):p_cb_func(_cbf)
{
	this->bInitControlUI=FALSE;
	this->bMouseEnterWndShow=FALSE;
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
		this->buildSaftWidgetPtr(&this->p_hrztlytGlobal,_T("hrztlytGlobal"));
		this->buildSaftWidgetPtr(&this->p_btnAvAttr,_T("btnAvAttr"));
		this->buildSaftWidgetPtr(&this->p_btnStop,_T("btnStop"));
		this->buildSaftWidgetPtr(&this->p_btnPrevFrame,_T("btnPrevFrame"));
		this->buildSaftWidgetPtr(&this->p_btnPlay,_T("btnPlay"));
		this->buildSaftWidgetPtr(&this->p_btnPause,_T("btnPause"));
		this->buildSaftWidgetPtr(&this->p_btnNextFrame,_T("btnNextFrame"));
		this->buildSaftWidgetPtr(&this->p_btnVolume,_T("btnVolume"));
		this->buildSaftWidgetPtr(&this->p_btnVolumeZero,_T("btnVolumeZero"));
		this->buildSaftWidgetPtr(&this->p_sliderVolume,_T("sliderVolume"));
		this->buildSaftWidgetPtr(&this->p_btnOpenUri,_T("btnOpenUri"));
		this->buildSaftWidgetPtr(&this->p_btnScreenFull,_T("btnScreenFull"));
		this->buildSaftWidgetPtr(&this->p_btnNoScreenFull,_T("btnNoScreenFull"));
		this->bInitControlUI=TRUE;
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
			::ShowWindow(this->GetHWND(),SW_HIDE);
		}
	}
	else
	{
		if(!this->bMouseEnterWndShow)
		{
			this->bMouseEnterWndShow=TRUE;
			::ShowWindow(this->GetHWND(),SW_SHOW);
		}
	}
}

void CDuiPlayToolCtrlWnd::Notify(DuiLib::TNotifyUI& _msg)
{
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
			this->p_cb_func->clickPrevFrame();
		}
		else if(_msg.pSender==p_btnPlay)
		{
			this->p_cb_func->clickPlayStart();
			p_btnPlay->SetVisible(false);
			p_btnPause->SetVisible(true);
		}
		else if(_msg.pSender==p_btnPause)
		{
			this->p_cb_func->clickPlayPause();
			p_btnPlay->SetVisible(true);
			p_btnPause->SetVisible(false);
		}
		else if(_msg.pSender==p_btnNextFrame)
		{
			this->p_cb_func->clickNextFrame();
		}
		else if(_msg.pSender==p_btnVolume)
		{
			this->p_cb_func->clickEnableVolume(true);
			p_sliderVolume->SetEnabled(true);
			p_btnVolume->SetVisible(false);
			p_btnVolumeZero->SetVisible(true);
		}
		else if(_msg.pSender==p_btnVolumeZero)
		{
			this->p_cb_func->clickEnableVolume(false);
			p_sliderVolume->SetEnabled(false);
			p_btnVolume->SetVisible(true);
			p_btnVolumeZero->SetVisible(false);
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
	else if(_msg.sType.Compare(DUI_MSGTYPE_VALUECHANGED)==0)
	{
		if(_msg.pSender==p_sliderVolume)
		{
			this->p_cb_func->clickSetVolume((float)p_sliderVolume->GetValue()/(float)100);
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_WINDOWINIT)==0)
	{
		::ShowWindow(this->GetHWND(),SW_HIDE);
	}
	return __super::Notify(_msg);
}


