#pragma once

#include <Windows.h>
#include <shellapi.h>
#include <atltypes.h>
#include <functional>
#include <vector>
#include <memory>
#include "../common/TxSystemDependent.hpp"


class MyDuiMessageNotifyDlg : public DuiLib::WindowImplBase
{
public :
	struct tagStartInfo
	{
		DuiLib::CDuiString text;
		int h_id;
		RECT wndRect;
		TxCppPlatform::function<void()> mCallBack;
	};
private:
	TxSystemDependent::TxTimeSpan<false> mRecordTimeSpan;
	DuiLib::CRichEditUI* p_RichEdit_message;
	DuiLib::CButtonUI* p_btn_closewnd;
	BOOL bInitControlUI;
	int h_id;
	UINT_PTR uTimerIDEvent;
	RECT mOrgRect;
	HCURSOR mOldCURSOR;
	TxCppPlatform::function<void()> mCallBack;
	enum {
		e_timer_id=1158,
		e_useid_temp_null=(WM_APP|WM_USER)+64,
		e_useid_start_notify,
	} ;
	TxSystemDependent::TxMutexWrap m_mutex;
private:
	template<class T>
	void buildSaftWidgetPtr(T **_obj,LPCTSTR _str)
	{
		DuiLib::CControlUI* p_ui=m_PaintManager.FindControl(_str);
		*_obj=NULL;
		if(p_ui!=NULL)
			*_obj=dynamic_cast<T*>(p_ui);
		if((*_obj)==NULL)
			DefAssertWrap(FALSE);
	}
public:
	MyDuiMessageNotifyDlg()
	{
		this->bInitControlUI=FALSE;
		this->h_id=-1;
		this->uTimerIDEvent=0;
		this->mOldCURSOR=::SetCursor(::LoadCursor(NULL,IDC_ARROW));
	}
	~MyDuiMessageNotifyDlg()
	{
	}
	virtual DuiLib::CDuiString GetSkinFolder()
	{
		return DefDuiSkin;
	}
	virtual DuiLib::CDuiString GetSkinFile()
	{
		return _T("message_notify.xml");
	}
	virtual UINT GetClassStyle() const
	{
		return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
	}
	virtual DuiLib::UILIB_RESOURCETYPE GetResourceType() const
	{
		return DuiLib::UILIB_FILE;
	}
	virtual LPCTSTR GetWindowClassName( void ) const
	{
		return _T("MyDuiMessageNotifyDlg");
	}
	bool OnTheRichEditNotify(void* _param )
	{
		TxSystemDependent::TxAutoLocker _temLock(this->m_mutex); (void)_temLock;
		DuiLib::TEventUI* pMsg = (DuiLib::TEventUI*)_param;
		switch(pMsg->Type)
		{
		case DuiLib::UIEVENT_BUTTONDOWN:
			if(this->p_RichEdit_message==pMsg->pSender&&this->mCallBack)
				this->mCallBack();
			break;
		case DuiLib::UIEVENT_MOUSEENTER:
			::SetCapture(this->m_hWnd);
			if(this->mCallBack)
				this->mOldCURSOR=::SetCursor(::LoadCursor(NULL,IDC_HAND));
			break;
		case DuiLib::UIEVENT_MOUSELEAVE:
			ReleaseCapture();
			this->mOldCURSOR=::SetCursor(this->mOldCURSOR);
			break;
		default:
			break;
		}
		return true;
	}
	virtual void InitWindow()
	{
		TxSystemDependent::TxAutoLocker _temLock(this->m_mutex); (void)_temLock;
		assert(this->bInitControlUI==FALSE);
		this->buildSaftWidgetPtr(&this->p_RichEdit_message,_T("RichEdit_message"));
		this->buildSaftWidgetPtr(&this->p_btn_closewnd,_T("btn_closewnd"));
		this->bInitControlUI=TRUE;
		::SetWindowPos(this->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		this->p_RichEdit_message->SetEnabled(false);
		this->p_RichEdit_message->OnEvent += MakeDelegate(this,&MyDuiMessageNotifyDlg::OnTheRichEditNotify);
		this->p_RichEdit_message->OnNotify += MakeDelegate(this,&MyDuiMessageNotifyDlg::OnTheRichEditNotify);
		this->p_btn_closewnd->OnEvent += MakeDelegate(this,&MyDuiMessageNotifyDlg::OnTheRichEditNotify);
		
	}
	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		TxSystemDependent::TxAutoLocker _temLock(this->m_mutex); (void)_temLock;
		switch(_uMsg)
		{
		case e_useid_start_notify:
			{
				tagStartInfo tem_StartInfo=*(tagStartInfo*)_wParam;
				this->p_RichEdit_message->SetText(tem_StartInfo.text);
				this->mCallBack=tem_StartInfo.mCallBack;
				this->mOrgRect=tem_StartInfo.wndRect;
				this->h_id=tem_StartInfo.h_id;
				::MoveWindow(*this,this->mOrgRect.left,this->mOrgRect.top,this->mOrgRect.right-this->mOrgRect.left,this->mOrgRect.bottom-this->mOrgRect.top,TRUE);
				this->ShowWindow(true);

				if(this->uTimerIDEvent==0)
				{
					this->uTimerIDEvent=::SetTimer(this->m_hWnd,e_timer_id,16,NULL);
				}
				mRecordTimeSpan.reset(0);
			}
			break;
		case WM_TIMER:
			{
				enum { e_t1=8*1000, e_t2=5*1000, };
				long long ll_t=this->mRecordTimeSpan.getCurrentMillisecond();
				if(this->h_id<0)
				{
					if(this->uTimerIDEvent!=0)
					{
						::KillTimer(this->m_hWnd,this->uTimerIDEvent);
						this->uTimerIDEvent=0;
					}
					this->mCallBack=TxCppPlatform::function<void()>();
					this->ShowWindow(false);
				}
				else if(ll_t<=e_t1)
				{
				}
				else if(ll_t<e_t1+e_t2)
				{
					int lc_del_y=(int)((this->mOrgRect.bottom-this->mOrgRect.top)*(ll_t-e_t1)/e_t2);
					RECT mLcNewRect(this->mOrgRect);
					mLcNewRect.top+=lc_del_y;
					::MoveWindow(*this,mLcNewRect.left,mLcNewRect.top,mLcNewRect.right-mLcNewRect.left,mLcNewRect.bottom-mLcNewRect.top,TRUE);
				}
				else
				{
					this->h_id=-1;
				}
			}
			break;
		default:
			break;
		}
		return __super::HandleMessage(_uMsg,_wParam,_lParam);
	}
	void startNotify(const tagStartInfo &_m_StartInfo)
	{
		::SendMessage(*this,e_useid_start_notify,(WPARAM)&_m_StartInfo,0);
	}
	virtual void Notify(DuiLib::TNotifyUI& _msg)
	{
		TxSystemDependent::TxAutoLocker _temLock(this->m_mutex); (void)_temLock;
		if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0)
		{
			if(_msg.pSender==this->p_btn_closewnd)
			{
				this->h_id=-1;
			}
		}
		else if(_msg.sType.Compare(DUI_MSGTYPE_TIMER)==0)
		{
			int jkldsf=0;
		}
		return __super::Notify(_msg);
	}
	int getHeightIndex()
	{
		TxSystemDependent::TxAutoLocker _temLock(this->m_mutex); (void)_temLock;
		return this->h_id;
	}
};

class TxDuiMessageNotifyDlg
{
private:
	std::vector<TxCppPlatform::shared_ptr<MyDuiMessageNotifyDlg>> spListWndElem;
	int index;
	const int iMaxCount; 
	SIZE mUnitSize;
	const BOOL bFullScreen;
	DuiLib::CWindowWnd *const parentWnd;
	TxSystemDependent::TxMutexWrap m_mutex;
private:
	void _notify__(LPCTSTR _text,TxCppPlatform::function<void()> _mCallBack)
	{
		MyDuiMessageNotifyDlg::tagStartInfo lc_arg;

		lc_arg.h_id=-1;
		lc_arg.text=_text;
		lc_arg.mCallBack=_mCallBack;

		int fsx=0,fsy=0;
		if(bFullScreen)
		{
			fsx=::GetSystemMetrics(SM_CXSCREEN);
			fsy=::GetSystemMetrics(SM_CYSCREEN);
		}
		else
		{
			fsx=::GetSystemMetrics(SM_CXFULLSCREEN);
			fsy=::GetSystemMetrics(SM_CYFULLSCREEN);
		}
		this->m_mutex.lock();
		this->initialize();
		for(int i=0;;i++)
		{
			if(i>=(int)this->spListWndElem.size())
			{
				this->index=(int)((this->index+1)%this->spListWndElem.size());
				break;
			}
			else if(this->spListWndElem[i]->getHeightIndex()<0)
			{
				this->index=i;
				break;
			}
		}
		for(int y=0;lc_arg.h_id<0;y++)
		{
			for(int i=0;lc_arg.h_id<0;i++)
			{
				if(i>=(int)spListWndElem.size())
				{
					lc_arg.h_id=y;
					break;
				}
				else if(i!=index&&this->spListWndElem[i]->getHeightIndex()==y)
				{
					break;
				}
			}
		}
		lc_arg.wndRect.left=fsx-this->mUnitSize.cx;
		lc_arg.wndRect.right=fsx;
		lc_arg.wndRect.bottom=fsy-lc_arg.h_id*(this->mUnitSize.cy+2);
		lc_arg.wndRect.top=lc_arg.wndRect.bottom-this->mUnitSize.cy;

		this->spListWndElem[this->index]->startNotify(lc_arg);
		this->m_mutex.unlock();
	}
	void _open_dir_event(DuiLib::CDuiString _dir)
	{
		TxSystemDependent::TxRuntimeWrap::openExplorerSelectFile(_dir);
	}
public :
	TxDuiMessageNotifyDlg(DuiLib::CWindowWnd *_parentWnd,int _count,bool _bFullScreen)
		:bFullScreen(!!_bFullScreen),iMaxCount(_count),parentWnd((_parentWnd))
	{
		this->index=0;
		::memset(&this->mUnitSize,0,sizeof(this->mUnitSize));
	}
	void initialize()
	{
		if(this->iMaxCount!=(int)this->spListWndElem.size())
		{
			this->spListWndElem.resize(this->iMaxCount);
			for(int i=0;i<(int)spListWndElem.size();i++)
			{
				this->spListWndElem[i].reset(new MyDuiMessageNotifyDlg());
				assert(parentWnd!=NULL);
				this->spListWndElem[i]->Create(NULL,NULL,UI_WNDSTYLE_FRAME,WS_EX_TOPMOST);
				//this->spListWndElem[i]->Create(parentWnd->GetHWND(),NULL,UI_WNDSTYLE_FRAME,WS_EX_TOPMOST);
				RECT lc_rect={0};
				if(::GetWindowRect(*this->spListWndElem[i].get(),&lc_rect))
				{
					mUnitSize.cx=lc_rect.right-lc_rect.left;
					mUnitSize.cy=lc_rect.bottom-lc_rect.top;
				}
				this->spListWndElem[i]->ShowWindow(false);
			}
		}
	}
	void notify(LPCTSTR _text)
	{
		this->_notify__(_text,TxCppPlatform::function<void()>());
	}
	template<class TFUNC>
	void notify(LPCTSTR _text,TFUNC &_mfunc)
	{
		this->_notify__(_text,_mfunc);
	}
	void notifyFilePath(LPCTSTR _text,LPCTSTR _dir)
	{
		this->_notify__(_text,TxCppPlatform::bind(&TxDuiMessageNotifyDlg::_open_dir_event,this,DuiLib::CDuiString(_dir)));
	}
};

