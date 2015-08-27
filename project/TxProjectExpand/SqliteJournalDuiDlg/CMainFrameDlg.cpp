#include "stdafx.h"
#include "CMainFrameDlg.h"


CMainFrameDlg::CMainFrameDlg(const tagConfigSettingData &_mConfigSettingData)
:m_LogSettingWnd(this),m_SqliteJournalDuiDlg(this),m_BrowseRemoteDuiDlg(this),m_PlayVideoDuiDlg(this)
{
	this->bInitControlUI=FALSE;
	this->getConfigSettingDataPtr()[0]=_mConfigSettingData;
}

CMainFrameDlg::~CMainFrameDlg()
{
}

DuiLib::CDuiString CMainFrameDlg::GetSkinFolder()
{
	return DefDuiSkin;
}

DuiLib::CDuiString CMainFrameDlg::GetSkinFile()
{
	return _T("sqlite_main_frame.xml");
}

LPCTSTR CMainFrameDlg::GetWindowClassName() const
{
	return _T("CMainFrameDlg");
}

UINT CMainFrameDlg::GetClassStyle() const
{
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
}

DuiLib::UILIB_RESOURCETYPE CMainFrameDlg::GetResourceType() const
{
	return DuiLib::UILIB_FILE;
}

bool CMainFrameDlg::OnLayoutWndSizeEvent(void* _pParam)
{
	assert((DuiLib::CControlUI*)_pParam==this->pMainLayout);
	CRect mLcRect(this->pMainLayout->GetPos());
	if(mLcRect.Width()>0&&mLcRect.Height()>0&&mLcRect.top>=0&&mLcRect.left>=0)
	{
		::MoveWindow(this->m_SqliteJournalDuiDlg,mLcRect.left,mLcRect.top,mLcRect.Width(),mLcRect.Height(),TRUE);
		::MoveWindow(this->m_BrowseRemoteDuiDlg,mLcRect.left,mLcRect.top,mLcRect.Width(),mLcRect.Height(),TRUE);
		::MoveWindow(this->m_PlayVideoDuiDlg,mLcRect.left,mLcRect.top,mLcRect.Width(),mLcRect.Height(),TRUE);
	}
	return true;
}

void CMainFrameDlg::InitWindow()
{
	{
		assert(this->bInitControlUI==FALSE);
		CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pMainLayout,_T("HorizontalLayout_wnd"));
		CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_viewLog,_T("Option_viewLog"));
		CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_remotFile,_T("Option_remotFile"));
		CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_playVideo,_T("Option_playVideo"));
		CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pCloseBtn,_T("closebtn"));
		CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pMaxBtn,_T("maxbtn"));
		CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pRestoreBtn,_T("restorebtn"));
		CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pMinBtn,_T("minbtn"));
		CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pSettingBtn,_T("setting_btn"));
		this->bInitControlUI=TRUE;
	}
	{
		this->m_BrowseRemoteDuiDlg.Create(*this,NULL,UI_WNDSTYLE_CHILD,WS_EX_ACCEPTFILES);
		this->m_PlayVideoDuiDlg.Create(*this,NULL,UI_WNDSTYLE_CHILD,0);
		this->m_SqliteJournalDuiDlg.Create(*this,NULL,UI_WNDSTYLE_CHILD,0);
		this->m_LogSettingWnd.Create(*this,NULL,UI_WNDSTYLE_FRAME,0);
		this->ShowWindow();
		this->m_BrowseRemoteDuiDlg.ShowWindow(false);
		this->m_PlayVideoDuiDlg.ShowWindow(false);
		this->m_LogSettingWnd.ShowWindow(false);
		this->m_SqliteJournalDuiDlg.ShowWindow(true);
	}
	{
		this->pMainLayout->OnSize += MakeDelegate(this,&CMainFrameDlg::OnLayoutWndSizeEvent);
		this->m_PaintManager.SetMinInfo(680,400);
		//this->pMainLayout->Add(&this->m_SqliteJournalDuiDlg);
		//::SetTimer(this->GetHWND(),1110,100,NULL);
	}
	return __super::InitWindow();
}

LRESULT CMainFrameDlg::MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled)
{
	return __super::MessageHandler(_uMsg,_wParam,_lParam,_bHandled);
}

LRESULT CMainFrameDlg::HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
{
	//if(_uMsg!=WM_TIMER)
	//{
	//	CString sss;
	//	static int iii=0;
	//	sss.Format(_T("dsf:%0X	[%d]\n"),(int)_uMsg,(int)iii++);
	//	::OutputDebugString(sss);
	//}
	switch(_uMsg)
	{
	case WM_TIMER:
		{
			//this->ShowWindow();
			int jkldsf=0;
		}
		break;
	case WM_PAINT:
		{
			//this->ShowWindow();
			int jkldsf=0;
		}
		break;
	case WM_MOVE:
		{
			int sdf=0;
			if(::IsWindow(this->m_PlayVideoDuiDlg.GetHWND()))
			this->m_PlayVideoDuiDlg.PostMessage(_uMsg,_wParam,_lParam);
		}
		break;
		//case WM_DESTROY:
		//case WM_QUIT:
		//case WM_CLOSE:
		//	this->PostMessage(WM_QUIT);
		//	break;
	case WM_SYSCOMMAND:
		{
			switch(_wParam)
			{
			case SC_CLOSE:
				this->PostMessage(WM_QUIT);
				break;
			default:
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int aaa=0;
		}
		break;
	default:
		{
			int jkldsf=0;
		}
		break;
	}
	return __super::HandleMessage(_uMsg,_wParam,_lParam);
}

void CMainFrameDlg::Notify(DuiLib::TNotifyUI& _msg)
{
	if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0) 
	{
		if(_msg.pSender==this->m_pCloseBtn)
		{
			this->PostMessage(WM_QUIT);
			return ;
		}
		else if(_msg.pSender==this->m_pMaxBtn)
		{
			this->SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			return ;
		}
		else if(_msg.pSender==this->m_pMinBtn)
		{
			this->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
			return ;
		}
		else if(_msg.pSender==this->m_pRestoreBtn)
		{
			this->SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
			return ;
		}
		else if(_msg.pSender==this->m_pSettingBtn)
		{
			this->getDuiListMenuExt()->run(TxCppPlatform::bind(&CMainFrameDlg::menuOptionClickEvent,
				this,TxCppPlatform::placeholders::_1),
				_T("ÉèÖÃ"),e_menu_option_setting,
				_T("ÈÎÎñÀ¸"),e_menu_option_task,
				NULL);
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_SELECTCHANGED)==0)
	{
		if(_msg.pSender==this->p_Option_viewLog)
		{
			this->m_BrowseRemoteDuiDlg.ShowWindow(false);
			this->m_PlayVideoDuiDlg.showTabWindow(false);
			this->m_SqliteJournalDuiDlg.ShowWindow(true);
		}
		else if(_msg.pSender==this->p_Option_remotFile)
		{
			this->m_SqliteJournalDuiDlg.ShowWindow(false);
			this->m_PlayVideoDuiDlg.showTabWindow(false);
			this->m_BrowseRemoteDuiDlg.ShowWindow(true);
		}
		else if(_msg.pSender==this->p_Option_playVideo)
		{
			this->m_SqliteJournalDuiDlg.ShowWindow(false);
			this->m_BrowseRemoteDuiDlg.ShowWindow(false);
			this->m_PlayVideoDuiDlg.showTabWindow(true);
		}
	}
	return __super::Notify(_msg);
}

void CMainFrameDlg::menuOptionClickEvent(int _id)
{
	switch(_id)
	{
	case e_menu_option_setting:
		m_LogSettingWnd.dealSettingConfig();
		break;
	case e_menu_option_task:
		this->m_BrowseRemoteDuiDlg.showDownUpLoadTaskWnd();
		break;
	default:
		assert(0);
		break;
	}
}

void CMainFrameDlg::vfSelectTabWidget(int _iIndex)
{
	switch(_iIndex)
	{
	case 0:
		this->p_Option_remotFile->Selected(true);
		break;
	case 1:
		this->p_Option_remotFile->Selected(true);
		break;
	case 2:
		this->p_Option_playVideo->Selected(true);
		break;
	default:
		assert(0);
		break;
	}
}

