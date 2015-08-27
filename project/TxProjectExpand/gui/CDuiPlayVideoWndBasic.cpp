#include "StdAfx.h"

#include "../common/TxSelectFileDialog.hpp"
#include "CDuiPlayVideoWndBasic.h"

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
	this->fPlayProgress=0;
	this->iVideoAttrDlgShowCount=0;
}

CDuiPlayVideoWndBasic::~CDuiPlayVideoWndBasic()
{
	this->_relase_resc_();
}

void CDuiPlayVideoWndBasic::_relase_resc_()
{
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

void CDuiPlayVideoWndBasic::_addPostTask_(TxCppPlatform::function<void(void)> _spFunc)
{
	this->mPostTaskOpCs.mTaskMutex.lock();
	this->mPostTaskOpCs.mListTask.push_back(_spFunc);
	this->mPostTaskOpCs.mTaskMutex.unlock();
	this->PostMessage(e_post_task_id);
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
	if(_w>0&&_h>0)
	{
		this->_moveWindow_ext_(this->mParentWnd,_x,_y,_w,_h);
	}
}

void CDuiPlayVideoWndBasic::myDoDropEvent(HDROP _hDrop)
{
	std::list<std::string> lc_list_string;
	{
		HDROP lc_hDrop=_hDrop;
		const int lc_count=(int)::DragQueryFile(lc_hDrop,-1,NULL,0);
		char __lc_path_tem[FILENAME_MAX+MAX_PATH+4];
		char *pLcPath=__lc_path_tem;
		int iLcPathMax=sizeof(__lc_path_tem)-1;
		for(int nn=0;nn<lc_count+1;nn++)
		{
			int iLcPathnameSize =::DragQueryFile(lc_hDrop,nn,NULL,0);
			if(iLcPathnameSize>0)
			{
				iLcPathnameSize++;
				if(iLcPathnameSize>iLcPathMax)
				{
					if(pLcPath!=__lc_path_tem)
						delete pLcPath;
					pLcPath=new char[iLcPathnameSize];
					iLcPathMax=iLcPathnameSize;
				}
				if((int)::DragQueryFile(lc_hDrop,nn,pLcPath,iLcPathMax)>0)
				{
					lc_list_string.push_back(pLcPath);
				}
			}
		}
		if (__lc_path_tem!=pLcPath)
			delete pLcPath;
		DragFinish(lc_hDrop);
	}
	if(lc_list_string.size()!=1)
	{
		//TxDuiMessageBox::run(this->GetHWND(),_T("错误"),_T("只能选择一个文件"),_T("确定"));
	}
	else
	{
		struct _stat64 lc_file_info={0};
		std::string lc_str_filename=lc_list_string.front();
		if(0==::_stat64(lc_str_filename.c_str(),&lc_file_info))
		{
			//文件大小
			if(lc_file_info.st_mode&S_IFDIR)
			{
				//TxDuiMessageBox::run(this->GetHWND(),_T("错误"),_T("只能选择文件\n不能选择文件夹\n\n"),_T("确定"));
			}
			else if(lc_file_info.st_size==0)
			{
				//TxDuiMessageBox::run(this->GetHWND(),_T("错误"),_T("这是一个空文件"),_T("确定"));
			}
			else if((lc_file_info.st_mode&S_IREAD)==0)
			{
				//TxDuiMessageBox::run(this->GetHWND(),_T("错误"),_T("此文件没有可读权限"),_T("确定"));
			}
			else 
			{
				this->p_IPlayVideoHandle->vfCtrlOpenUri(lc_str_filename.c_str());
			}
		}
	}
}

void CDuiPlayVideoWndBasic::postPaintFrame(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> &_spLcDdFrame,float _fPlayProgress)
{
	this->mFrameMutex.lock();
	this->spCurrentFrame=_spLcDdFrame;
	this->fPlayProgress=_fPlayProgress;
	this->mFrameMutex.unlock();
	this->PostMessage(e_post_paint_id);
}

void CDuiPlayVideoWndBasic::postStopVideoEvent()
{
	struct T_R
	{
		CDuiPlayVideoWndBasic *pThis;
		void operator()()
		{
			if(::IsWindow(this->pThis->m_DuiPlayToolCtrlWnd.GetHWND())
				&&::IsWindow(this->pThis->GetHWND()))
			{
				this->pThis->m_DuiPlayToolCtrlWnd.resetProgressSliderUi();
				::InvalidateRect(this->pThis->GetHWND(),NULL,TRUE);
			}
		}
	} m_T_R;
	m_T_R.pThis=this;
	this->_addPostTask_(m_T_R);
}

void CDuiPlayVideoWndBasic::postToolSelectPlayPauseCtrBtnVisual(bool _bShowPlayBtn)
{
	struct T_R
	{
		CDuiPlayVideoWndBasic *pThis;
		BOOL bPlayBtnVisual;
		void operator()()
		{
			this->pThis->m_DuiPlayToolCtrlWnd.setPlayPauseCtrBtnVisual(!!this->bPlayBtnVisual);
		}
	} m_T_R;
	m_T_R.bPlayBtnVisual=!!_bShowPlayBtn;
	m_T_R.pThis=this;
	this->_addPostTask_(m_T_R);
}

void CDuiPlayVideoWndBasic::postEnableToolBarCtrlWindow(bool _bEnable)
{
	struct T_R
	{
		CDuiPlayVideoWndBasic *pThis;
		BOOL bEnable;
		void operator()()
		{
			this->pThis->m_DuiPlayToolCtrlWnd.setEnableAnyControl(!!this->bEnable);
		}
	} m_T_R;
	m_T_R.bEnable=!!_bEnable;
	m_T_R.pThis=this;
	this->_addPostTask_(m_T_R);
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
	case WM_DROPFILES:
		this->myDoDropEvent((HDROP)_wParam);
		break;
	case WM_MOUSEMOVE:
	case WM_TIMER:
		if(this->iVideoAttrDlgShowCount==0&&::IsWindowVisible(this->GetHWND()))
			this->m_DuiPlayToolCtrlWnd.timerHitShow();
		break;
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		this->_relase_resc_();
		break;
	case e_post_task_id:
		{
			std::list<TxCppPlatform::function<void(void)>> mLcListTask;
			this->mPostTaskOpCs.mTaskMutex.lock();
			mLcListTask.swap(this->mPostTaskOpCs.mListTask);
			this->mPostTaskOpCs.mListTask.clear();
			this->mPostTaskOpCs.mTaskMutex.unlock();
			std::list<TxCppPlatform::function<void(void)>>::iterator iter=mLcListTask.begin();
			std::list<TxCppPlatform::function<void(void)>>::iterator iter_end=mLcListTask.end();
			for(;iter!=iter_end;iter++)
				(*iter)();
		}
		break;
	case e_post_paint_id:
		{
			this->mFrameMutex.lock();
			float fLcPlayPos=this->fPlayProgress;
			TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> lc_sp_frame=this->spCurrentFrame;
			this->mFrameMutex.unlock();
			this->m_DuiPlayToolCtrlWnd.setPlayProgress(fLcPlayPos);
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
			}
		}
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
		this->postToolSelectPlayPauseCtrBtnVisual(true);
		this->postEnableToolBarCtrlWindow(false);
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
	assert(this->iVideoAttrDlgShowCount==0);
	++this->iVideoAttrDlgShowCount;
	::ShowWindow(this->m_DuiPlayToolCtrlWnd.GetHWND(),SW_HIDE);
	this->p_IPlayVideoHandle->vfCtrlAvAttr();
	--this->iVideoAttrDlgShowCount;
}

void CDuiPlayVideoWndBasic::clickSetProgress(float _r)
{
	return this->p_IPlayVideoHandle->vfCtrlSetProgress(_r);
}


void CDuiPlayVideoWndBasic::clickOpenUri()
{
	CString lc_file=TxSelectFileDialog::getOpenFileName(this->m_DuiPlayToolCtrlWnd.GetHWND(),_T("All *.*"));
	if(lc_file.GetLength()>0)
	{
		this->p_IPlayVideoHandle->vfCtrlOpenUri((LPCTSTR)lc_file);
	}
}
