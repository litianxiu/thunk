#pragma once

#include <UIlib.h>
#include <vector>
#include <memory>
#include <functional>
#include <Windows.h>
#include "TxGlobalConfig.h"
#include "TxAssertWrap.hpp"

class TxDuiListMenuExt : public DuiLib::WindowImplBase
{
public:
	struct tagArgs
	{
		LPCTSTR text;
		int index;
	};
private:
	class MyDuiListMenuElemExt: public DuiLib::CListContainerElementUI
	{
	private:
		DuiLib::CLabelUI *label;
		int iIndex;
	public :
		MyDuiListMenuElemExt(LPCTSTR _text,int _iIndex)
		{
			assert(_iIndex>=0);
			this->iIndex=_iIndex;
			this->label=new DuiLib::CLabelUI();
			this->Add(this->label);
			this->label->SetText(_text);
		}
		~MyDuiListMenuElemExt()
		{
		}
		inline int getMyIndex() const
		{
			return this->iIndex;
		}
		DuiLib::CLabelUI* getMyLabel()
		{
			return this->label;
		}
	};
private:
	DuiLib::CListUI *m_pListUi;
	TxCppPlatform::function<void(int)> m_callback;
	std::vector<MyDuiListMenuElemExt*> mRecordCtrlPtr;
	HWND m_HwndParent;
	BOOL bInitControlUI;
	BOOL bSizeSign;
private:

	virtual LPCTSTR GetWindowClassName() const
	{
		return _T("TxDuiListMenuExt");
	}

	virtual DuiLib::CDuiString GetSkinFolder()
	{
		return DefDuiSkin;
	}

	virtual DuiLib::CDuiString GetSkinFile()
	{
		return _T("list_menu.xml");
	}

	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		switch(_uMsg)
		{
		case WM_KILLFOCUS:
			this->ShowWindow(false,false);
			break;
		default:
			break;
		}
		return __super::HandleMessage(_uMsg,_wParam,_lParam);
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
		return __super::MessageHandler(_uMsg,_wParam,_lParam,_bHandled);
	}

	bool OnListMenuItemEvent(void* pParam)
	{
		DuiLib::TEventUI* pEvent = (DuiLib::TEventUI*)pParam;
		switch(pEvent->Type)
		{
		case DuiLib::UIEVENT_BUTTONUP:
			for(int i=(int)this->mRecordCtrlPtr.size();i--;)
			{
				CRect lc_rect(this->mRecordCtrlPtr[i]->GetPos());
				if(lc_rect.PtInRect(pEvent->ptMouse))
				{
					this->ShowWindow(false);
					if(this->m_callback)
					{
						TxCppPlatform::function<void(int)> m_lc_f(this->m_callback);
						this->m_callback=TxCppPlatform::function<void(int)>();
						m_lc_f(this->mRecordCtrlPtr[i]->getMyIndex());
					}
					break;
				}
			}
			break;
		default:
			break;
		}
		return true;
	}

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

	virtual void InitWindow()
	{
		DefAssertWrap(this->bInitControlUI==FALSE);
		this->buildSaftWidgetPtr(&this->m_pListUi,_T("nemu_list_name"));
		this->bInitControlUI=TRUE;
	}

	void _run_(const TxDuiListMenuExt::tagArgs *_elem,int _count,TxCppPlatform::function<void(int)>& _m_callback)
	{
		DefAssertWrap(this->bInitControlUI!=FALSE);
		this->mRecordCtrlPtr.clear();
		this->m_callback=_m_callback;
		this->m_pListUi->RemoveAll();
		CRect lc_g_rect(0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN));
		int iLcW=0,iLcH=0;
		for(int i=0;i<_count;i++)
		{
			if(_elem[i].text!=NULL&&_elem[i].index>=0)
			{
				MyDuiListMenuElemExt *pDuiListMenuElemExt=new MyDuiListMenuElemExt(_elem[i].text,_elem[i].index);

				CRect lcRealTextRect(lc_g_rect);
				DuiLib::CRenderEngine::DrawText(this->m_PaintManager.GetPaintDC(),&this->m_PaintManager,
					lcRealTextRect,_elem[i].text,0,-1,DT_CALCRECT|DT_CENTER|DT_EDITCONTROL|DT_WORDBREAK);
				lcRealTextRect.InflateRect(0,0,30,2);
				if(iLcW<lcRealTextRect.Width()) iLcW=lcRealTextRect.Width();
				iLcH+=lcRealTextRect.Height();
				pDuiListMenuElemExt->SetInset(CRect(28,1,1,1));
				pDuiListMenuElemExt->OnEvent+=MakeDelegate(this, &TxDuiListMenuExt::OnListMenuItemEvent);
				pDuiListMenuElemExt->getMyLabel()->OnEvent+=MakeDelegate(this, &TxDuiListMenuExt::OnListMenuItemEvent);
				this->m_pListUi->Add(pDuiListMenuElemExt);
				this->mRecordCtrlPtr.push_back(pDuiListMenuElemExt);
			}
		}
		//this->ResizeClient(iLcW+iLcH/(_count)+8,iLcH+8);
		CPoint ptPos(0,0);
		::GetCursorPos(&ptPos);
		//::SetWindowPos(*this,NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		::MoveWindow(this->GetHWND(),ptPos.x, ptPos.y,(iLcW+iLcH/(_count)+8),(iLcH+iLcH/4+8)+(this->bSizeSign?0:1),TRUE);
		this->bSizeSign=(!this->bSizeSign);
		this->ShowWindow(true,true);
	}

public:

	explicit TxDuiListMenuExt()
	{
		this->bSizeSign=FALSE;
		this->bInitControlUI=FALSE;
	}

	virtual ~TxDuiListMenuExt()
	{
	}

	void initialize(HWND _HwndParent)
	{
		if(!this->bInitControlUI)
		{
			this->Create(_HwndParent,_T("TxDuiListMenuExt"), UI_WNDSTYLE_CONTAINER, WS_EX_WINDOWEDGE);
		}
	}

	void run(TxCppPlatform::function<void(int)> _m_callback,
		LPCTSTR _text0,int _index0,
		LPCTSTR _text1=NULL,int _index1=-1,
		LPCTSTR _text2=NULL,int _index2=-1,
		LPCTSTR _text3=NULL,int _index3=-1,
		LPCTSTR _text4=NULL,int _index4=-1,
		LPCTSTR _text5=NULL,int _index5=-1,
		LPCTSTR _text6=NULL,int _index6=-1,
		LPCTSTR _text7=NULL,int _index7=-1,
		LPCTSTR _text8=NULL,int _index8=-1,
		LPCTSTR _text9=NULL,int _index9=-1)
	{
		tagArgs lc_elem[]= {
			{_text0,_index0},
			{_text1,_index1},
			{_text2,_index2},
			{_text3,_index3},
			{_text4,_index4},
			{_text5,_index5},
			{_text6,_index6},
			{_text7,_index7},
			{_text8,_index8},
			{_text9,_index9},
			{NULL,NULL}
		};
		int lc_count=0;
		while(lc_elem[lc_count].text!=NULL)
			lc_count++;
		this->_run_(lc_elem,lc_count,_m_callback);
	}
	void run(const TxDuiListMenuExt::tagArgs *_elem,int _count,TxCppPlatform::function<void(int)>& _m_callback)
	{
		return this->_run_(_elem,_count,_m_callback);
	}
	template<class TFUNC>
	void run(const TxDuiListMenuExt::tagArgs *_elem,int _count,TFUNC &_mFunc)
	{
		this->_run_(_elem,_count,_mFunc);
	}
};
