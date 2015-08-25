#include "StdAfx.h"

#include "CLogSettingAddEditSrvEditWnd.h"

CLogSettingAddEditSrvEditWnd::CLogSettingAddEditSrvEditWnd()
{
	this->bInitControlUI=FALSE;
	this->result=-1;
}

DuiLib::CDuiString CLogSettingAddEditSrvEditWnd::GetSkinFolder()
{
	return DefDuiSkin;
}

DuiLib::CDuiString CLogSettingAddEditSrvEditWnd::GetSkinFile()
{
	return _T("journal_setting_edit_server.xml");
}

LPCTSTR CLogSettingAddEditSrvEditWnd::GetWindowClassName() const
{
	return _T("CLogSettingAddEditSrvEditWnd");
}

UINT CLogSettingAddEditSrvEditWnd::GetClassStyle() const
{
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
}

DuiLib::UILIB_RESOURCETYPE CLogSettingAddEditSrvEditWnd::GetResourceType() const
{
	return DuiLib::UILIB_FILE;
}

void CLogSettingAddEditSrvEditWnd::InitWindow()
{
	assert(this->bInitControlUI==FALSE);

	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pEdit_svr_port,_T("Edit_svr_port"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pEdit_db_filename,_T("Edit_db_filename"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pOption_is_log,_T("Option_is_log"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pbtn_0,_T("btn_0"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pbtn_1,_T("btn_1"));

	this->bInitControlUI=TRUE;
}

LRESULT CLogSettingAddEditSrvEditWnd::MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled)
{
	switch(_uMsg)
	{
	case WM_KEYDOWN:
		if(_wParam==VK_ESCAPE||_wParam==VK_CANCEL)
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

void CLogSettingAddEditSrvEditWnd::Notify(DuiLib::TNotifyUI& _msg)
{
	if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0) 
	{
		if(_msg.pSender==this->pbtn_0)
		{
			this->result=0;
			this->Close();
			return ;
		}
		else if(_msg.pSender==this->pbtn_1)
		{
			this->result=1;
			this->Close();
			return ;
		}
	}
	return __super::Notify(_msg);
}

int CLogSettingAddEditSrvEditWnd::run(tagSettingSrvInfo *_data,HWND hwndParent,LPCTSTR _btn0,LPCTSTR _btn1)
{
	CLogSettingAddEditSrvEditWnd mWnd;
	mWnd.Create(NULL,NULL,UI_WNDSTYLE_FRAME,0);

	mWnd.pEdit_db_filename->SetText(_data->strFileName.c_str());
	if(_data->port>0) mWnd.pEdit_svr_port->SetText(TxBlobString::fromDatum(_data->port).c_str());
	mWnd.pOption_is_log->Selected(!!_data->bIsLog);

	mWnd.CenterWindow();
	mWnd.ShowModal();

	_data->strFileName.assign((LPCTSTR)mWnd.pEdit_db_filename->GetText());
	TxBlobString((LPCTSTR)mWnd.pEdit_svr_port->GetText()).toDatum(_data->port);
	_data->bIsLog=mWnd.pOption_is_log->IsSelected();

	return mWnd.result;
}

