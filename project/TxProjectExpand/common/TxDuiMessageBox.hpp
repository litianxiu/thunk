#pragma once

#include <atltypes.h>
#include "../common/TxAssertWrap.hpp"

class TxDuiMessageBox : public DuiLib::WindowImplBase
{
private:
	DuiLib::CDuiString mStrText;
	DuiLib::CDuiString mStrTitle;
	DuiLib::CDuiString mStrBtn0;
	DuiLib::CDuiString mStrBtn1;
	DuiLib::CDuiString mStrBtn2;
	int iResult;
private:
	DuiLib::CButtonUI *p_btn0;
	DuiLib::CButtonUI *p_btn1;
	DuiLib::CButtonUI *p_btn2;
	DuiLib::CLabelUI *p_label_title;
	DuiLib::CRichEditUI *p_RichEdit;
	DuiLib::CHorizontalLayoutUI *p_rich_edit_hrzlyt;
	DuiLib::CVerticalLayoutUI *p_btn01_vlyt;
	DuiLib::CVerticalLayoutUI *p_btn12_vlyt;
	BOOL bInitControlUI;
private:
	TxDuiMessageBox()
	{
		this->bInitControlUI=FALSE;
	}
	virtual void InitWindow()
	{
		assert(this->bInitControlUI==FALSE);

		this->buildSaftWidgetPtr(&this->p_btn0,_T("btn0"));
		this->buildSaftWidgetPtr(&this->p_btn1,_T("btn1"));
		this->buildSaftWidgetPtr(&this->p_btn2,_T("btn2"));
		this->buildSaftWidgetPtr(&this->p_btn01_vlyt,_T("btn01_vlyt"));
		this->buildSaftWidgetPtr(&this->p_btn12_vlyt,_T("btn12_vlyt"));
		this->buildSaftWidgetPtr(&this->p_label_title,_T("label_title"));
		this->buildSaftWidgetPtr(&this->p_RichEdit,_T("label_message"));
		this->buildSaftWidgetPtr(&this->p_rich_edit_hrzlyt,_T("rich_edit_hrzlyt"));

		this->bInitControlUI=TRUE;
	}
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled)
	{
		switch(_uMsg)
		{
		case WM_KEYDOWN:
			if(_wParam==VK_ESCAPE||_wParam==VK_CANCEL||_wParam==VK_RETURN)
			{
				_bHandled=true;
				return S_OK;
			}
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
			assert(this->bInitControlUI);
			CRect lcRealTextRect(0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN));
			DuiLib::CRenderEngine::DrawText(this->m_PaintManager.GetPaintDC(),&this->m_PaintManager,
				lcRealTextRect,this->mStrText,0,-1,DT_CALCRECT|DT_CENTER|DT_EDITCONTROL|DT_WORDBREAK);
			lcRealTextRect.InflateRect(0,0,2,2);
			int i_lc_tag=this->m_PaintManager.GetDefaultFontInfo()->iSize+16;
			CSize lc_g_size(i_lc_tag*2+lcRealTextRect.Width(),i_lc_tag*2+lcRealTextRect.Height()+100);
			this->ResizeClient(lc_g_size.cx,lc_g_size.cy);
			this->CenterWindow();
			this->p_rich_edit_hrzlyt->SetFixedHeight(lcRealTextRect.Height());

			this->p_btn0->SetText(this->mStrBtn0);
			if(this->mStrBtn1.GetLength()>0)
			{
				this->p_btn1->SetText(this->mStrBtn1);
			}
			else
			{
				this->p_btn01_vlyt->SetVisible(false);
				this->p_btn1->SetVisible(false);
			}
			if(this->mStrBtn2.GetLength()>0)
			{
				this->p_btn2->SetText(this->mStrBtn2);
			}
			else
			{
				this->p_btn12_vlyt->SetVisible(false);
				this->p_btn2->SetVisible(false);
			}

			this->p_RichEdit->SetText(this->mStrTitle);
			this->p_RichEdit->SetFloat(false);
			this->p_RichEdit->SetText(this->mStrText);
			this->p_RichEdit->SetEnabled(false);
			this->p_label_title->SetText(this->mStrTitle);
		}
		else if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0)
		{
			if(_msg.pSender==this->p_btn0)
			{
				this->iResult=0;
				this->Close();
			}
			else if(_msg.pSender==this->p_btn1)
			{
				this->iResult=1;
				this->Close();
			}
			else if(_msg.pSender==this->p_btn2)
			{
				this->iResult=2;
				this->Close();
			}
		}
		return __super::Notify(_msg);
	}
	virtual DuiLib::CDuiString GetSkinFolder()
	{
		return DefDuiSkin;
	}
	virtual DuiLib::CDuiString GetSkinFile()
	{
		return _T("message_dui_box.xml");
	}
	virtual LPCTSTR GetWindowClassName() const
	{
		return _T("TxDuiMessageBox");
	}
	virtual UINT GetClassStyle() const
	{
		return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
	}
	virtual DuiLib::UILIB_RESOURCETYPE GetResourceType() const
	{
		return DuiLib::UILIB_FILE;
	}
	void setText(LPCTSTR _text);
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
			DefAssertWrap(FALSE);
	}
public :
	static int run(HWND _parent,LPCTSTR _title,LPCTSTR _text,LPCTSTR _btn0,LPCTSTR _btn1=NULL,LPCTSTR _btn2=NULL)
	{
		TxDuiMessageBox mLcWnd;
		mLcWnd.iResult=-1;
		mLcWnd.mStrText=_text;
		mLcWnd.mStrTitle=_title;
		if(_btn0!=NULL) mLcWnd.mStrBtn0=_btn0;
		if(_btn1!=NULL) mLcWnd.mStrBtn1=_btn1;
		if(_btn2!=NULL) mLcWnd.mStrBtn2=_btn2;
		mLcWnd.Create(_parent,NULL,UI_WNDSTYLE_FRAME&(~WS_MAXIMIZEBOX)&(~WS_MINIMIZEBOX),0); // ´´½¨DLG´°¿Ú
		mLcWnd.ShowModal();
		return mLcWnd.iResult;
	}
};
