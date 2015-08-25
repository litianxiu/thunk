#pragma once
#include <atlimage.h>
#include <vector>
#include <UIlib.h>
#include "TxSystemDependent.hpp"

// MyAutoWaitDlgR_ ¶Ô»°¿ò

class MyAutoWaitDuiDlgR_ : public DuiLib::WindowImplBase
{
private:
	TxSystemDependent::TxEventWrap *const p_event;
	DuiLib::CControlUI *pTextWnd;
	DuiLib::CDuiString mStrText;
	int nHitPos;
	enum { e_hit_max_pos=6, };
	int iTimerId;
	BOOL bThreadRunSign;
	TxSystemDependent::TxMutexWrap mutex;
public:
	enum { _e_start_temp_=(WM_APP|WM_USER)+164,
		e_My_PostMessageUdateText,
		e_My_PostMessageExitWnd,
	};
public:
	MyAutoWaitDuiDlgR_(TxSystemDependent::TxEventWrap *_p_event,LPCTSTR _strText)
		:p_event(_p_event)
	{
		this->mStrText=_strText;
		this->iTimerId=0;
		this->nHitPos=0;
		this->bThreadRunSign=FALSE;
		this->pTextWnd=NULL;
	}
	void myRefreshText_()
	{
		DuiLib::CDuiString ss;
		for(int i=this->nHitPos;i--;)
			ss+=_T("¡£");
		this->mutex.lock();
		DuiLib::CDuiString mLcText=this->mStrText;
		this->mutex.unlock();
		this->pTextWnd->SetText(mLcText+ss);
	}
	void myAfterInitWindow()
	{
		CRect lcRealTextRect(0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN));
		CString sssss=this->mStrText;
		this->nHitPos=0;
		for (int iii=e_hit_max_pos;--iii;)
			sssss+=_T("¡£");
		DuiLib::CRenderEngine::DrawText(this->m_PaintManager.GetPaintDC(),&this->m_PaintManager,
			lcRealTextRect,sssss,0,-1,DT_CALCRECT|DT_CENTER|DT_EDITCONTROL|DT_WORDBREAK);
		lcRealTextRect.InflateRect(0,0,2,2);
		int i_lc_tag=lcRealTextRect.Height()/3;
		CSize lc_g_size(i_lc_tag*2+lcRealTextRect.Width(),i_lc_tag*2+lcRealTextRect.Height());
		this->ResizeClient(lc_g_size.cx,lc_g_size.cy);
		this->CenterWindow();
		this->pTextWnd->SetPos(CRect(CPoint(0,(int)(i_lc_tag)),CSize(lc_g_size.cx,lcRealTextRect.Height())));
		this->myRefreshText_();
		enum { e_lc_timer_id=1234, };
		this->iTimerId=::SetTimer(*this,e_lc_timer_id,420,NULL);
		if(this->iTimerId!=e_lc_timer_id)
		{
			this->iTimerId=0;
			DefAssertWrap(FALSE);
		}
		this->p_event->setEvent();
	}
	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		switch(_uMsg)
		{
		case WM_CREATE:
			{
				this->mutex.lock();
				this->bThreadRunSign=TRUE;
				this->mutex.unlock();
			}
			break;
		case WM_QUIT:
		case WM_CLOSE:
		case WM_DESTROY:
			if(this->iTimerId!=0)
			{
				::KillTimer(*this,this->iTimerId);
				this->iTimerId=0;
			}
			this->mutex.lock();
			this->bThreadRunSign=FALSE;
			this->mutex.unlock();
			break;
		case WM_TIMER:
			if((int)_wParam==(int)this->iTimerId)
			{
				this->myRefreshText_();
				this->nHitPos=(this->nHitPos+1)%e_hit_max_pos;
				return S_OK;
			}
			break;
		default:
			break;
		}
		return __super::HandleMessage(_uMsg, _wParam, _lParam);
	}
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled)
	{
		switch(_uMsg)
		{
		case WM_KEYDOWN:
			if(_wParam==VK_ESCAPE||_wParam==VK_CANCEL)
				_bHandled=true;
			return S_OK;
			break;
		case e_My_PostMessageUdateText:
			DefAssertTextWrap(_lParam==NULL,"");
			this->myRefreshText_();
			_bHandled=true;
			return S_OK;
			break;
		case e_My_PostMessageExitWnd:
			this->Close();
			_bHandled=true;
			return S_OK;
			break;
		default:
			break;
		}
		return __super::MessageHandler(_uMsg, _wParam, _lParam,_bHandled);
	}
	virtual void Notify(DuiLib::TNotifyUI& _msg)
	{
		if(_msg.sType.Compare(DUI_MSGTYPE_WINDOWINIT)==0)
		{
			this->pTextWnd=this->m_PaintManager.FindControl(_T("label_wait_text"));
			this->myAfterInitWindow();
			this->pTextWnd->SetEnabled(false);
		}
		else if(_msg.sType.Compare(DUI_MSGTYPE_TIMER)==0)
		{
		}
		return __super::Notify(_msg);
	}
	virtual DuiLib::CDuiString GetSkinFolder()
	{
		return DefDuiSkin;
	}
	virtual DuiLib::CDuiString GetSkinFile()
	{
		return _T("auto_wait_dlg.xml");
	}
	virtual LPCTSTR GetWindowClassName() const
	{
		return _T("MyAutoWaitDuiDlgR_");
	}
	virtual UINT GetClassStyle() const
	{
		return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
	}
	virtual DuiLib::UILIB_RESOURCETYPE GetResourceType() const
	{
		return DuiLib::UILIB_FILE;
	}
	void update_text(LPCTSTR _text)
	{
		this->mutex.lock();
		if(this->bThreadRunSign)
		{
			this->mStrText=_text;
			this->PostMessage(e_My_PostMessageUdateText,NULL,NULL);
		}
		this->mutex.unlock();
	}
	void exit_wnd()
	{
		this->mutex.lock();
		if(this->bThreadRunSign)
		{
			this->PostMessage(e_My_PostMessageExitWnd,NULL,NULL);
		}
		this->mutex.unlock();
	}
};

class TxAutoWaitDuiDialog
{
private:
	TxSystemDependent::TxThreadWrap mThread;
	TxCppPlatform::weak_ptr<MyAutoWaitDuiDlgR_> wpWaitDlg;
	CString csText;
	TxSystemDependent::TxEventWrap mEvent;
private:
	static void _static_call_back_(void *_arg1,void*)
	{
		((TxAutoWaitDuiDialog*)_arg1)->_call_back_();
	}
	void _call_back_()
	{
		TxCppPlatform::shared_ptr<MyAutoWaitDuiDlgR_> spLcDlg(new MyAutoWaitDuiDlgR_(&this->mEvent,csText));
		this->wpWaitDlg=spLcDlg;
		spLcDlg->Create(NULL,NULL,UI_WNDSTYLE_FRAME&(~WS_MAXIMIZEBOX)&(~WS_MINIMIZEBOX),0);
		spLcDlg->ShowModal();
	}
public :
	TxAutoWaitDuiDialog(LPCTSTR _strText)
	{
		this->csText=_strText;
		mThread.create(_static_call_back_,this,NULL);
		this->mEvent.waitEvent();
	}
	~TxAutoWaitDuiDialog()
	{
		this->quitDuiDialog();
	}
	void updateText(LPCTSTR _strText)
	{
		TxCppPlatform::shared_ptr<MyAutoWaitDuiDlgR_> spLcDlg(this->wpWaitDlg.lock());
		if(spLcDlg) spLcDlg->update_text(_strText);
	}
	void quitDuiDialog()
	{
		TxCppPlatform::shared_ptr<MyAutoWaitDuiDlgR_> spLcDlg(this->wpWaitDlg.lock());
		if(spLcDlg) spLcDlg->exit_wnd();
		this->mThread.join();
	}
};
