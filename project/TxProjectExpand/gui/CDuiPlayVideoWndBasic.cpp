#include "StdAfx.h"

#include "../common/TxSelectFileDialog.hpp"
#include "CDuiPlayVideoWndBasic.h"

void CDuiPlayVideoWndBasic::_relase_resc_()
{
	this->stop();
	if(this->pGuiDirectDrawDisplay!=NULL)
	{
		delete this->pGuiDirectDrawDisplay;
		this->pGuiDirectDrawDisplay=NULL;
	}
	if(this->uiTimerId_1000!=0)
	{
		::KillTimer(this->GetHWND(),this->uiTimerId_1000);
		this->uiTimerId_1000=0;
	}
}

void CDuiPlayVideoWndBasic::_static_call_back_(void *_arg1,void *_arg2)
{
	(void)_arg1;
	(void)_arg2;
	((CDuiPlayVideoWndBasic*)_arg1)->_call_back_();
}

void CDuiPlayVideoWndBasic::_call_back_()
{
	std::list<std::pair<long long,tagUnitInfo>> mLcListSpFrame;
	while(this->bThreadRunning)
	{
		this->mEvent.waitEvent(10);
		BOOL bLcPostPaint=FALSE;
		this->mMutex.lock();
		int iLcListSize=(int)this->mapThreadFrame.size();
		std::map<long long,tagUnitInfo>::iterator lcMapThreadFrameIter=this->mapThreadFrame.begin();
		if(iLcListSize>0&&lcMapThreadFrameIter->first<=this->mTimeSpan.getCurrentMillisecond())
		{
			this->spCurrentFrame=lcMapThreadFrameIter->second.spDdFrame;
			this->mapThreadFrame.erase(lcMapThreadFrameIter);
			bLcPostPaint=TRUE;
		}
		this->mMutex.unlock();
		if(bLcPostPaint)
		{
			this->PostMessage(e_post_paint_id);
		}
		if(iLcListSize<e_img_count)
			this->vfThreadReadFrame(&mLcListSpFrame,e_img_count-iLcListSize);
		if(mLcListSpFrame.size()>0)
		{
			this->mMutex.lock();
			this->mapThreadFrame.insert(mLcListSpFrame.begin(),mLcListSpFrame.end());
			this->mMutex.unlock();
			mLcListSpFrame.clear();
		}
	}
	this->mMutex.lock();
	this->mapThreadFrame.clear();
	this->mMutex.unlock();
}

CDuiPlayVideoWndBasic::CDuiPlayVideoWndBasic(CDuiPlayVideoWndBasic::IPlayVideoHandle *_p_IPlayVideoHandle)
	:m_DuiPlayToolCtrlWnd(this),p_IPlayVideoHandle(_p_IPlayVideoHandle)
{
	this->mWndScreenRect.left=0;
	this->mWndScreenRect.right=0;
	this->mWndScreenRect.top=0;
	this->mWndScreenRect.bottom=0;
	this->pGuiDirectDrawDisplay=NULL;
	this->mParentWnd=NULL;
	this->uiTimerId_1000=0;
}

CDuiPlayVideoWndBasic::~CDuiPlayVideoWndBasic()
{
	this->_relase_resc_();
}

LPCTSTR CDuiPlayVideoWndBasic::GetWindowClassName() const
{
	return _T("CDuiPlayVideoWndBasic");
}

DuiLib::CDuiString CDuiPlayVideoWndBasic::GetSkinFolder()
{
	return DefDuiSkin;
}

DuiLib::CDuiString CDuiPlayVideoWndBasic::GetSkinFile()
{
	return _T("dui_play_video_wnd.xml");
}

bool CDuiPlayVideoWndBasic::dtctThreadRunningStatus() const
{
	return !!this->bThreadRunning;
}

void CDuiPlayVideoWndBasic::start()
{
	this->stop();
	this->bThreadRunning=TRUE;
	this->mThread.create(_static_call_back_,this,NULL);
}

void CDuiPlayVideoWndBasic::stop()
{
	this->bThreadRunning=FALSE;
	this->mEvent.setEvent();
	this->mThread.join();
}

bool CDuiPlayVideoWndBasic::create(HWND _parentWnd)
{
	assert(this->pGuiDirectDrawDisplay==NULL&&this->mParentWnd==NULL);
	HWND hLcWnd=NULL;
	if(this->pGuiDirectDrawDisplay==NULL&&this->mParentWnd==NULL)
	{
		this->mParentWnd=_parentWnd;
		hLcWnd=__super::Create(_parentWnd,NULL,UI_WNDSTYLE_FRAME,WS_EX_ACCEPTFILES);
		(void)hLcWnd;
		assert(hLcWnd!=NULL);
		if(hLcWnd!=NULL)
		{
			this->pGuiDirectDrawDisplay=new CGuiDirectDrawDisplay(this->GetHWND());
			this->m_DuiPlayToolCtrlWnd.Create(this->GetHWND(),NULL,UI_WNDSTYLE_DIALOG,0);
		}
	}
	return hLcWnd!=NULL;
}

void CDuiPlayVideoWndBasic::_moveWindow_ext_(HWND _hParentWnd,int _x,int _y,int _w,int _h)
{
	POINT lc_pt={0};
	lc_pt.x=_x;
	lc_pt.y=_y;
	if(_hParentWnd==NULL||::ClientToScreen(_hParentWnd,&lc_pt))
	{
		this->mWndScreenRect.left=lc_pt.x;
		this->mWndScreenRect.top=lc_pt.y;
		this->mWndScreenRect.right=this->mWndScreenRect.left+_w;
		this->mWndScreenRect.bottom=this->mWndScreenRect.top+_h;

		::MoveWindow(this->GetHWND(),this->mWndScreenRect.left,
			this->mWndScreenRect.top,this->mWndScreenRect.right-this->mWndScreenRect.left,
			this->mWndScreenRect.bottom-this->mWndScreenRect.top,TRUE);

		RECT mLcRect={0};
		if(::GetWindowRect(this->m_DuiPlayToolCtrlWnd.GetHWND(),&mLcRect)&&mLcRect.left<mLcRect.right&&mLcRect.top<mLcRect.bottom)
		{
			int lc_yyy=(int)(mLcRect.bottom-mLcRect.top);
			::MoveWindow(this->m_DuiPlayToolCtrlWnd.GetHWND(),this->mWndScreenRect.left,this->mWndScreenRect.bottom-lc_yyy,_w,lc_yyy,TRUE);
		}
	}
}

void CDuiPlayVideoWndBasic::moveWindow(int _x,int _y,int _w,int _h)
{
	return this->_moveWindow_ext_(this->mParentWnd,_x,_y,_w,_h);
}

//TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> CDuiPlayVideoWndBasic::newDataCell()
//{
//	TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> ret;
//	this->mListFreeFrameMutex.lock();
//	if(this->mListFreeFrame.size()>0)
//	{
//		ret=this->mListFreeFrame.front();
//		this->mListFreeFrame.pop_front();
//	}
//	this->mListFreeFrameMutex.unlock();
//	if(!ret) ret.reset(new CDirectDrawFrameFormat());
//	return ret;
//}

void CDuiPlayVideoWndBasic::refresh_frame_gui()
{
	this->mMutex.lock();
	TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> lc_sp_frame=this->spCurrentFrame;
	this->spCurrentFrame.reset();
	this->mMutex.unlock();
	if(lc_sp_frame)
	{
		if(this->pGuiDirectDrawDisplay!=NULL
			&&this->mWndScreenRect.left>=0
			&&this->mWndScreenRect.top>=0
			&&this->mWndScreenRect.left<this->mWndScreenRect.right
			&&this->mWndScreenRect.top<this->mWndScreenRect.bottom)
		{
			this->pGuiDirectDrawDisplay->draw(lc_sp_frame.get(),this->mWndScreenRect.left,this->mWndScreenRect.top,
				this->mWndScreenRect.right-this->mWndScreenRect.left,
				this->mWndScreenRect.bottom-this->mWndScreenRect.top);
		}
		else
		{
			//assert(0);
		}
		this->mListFreeFrameMutex.lock();
		if(this->mListFreeFrame.size()<e_img_count)
			this->mListFreeFrame.push_back(lc_sp_frame);
		this->mListFreeFrameMutex.unlock();
	}
}

void CDuiPlayVideoWndBasic::showFullScreen(bool _bFull)
{
	if(_bFull) ::ShowWindow(this->GetHWND(),SW_SHOWMAXIMIZED);
	else ::ShowWindow(this->GetHWND(),SW_NORMAL);
	RECT lcWndRect={0};
	::GetWindowRect(this->GetHWND(),&lcWndRect);
	this->_moveWindow_ext_(NULL,lcWndRect.left,lcWndRect.top,lcWndRect.right-lcWndRect.left,lcWndRect.bottom-lcWndRect.top);
}

LRESULT CDuiPlayVideoWndBasic::MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled)
{
	switch(_uMsg)
	{
	case WM_KEYDOWN:
		if(_wParam==VK_ESCAPE||_wParam==VK_CANCEL||_wParam==VK_RETURN)
		{
			_bHandled=true;
			this->showFullScreen(false);
			return S_OK;
		}
		break;
	default:
		break;
	}
	return __super::MessageHandler(_uMsg, _wParam, _lParam,_bHandled);
}

LRESULT CDuiPlayVideoWndBasic::HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
{
	(void)_uMsg;
	(void)_wParam;
	(void)_lParam;
	switch(_uMsg)
	{
	case WM_MOUSEMOVE:
	case WM_TIMER:
		this->m_DuiPlayToolCtrlWnd.timerHitShow();
		break;
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		this->_relase_resc_();
		break;
	case e_post_paint_id:
		this->refresh_frame_gui();
		break;
	default:
		break;
	}
	return __super::HandleMessage(_uMsg,_wParam,_lParam);
}

void CDuiPlayVideoWndBasic::Notify(DuiLib::TNotifyUI& _msg)
{
	if(_msg.sType.Compare(DUI_MSGTYPE_WINDOWINIT)==0)
	{
		if(this->uiTimerId_1000==0)
		{
			this->uiTimerId_1000=::SetTimer(this->GetHWND(),1123,1000,NULL);
		}
	}
}

void CDuiPlayVideoWndBasic::showWindowVisible(bool _bShow)
{
	if(::IsWindow(this->GetHWND())&&::IsWindow(this->m_DuiPlayToolCtrlWnd.GetHWND()))
	{
		this->ShowWindow(_bShow);
		this->m_DuiPlayToolCtrlWnd.ShowWindow(_bShow);
	}
}

void CDuiPlayVideoWndBasic::OnFinalMessage(HWND hWnd)
{
	this->_relase_resc_();
	return __super::OnFinalMessage(hWnd);
}

void CDuiPlayVideoWndBasic::clickFullScreen(bool _bFull)
{
	this->showFullScreen(_bFull);
}

void CDuiPlayVideoWndBasic::clickPrevFrame()
{
	return this->p_IPlayVideoHandle->vfCtrlPrevFrame();
}

void CDuiPlayVideoWndBasic::clickNextFrame()
{
	return this->p_IPlayVideoHandle->vfCtrlNextFrame();
}

void CDuiPlayVideoWndBasic::clickPlayStart()
{
	return this->p_IPlayVideoHandle->vfCtrlPlayStart();
}

void CDuiPlayVideoWndBasic::clickPlayPause()
{
	return this->p_IPlayVideoHandle->vfCtrlPlayPause();
}

void CDuiPlayVideoWndBasic::clickPlayStop()
{
	return this->p_IPlayVideoHandle->vfCtrlPlayStop();
}

void CDuiPlayVideoWndBasic::clickAvAttr()
{
	return this->p_IPlayVideoHandle->vfCtrlAvAttr();
}

void CDuiPlayVideoWndBasic::clickSetVolume(float _r)
{
	return this->p_IPlayVideoHandle->vfCtrlSetVolume(_r);
}

void CDuiPlayVideoWndBasic::clickEnableVolume(bool _b)
{
	return this->p_IPlayVideoHandle->vfCtrlEnableVolume(_b);
}

void CDuiPlayVideoWndBasic::clickOpenUri()
{
	CString lc_file=TxSelectFileDialog::getOpenFileName(this->m_DuiPlayToolCtrlWnd.GetHWND(),_T("All *.*"));
	if(lc_file.GetLength()>0)
	{
		this->p_IPlayVideoHandle->vfCtrlOpenUri((LPCTSTR)lc_file);
	}
}
