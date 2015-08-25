#include "stdafx.h"

#include "CJournalDuiExtSqliteExec.hpp"
#include "CSqliteJournalDuiDlg.h"
#include "../common/TxDuiMessageBox.hpp"
#include "CSqliteLogExec.h"


CSqliteJournalDuiDlg::CSqliteJournalDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic)
	:m_DuiLogViewListWrapExt(&m_ListCtrlColumnInfo),pMainFrameDlgBasic(_pMainFrameDlgBasic)
{
	this->bInitControlUI=FALSE;
	this->iTimerId=0;
	this->llMonitorSqlIndex=-1;
}

CSqliteJournalDuiDlg::~CSqliteJournalDuiDlg()
{
}

DuiLib::CDuiString CSqliteJournalDuiDlg::GetSkinFolder()
{
	return DefDuiSkin;
}

DuiLib::CDuiString CSqliteJournalDuiDlg::GetSkinFile()
{
	return _T("sqlite_journal.xml");
}

LPCTSTR CSqliteJournalDuiDlg::GetWindowClassName() const
{
	return _T("CSqliteJournalDuiDlg");
}

UINT CSqliteJournalDuiDlg::GetClassStyle() const
{
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
}

DuiLib::UILIB_RESOURCETYPE CSqliteJournalDuiDlg::GetResourceType() const
{
	return DuiLib::UILIB_FILE;
}

bool CSqliteJournalDuiDlg::OnTheLogListResizeEvent(void* _pParam)
{
	DuiLib::CControlUI *pLcParam=(DuiLib::CControlUI*)_pParam;
	if(this->m_pListLogContent==pLcParam)
	{
		CRect mLcSqlTextRect(this->m_pListLogContent->GetPos());
		this->doRefreshListItemWidth(mLcSqlTextRect.Width());
	}
	return true;
}

void CSqliteJournalDuiDlg::initListLogContent()
{
	this->m_pListLogContent->SetTextCallback(&this->m_DuiLogViewListWrapExt);
	DuiLib::CListHeaderUI *pLcHead=this->m_pListLogContent->GetHeader();
	for(int i=0;i<(int)this->m_ListCtrlColumnInfo.getColumnCount();i++)
	{
		DuiLib::CListHeaderItemUI *pLcHeadItem=new DuiLib::CListHeaderItemUI();
		pLcHeadItem->SetText(this->m_ListCtrlColumnInfo.getColumnName(i));
		pLcHeadItem->SetSepImage(_T("list_header_sep.png"));
		pLcHeadItem->SetNormalImage(_T("list_head_normal.bmp"));
		pLcHeadItem->SetPushedImage(_T("list_head_down.bmp"));
		pLcHeadItem->SetSepWidth(E_LIST_ITEM_SEP_WIDTH);
		pLcHeadItem->SetFixedWidth(this->m_ListCtrlColumnInfo.getColumnWidth(i));
		pLcHead->Add(pLcHeadItem);
	}
	this->m_pListLogContent->OnSize+=DuiLib::MakeDelegate(this, &CSqliteJournalDuiDlg::OnTheLogListResizeEvent);
}

void CSqliteJournalDuiDlg::InitWindow()
{
	DefAssertTextWrap(this->bInitControlUI==FALSE,"");
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_pListLogContent,_T("list_log_content"));

	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_btnSqlMonitor,_T("btnSqlMonitor"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_btnSqlSelect,_T("btnSqlSelect"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_btnSqlExec,_T("btnSqlExec"));

	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_Edit_ip,_T("Edit_ip"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_Edit_port,_T("Edit_port"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_RichEdit_sqltext,_T("RichEdit_sqltext"));

	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_msglevel,_T("Option_msglevel"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_srcfilename,_T("Option_srcfilename"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_msgrand,_T("Option_msgrand"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_ostime,_T("Option_ostime"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_softname,_T("Option_softname"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_srcfuncname,_T("Option_srcfuncname"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_srclinenumber,_T("Option_srclinenumber"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_svrtime,_T("Option_svrtime"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_softtime,_T("Option_softtime"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_msgid,_T("Option_msgid"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->m_VerticalLayout_global,_T("VerticalLayout_global"));
	this->bInitControlUI=TRUE;

	this->initListLogContent();
}

bool CSqliteJournalDuiDlg::dtctOptionShow(DuiLib::COptionUI *_p_Option)
{
	if(_p_Option==NULL) return false;
	bool bLcIsSelect=(!_p_Option->IsSelected());
	if(_p_Option==this->p_Option_msglevel)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_msg_level,bLcIsSelect);
		return true;
	}
	else if(_p_Option==this->p_Option_srcfilename)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_src_file,bLcIsSelect);
		return true;
	}
	else if(_p_Option==this->p_Option_msgrand)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_msg_rand,bLcIsSelect);
		return true;
	}
	else if(_p_Option==this->p_Option_ostime)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_os_time,bLcIsSelect);
		return true;
	}
	else if(_p_Option==this->p_Option_softname)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_proc_name,bLcIsSelect);
		return true;
	}
	else if(_p_Option==this->p_Option_srcfuncname)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_src_func,bLcIsSelect);
		return true;
	}
	else if(_p_Option==this->p_Option_srclinenumber)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_src_line,bLcIsSelect);
		return true;
	}
	else if(_p_Option==this->p_Option_svrtime)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_svr_time,bLcIsSelect);
		return true;
	}
	else if(_p_Option==this->p_Option_softtime)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_soft_time,bLcIsSelect);
		return true;
	}
	else if(_p_Option==this->p_Option_msgid)
	{
		this->m_ListCtrlColumnInfo.enableCheckShowItem(CListCtrlColumnInfo::e_listctrl_column_src_id,bLcIsSelect);
		return true;
	}
	return false;
}

void CSqliteJournalDuiDlg::doRefreshListItemWidth(int _width)
{
	CRect lcRect(this->m_pListLogContent->GetPos());
	if(lcRect.Width()==_width)
		return ;
	if(_width<=0)
		_width=lcRect.Width();
	if(_width<=0) return ;

	for(int i=this->m_ListCtrlColumnInfo.getColumnCount();i--;)
		this->m_ListCtrlColumnInfo.setColumnWidth(i,this->m_pListLogContent->GetHeader()->GetItemAt(i)->GetWidth());

	this->m_ListCtrlColumnInfo.refreshTotalColumnWidth(_width,E_LIST_ITEM_SEP_WIDTH);
	for(int i=this->m_ListCtrlColumnInfo.getColumnCount();i--;)
	{
		this->m_pListLogContent->GetHeader()->GetItemAt(i)->SetFixedWidth(this->m_ListCtrlColumnInfo.getColumnWidth(i));
		this->m_pListLogContent->GetHeader()->GetItemAt(i)->SetVisible(this->m_ListCtrlColumnInfo.getColumnVisible(i));
	}
	this->m_pListLogContent->SetVisible(false);
	this->m_pListLogContent->SetVisible(true);
}

void CSqliteJournalDuiDlg::logListMenuClick(int _item,int _clik_index)
{
	switch(_clik_index)
	{
	case e_list_menu_log_view:
		{
			CLogItemDetailWnd *pLcWnd=new CLogItemDetailWnd();
			pLcWnd->Create(*this,NULL,UI_WNDSTYLE_FRAME,0); // 创建DLG窗口
			pLcWnd->CenterWindow(); // 窗口居中
			pLcWnd->set(this->m_DuiLogViewListWrapExt.at(_item).get());
			pLcWnd->ShowModal();
			delete pLcWnd;
		}
		break;
	case e_list_menu_log_attr:
		this->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(_T("该功能暂时没实现。"));
		break;
	default:
		DefAssertWrap(FALSE);
		break;
	}
}

LRESULT CSqliteJournalDuiDlg::HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch(_uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		if(this->iTimerId!=0)
		{
			::KillTimer(*this,this->iTimerId);
			this->iTimerId=0;
		}
		break;
	case WM_ERASEBKGND:
		break;
	case WM_TIMER:
		if(this->mTimerSpan_5000.dtctCurTimeResetSpanMsec(5*1000,true))
		{
			this->doTimerEvent5000();
		}
		break;
	default:
		break;
	}
	return __super::HandleMessage(_uMsg,_wParam,_lParam);
}

LRESULT CSqliteJournalDuiDlg::MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled)
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

void CSqliteJournalDuiDlg::Notify(DuiLib::TNotifyUI& _msg)
{
	if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0) 
	{
		if(this->dtctOptionShow(dynamic_cast<DuiLib::COptionUI*>(_msg.pSender)))
		{
			this->doRefreshListItemWidth();
			//CRect lcRect(this->m_pListLogContent->GetRelativePos());
			//if(lcRect.Width()>0&&lcRect.Height()>0)
			//	this->m_pListLogContent->SetPos(lcRect,true);
			return ;
		}
		else if(_msg.pSender==this->m_btnSqlMonitor)
		{
			this->clearLogMessageContent();
			if(this->llMonitorSqlIndex>=0)
			{//响应停止侦听
				this->llMonitorSqlIndex=-1;
				this->spMonitorClientSession.reset();
				this->m_btnSqlExec->SetEnabled(true);
				this->m_btnSqlSelect->SetEnabled(true);
				this->m_Edit_ip->SetEnabled(true);
				this->m_Edit_port->SetEnabled(true);
				this->m_btnSqlMonitor->SetText(this->strMonitorBtnText);
				this->spMonitorClientSession.reset();
			}
			else
			{//响应开始侦听
				TxBlobString sLcErrorText;
				if(this->btnMonitorSqlMessageContent(&sLcErrorText))
				{
					this->strMonitorBtnText=this->m_btnSqlMonitor->GetText();
					this->m_btnSqlMonitor->SetText(_T("停止侦听"));
					this->m_btnSqlExec->SetEnabled(false);
					this->m_btnSqlSelect->SetEnabled(false);
					this->m_Edit_ip->SetEnabled(false);
					this->m_Edit_port->SetEnabled(false);
				}
				else if(sLcErrorText.length()>0)
				{
					TxDuiMessageBox::run(*this,_T("错误"),sLcErrorText.c_str(),_T("确定"));
				}
				else
				{
					DefAssertWrap(FALSE);
					TxDuiMessageBox::run(*this,_T("错误"),_T("未知错误"),_T("确定"));
				}
			}
			return ;
		}
		else if(_msg.pSender==this->m_btnSqlSelect)
		{
			_msg.pSender->SetEnabled(false);
			this->clearLogMessageContent();
			TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在获取日志信息。")));

			TxBlobString str_ip;
			int iLcPort=0;
			if(CMainFrameDlgBasic::gTransTcpIpPort(&str_ip,&iLcPort,this->m_Edit_ip->GetText(),this->m_Edit_port->GetText()))
			{
				CJournalDuiExtSqliteExec m_exec(this,this->pMainFrameDlgBasic);
				m_exec.inquireNewestLogData(str_ip.c_str(),iLcPort,1000,30*1000);
				if(m_exec.getErrorText().length()>0)
				{
					this->pMainFrameDlgBasic->clearAutoWaitDuiDialog();
					TxDuiMessageBox::run(*this,_T("错误"),m_exec.getErrorText().c_str(),_T("确定"));
				}
				else if(m_exec.getLstGainRowCount()==0)
				{
					this->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(_T("找不到响应的记录"));
				}
			}
			_msg.pSender->SetEnabled(true);
			return ;
		}
		else if(_msg.pSender==this->m_btnSqlExec)
		{
			_msg.pSender->SetEnabled(false);
			this->clearLogMessageContent();
			TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在执行SQL语句\n。")));

			TxBlobString str_ip;
			int iLcPort=0;
			if(CMainFrameDlgBasic::gTransTcpIpPort(&str_ip,&iLcPort,this->m_Edit_ip->GetText(),this->m_Edit_port->GetText()))
			{
				CJournalDuiExtSqliteExec m_exec(this,this->pMainFrameDlgBasic);
				m_exec.executeSql(str_ip.c_str(),iLcPort,this->m_RichEdit_sqltext->GetText(),30*1000);
				if(m_exec.getErrorText().length()>0)
				{
					this->pMainFrameDlgBasic->clearAutoWaitDuiDialog();
					TxDuiMessageBox::run(*this,_T("错误"),m_exec.getErrorText().c_str(),_T("确定"));
				}
				else if(m_exec.getLstGainRowCount()==0)
				{
					this->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(_T("找不到响应的记录"));
				}
			}
			_msg.pSender->SetEnabled(true);
			return ;
		}
		else
		{
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_MENU)==0)
	{
		if(_msg.pSender==this->m_pListLogContent)
		{
			int iLcCurItem=this->m_pListLogContent->GetCurSel();
			if(iLcCurItem>=0)
			{
				this->pMainFrameDlgBasic->getDuiListMenuExt()->run(TxCppPlatform::bind(&CSqliteJournalDuiDlg::logListMenuClick,
					this,iLcCurItem,TxCppPlatform::placeholders::_1),
					_T("查看"),e_list_menu_log_view,
					_T("属性"),e_list_menu_log_attr);
				return ;
			}
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_HEADERCLICK)==0)
	{
		int iLcHeaderIndex=(int)this->m_pListLogContent->GetHeader()->GetItemIndex(_msg.pSender);
		if(iLcHeaderIndex>=0)
		{
			if(iLcHeaderIndex!=CListCtrlColumnInfo::e_listctrl_column_msg_content)
			{
				this->m_DuiLogViewListWrapExt.sort_all(iLcHeaderIndex);
				this->m_pListLogContent->Invalidate();
			}
			return ;
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_WINDOWINIT)==0)
	{
		DefAssertTextWrap(this->iTimerId==0,"");
		this->iTimerId=::SetTimer(*this,2251,1000,NULL);
		DefAssertTextWrap(this->iTimerId!=0,"");
	}
	return __super::Notify(_msg);
}

void CSqliteJournalDuiDlg::clearLogMessageContent()
{
	this->m_pListLogContent->RemoveAll();
	this->m_DuiLogViewListWrapExt.removeAll();
}

bool CSqliteJournalDuiDlg::btnMonitorSqlMessageContent(TxBlobString *_sLcErrorText)
{
	this->m_btnSqlMonitor->SetEnabled(false);
	TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在执行SQL语句。")));
	this->llMonitorSqlIndex=-1;
	this->clearLogMessageContent();
	TxBlobString str_ip((LPCTSTR)this->m_Edit_ip->GetText());
	str_ip=str_ip.trimLR("	\n\r\t");
	int iLcPort=TxBlobString((LPCTSTR)this->m_Edit_port->GetText()).trimLR("	\n\r\t").toDatumRet<int>(0);
	if(str_ip.length()>0&&iLcPort>0&&iLcPort<=65536)
	{
		if(!this->spMonitorClientSession)
		{
			TxBlobString str_ip((LPCTSTR)this->m_Edit_ip->GetText());
			str_ip=str_ip.trimLR("	\n\r\t");
			int iLcPort=TxBlobString((LPCTSTR)this->m_Edit_port->GetText()).trimLR("	\n\r\t").toDatumRet<int>(0);
			if(str_ip.length()>0&&iLcPort>0&&iLcPort<=65536)
				this->spMonitorClientSession.reset(new CDbmsClientSessionDll(str_ip.c_str(),iLcPort));
		}
		this->llMonitorSqlIndex=CSqliteLogExec::execNewestLogDataId(this->spMonitorClientSession.get(),_sLcErrorText);
		//this->llMonitorSqlIndex=CSqliteLogExec::execNewestLogDataId(str_ip.c_str(),iLcPort);
	}
	this->m_btnSqlMonitor->SetEnabled(true);
	return this->llMonitorSqlIndex>=0;
}

void CSqliteJournalDuiDlg::doTimerEvent5000()
{
	if(this->llMonitorSqlIndex>=0)
	{//以下是侦听日志事件
		CJournalDuiExtSqliteExec mLcExec(this,NULL);
		mLcExec.executeSql(this->spMonitorClientSession.get(),(TxBlobString("select * from journal where id > ")
			+TxBlobString::fromDatum(this->llMonitorSqlIndex)+" limit 1000;").c_str(),2*1000);
		if(mLcExec.getErrorText().length()>0)
			this->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(mLcExec.getErrorText().c_str());
	}
	else if(this->spMonitorClientSession)
	{
		this->spMonitorClientSession.reset();
	}
}
