#include "StdAfx.h"

#include "../include/CSqliteExpandDll.h"
#include "CLogItemDetailWnd.h"

CLogItemDetailWnd::CLogItemDetailWnd()
{
	this->bInitControlUI=FALSE;
}

CLogItemDetailWnd::~CLogItemDetailWnd()
{
}

DuiLib::CDuiString CLogItemDetailWnd::GetSkinFolder()
{
	return DefDuiSkin;
}

DuiLib::CDuiString CLogItemDetailWnd::GetSkinFile()
{
	return _T("journal_item.xml");
}

LPCTSTR CLogItemDetailWnd::GetWindowClassName() const
{
	return _T("CLogItemDetailWnd");
}

UINT CLogItemDetailWnd::GetClassStyle() const
{
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
}

DuiLib::UILIB_RESOURCETYPE CLogItemDetailWnd::GetResourceType() const
{
	return DuiLib::UILIB_FILE;
}

void CLogItemDetailWnd::InitWindow()
{
	assert(this->bInitControlUI==FALSE);

	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pEdit_softname,_T("Edit_softname"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pEdit_filename,_T("Edit_filename"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pEdit_funcname,_T("Edit_funcname"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pEdit_linenumber,_T("Edit_linenumber"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pEdit_ostime,_T("Edit_ostime"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pEdit_softtime,_T("Edit_softtime"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pEdit_svrtime,_T("Edit_svrtime"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pEdit_msglevel,_T("Edit_msglevel"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pEdit_msgrand,_T("Edit_msgrand"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pRichEdit_msgcontent,_T("RichEdit_msgcontent"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pCloseBtn,_T("closebtn"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pMaxBtn,_T("maxbtn"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pRestoreBtn,_T("restorebtn"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pMinBtn,_T("minbtn"));

	this->bInitControlUI=TRUE;

	this->ResizeClient(480,460);
	this->m_PaintManager.SetMinInfo(380,360);
}

void CLogItemDetailWnd::Notify(DuiLib::TNotifyUI& _msg)
{
	if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0) 
	{
		if(_msg.pSender==this->m_pCloseBtn)
		{
			this->Close();
			return ;
		}
		else if(_msg.pSender==this->m_pMaxBtn)
		{
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			return ;
		}
		else if(_msg.pSender==this->m_pMinBtn)
		{
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
			return ;
		}
		else if(_msg.pSender==this->m_pRestoreBtn)
		{
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
			return ;
		}
	}
	return __super::Notify(_msg);
}

DuiLib::CDuiString CLogItemDetailWnd::transGetStandTimeString(const CDuiListWrapExt::tagListItem *_data,int _col_index)
{
	DuiLib::CDuiString ret;
	CString str=_data->mItemText[_col_index];
	if(str.GetLength()==14)
	{
		ret.Format(_T("%s年%s月%s日 %s时%s分%s秒"),
			(LPCTSTR)str.Mid( 0,4),
			(LPCTSTR)str.Mid( 4,2),
			(LPCTSTR)str.Mid( 6,2),
			(LPCTSTR)str.Mid( 8,2),
			(LPCTSTR)str.Mid(10,2),
			(LPCTSTR)str.Mid(12,2));
	}
	else ret=(LPCTSTR)str;
	return ret;
}

DuiLib::CDuiString CLogItemDetailWnd::transGetStandLevelString(const CDuiListWrapExt::tagListItem *_data,int _col_index)
{
	DuiLib::CDuiString ret;
	CString str=_data->mItemText[_col_index];
	switch(str.GetLength()>0?((int)str[0]-'0'):(-1))
	{
	case eSqliteJournalLevelInfo:
		ret=_T("信息");
		break;
	case eSqliteJournalLevelWarn:
		ret=_T("警告");
		break;
	case eSqliteJournalLevelError:
		ret=_T("错误");
		break;
	case eSqliteJournalLevelFatal:
		ret=_T("异常");
		break;
	default:
		ret=_T("未知");
		break;
	}
	return ret;
}

void CLogItemDetailWnd::set(const CDuiListWrapExt::tagListItem *_data)
{
	assert(_data!=NULL);
	if(_data==NULL) return ;
	this->m_pEdit_softname->SetText((LPCTSTR)(_data->mItemText[CListCtrlColumnInfo::e_listctrl_column_proc_name]));
	this->m_pEdit_filename->SetText((LPCTSTR)(_data->mItemText[CListCtrlColumnInfo::e_listctrl_column_src_file]));
	this->m_pEdit_funcname->SetText((LPCTSTR)(_data->mItemText[CListCtrlColumnInfo::e_listctrl_column_src_func]));
	this->m_pEdit_linenumber->SetText((LPCTSTR)(_data->mItemText[CListCtrlColumnInfo::e_listctrl_column_src_line]));

	this->m_pEdit_softtime->SetText(transGetStandTimeString(_data,CListCtrlColumnInfo::e_listctrl_column_soft_time));
	this->m_pEdit_ostime->SetText(transGetStandTimeString(_data,CListCtrlColumnInfo::e_listctrl_column_os_time));
	this->m_pEdit_svrtime->SetText(transGetStandTimeString(_data,CListCtrlColumnInfo::e_listctrl_column_svr_time));

	this->m_pEdit_msglevel->SetText(transGetStandLevelString(_data,CListCtrlColumnInfo::e_listctrl_column_msg_level));
	this->m_pEdit_msgrand->SetText((LPCTSTR)(_data->mItemText[CListCtrlColumnInfo::e_listctrl_column_msg_rand]));

	this->m_pRichEdit_msgcontent->SetText((LPCTSTR)(_data->mItemText[CListCtrlColumnInfo::e_listctrl_column_msg_content]));
}
