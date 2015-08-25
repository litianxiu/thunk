#include "stdafx.h"
#include "CLogSettingWnd.h"


CLogSettingWnd::CLogSettingWnd(CMainFrameDlgBasic *_pMainFrameDlgBasic):pMainFrameDlgBasic(_pMainFrameDlgBasic)
{
	this->bInitControlUI=FALSE;
}

CLogSettingWnd::~CLogSettingWnd()
{
}

DuiLib::CDuiString CLogSettingWnd::GetSkinFolder()
{
	return DefDuiSkin;
}

DuiLib::CDuiString CLogSettingWnd::GetSkinFile()
{
	return _T("journal_setting.xml");
}

LPCTSTR CLogSettingWnd::GetWindowClassName() const
{
	return _T("CLogSettingWnd");
}

UINT CLogSettingWnd::GetClassStyle() const
{
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
}

DuiLib::UILIB_RESOURCETYPE CLogSettingWnd::GetResourceType() const
{
	return DuiLib::UILIB_FILE;
}

void CLogSettingWnd::InitWindow()
{
	DefAssertTextWrap(this->bInitControlUI==FALSE,"");
	
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pEdit_iNetWork_Mtu,_T("Edit_iNetWork_Mtu"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pEdit_iSvcThreadCount,_T("Edit_iSvcThreadCount"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pOption_bKillOtherSameNameProcess,_T("Option_bKillOtherSameNameProcess"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pButton_addPortServer,_T("Button_addPortServer"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pList_portServer,_T("List_portServer"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pOption_bServerInfo_bEnableRemotFileServer,_T("Option_bServerInfo_bEnableRemotFileServer"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pEdit_iServerResponseLogMaxCount,_T("Edit_iServerResponseLogMaxCount"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pEdit_iServerResponseLogMaxMemory,_T("Edit_iServerResponseLogMaxMemory"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pEdit_iViewLogMaxCount,_T("Edit_iViewLogMaxCount"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pEdit_iViewLogWaitMaxTime,_T("Edit_iViewLogWaitMaxTime"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pEdit_iExecSqlWaitMaxTime,_T("Edit_iExecSqlWaitMaxTime"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pbtn_ok,_T("btn_ok"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pbtn_cancel,_T("btn_cancel"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->pbtn_appy,_T("btn_appy"));

	this->bInitControlUI=TRUE;
}

void CLogSettingWnd::dealSettingConfig()
{
tagConfigSettingData *pLcCfgData=this->pMainFrameDlgBasic->getConfigSettingDataPtr();
	this->pEdit_iNetWork_Mtu->SetText(TxBlobString::fromDatum(pLcCfgData->iNetWork_Mtu).c_str());
	this->pEdit_iSvcThreadCount->SetText(TxBlobString::fromDatum(pLcCfgData->iSvcThreadCount).c_str());
	this->pOption_bKillOtherSameNameProcess->Selected(pLcCfgData->bKillOtherSameNameProcess);

	this->pOption_bServerInfo_bEnableRemotFileServer->Selected(!!pLcCfgData->mServerInfo.bEnableRemotFileServer);
	this->pEdit_iServerResponseLogMaxCount->SetText(TxBlobString::fromDatum(pLcCfgData->mServerInfo.iServerResponseLogMaxCount).c_str());
	this->pEdit_iServerResponseLogMaxMemory->SetText(TxBlobString::fromDatum(pLcCfgData->mServerInfo.iServerResponseLogMaxMemory).c_str());

	this->pEdit_iViewLogMaxCount->SetText(TxBlobString::fromDatum(pLcCfgData->mClientInfo.iViewLogMaxCount).c_str());
	this->pEdit_iViewLogWaitMaxTime->SetText(TxBlobString::fromDatum(pLcCfgData->mClientInfo.iViewLogWaitMaxTime).c_str());
	this->pEdit_iExecSqlWaitMaxTime->SetText(TxBlobString::fromDatum(pLcCfgData->mClientInfo.iExecSqlWaitMaxTime).c_str());

	std::vector<tagConfigSettingSqliteServerPort>::const_iterator iter=pLcCfgData->mServerInfo.listSqliteServer.begin(),
		iter_end=pLcCfgData->mServerInfo.listSqliteServer.end();

	this->pList_portServer->RemoveAll();
	this->m_DuiListWrapExt.removeAll();
	for(;iter!=iter_end;iter++)
		this->addServerPortListView(iter->port,iter->filename.c_str(),iter->bJournal);

	this->CenterWindow();
	this->ShowWindow(true);
}

void CLogSettingWnd::addServerPortListView(int _port,LPCTSTR _filename,bool _blog)
{
	TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> lcNewElem=this->m_DuiListWrapExt.insertBack(new DuiLib::CListTextElementUI());
	lcNewElem->mItemText.resize(3);
	lcNewElem->mItemText[0]=TxBlobString::fromDatum(_port).c_str();
	lcNewElem->mItemText[1]=_filename;
	lcNewElem->mItemText[2]=(_blog?_T("1"):_T("0"));
	this->pList_portServer->Add(lcNewElem->ui);
}

bool CLogSettingWnd::dealSaveConfig()
{
	tagConfigSettingData lc_data=this->getDuiDlgData();

	const char *_err=NULL;
	_err=lc_data.saveConfig();
	if(_err!=NULL)
	{
		TxDuiMessageBox::run(*this,_T("保存配置失败"),_err,_T("确定"));
		return false;
	}
	tagConfigSettingData lc_data_2;
	_err=lc_data_2.loadConfig();
	if(_err!=NULL)
	{
		TxDuiMessageBox::run(*this,_T("保存配置失败"),_err,_T("确定"));
		return false;
	}
	this->pMainFrameDlgBasic->getConfigSettingDataPtr()[0]=lc_data_2;
	return true;
}

tagConfigSettingData CLogSettingWnd::getDuiDlgData()
{
	tagConfigSettingData ret;
	TxBlobString((LPCTSTR)this->pEdit_iNetWork_Mtu->GetText()).toDatum(ret.iNetWork_Mtu);
	TxBlobString((LPCTSTR)this->pEdit_iSvcThreadCount->GetText()).toDatum(ret.iSvcThreadCount);
	ret.bKillOtherSameNameProcess=!!this->pOption_bKillOtherSameNameProcess->IsSelected();

	ret.mServerInfo.bEnableRemotFileServer=!!this->pOption_bServerInfo_bEnableRemotFileServer->IsSelected();
	TxBlobString((LPCTSTR)this->pEdit_iServerResponseLogMaxCount->GetText()).toDatum(ret.mServerInfo.iServerResponseLogMaxCount);
	TxBlobString((LPCTSTR)this->pEdit_iServerResponseLogMaxMemory->GetText()).toDatum(ret.mServerInfo.iServerResponseLogMaxMemory);

	TxBlobString((LPCTSTR)this->pEdit_iViewLogMaxCount->GetText()).toDatum(ret.mClientInfo.iViewLogMaxCount);
	TxBlobString((LPCTSTR)this->pEdit_iViewLogWaitMaxTime->GetText()).toDatum(ret.mClientInfo.iViewLogWaitMaxTime);
	TxBlobString((LPCTSTR)this->pEdit_iExecSqlWaitMaxTime->GetText()).toDatum(ret.mClientInfo.iExecSqlWaitMaxTime);

	for(int y=0;y<this->m_DuiListWrapExt.getItemCount();y++)
	{
		tagConfigSettingSqliteServerPort tem;
		TxBlobString::fromDatum((LPCTSTR)this->m_DuiListWrapExt.at(y)->mItemText[0]).toDatum(tem.port);
		TxBlobString::fromDatum((LPCTSTR)this->m_DuiListWrapExt.at(y)->mItemText[1]).toDatum(tem.filename);
		TxBlobString::fromDatum((LPCTSTR)this->m_DuiListWrapExt.at(y)->mItemText[2]).toDatum(tem.bJournal);
		ret.mServerInfo.listSqliteServer.push_back(tem);
	}
	return ret;
}

LRESULT CLogSettingWnd::MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled)
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

void CLogSettingWnd::logServerListMenuClick(int _item,int _i_clck_index)
{
	switch(_i_clck_index)
	{
	case e_list_menu_svr_edit:
		{
			CLogSettingAddEditSrvEditWnd::tagSettingSrvInfo lc_data;
			lc_data.port=0;
			TxBlobString::fromDatum((LPCTSTR)this->m_DuiListWrapExt.at(_item)->mItemText[0]).toDatum(lc_data.port);
			lc_data.strFileName=(LPCTSTR)this->m_DuiListWrapExt.at(_item)->mItemText[1];
			lc_data.bIsLog=true;
			TxBlobString::fromDatum((LPCTSTR)this->m_DuiListWrapExt.at(_item)->mItemText[2]).toDatum(lc_data.bIsLog);
			if(CLogSettingAddEditSrvEditWnd::run(&lc_data,*this,_T("确定"),_T("取消"))==0)
			{
				this->m_DuiListWrapExt.at(_item)->mItemText[0]=TxBlobString::fromDatum(lc_data.port).c_str();
				this->m_DuiListWrapExt.at(_item)->mItemText[1]=lc_data.strFileName.c_str();
				this->m_DuiListWrapExt.at(_item)->mItemText[2]=TxBlobString::fromDatum(lc_data.bIsLog).c_str();
				this->m_DuiListWrapExt.at(_item)->ui->Invalidate();
			}
		}
		break;
	case e_list_menu_svr_del:
		{
			if(this->m_DuiListWrapExt.at(_item)->ui==this->pList_portServer->GetItemAt(_item)&&this->pList_portServer->RemoveAt(_item))
			{
				this->m_DuiListWrapExt.erase(_item);
			}
			else
			{
				DefAssertWrap(FALSE);
			}
		}
		break;
	default:
		DefAssertWrap(FALSE);
		break;
	}
}

void CLogSettingWnd::Notify(DuiLib::TNotifyUI& _msg)
{
	if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0) 
	{
		if(_msg.pSender==this->pbtn_appy)
		{
			if(this->dealSaveConfig())
				this->dealSettingConfig();
			return ;
		}
		else if(_msg.pSender==this->pbtn_ok)
		{
			if(this->dealSaveConfig())
				this->ShowWindow(false);
			return ;
		}
		else if(_msg.pSender==this->pbtn_cancel)
		{
			this->ShowWindow(false);
			return ;
		}
		else if(_msg.pSender==this->pButton_addPortServer)
		{
			CLogSettingAddEditSrvEditWnd::tagSettingSrvInfo lc_data;
			lc_data.strFileName="";
			lc_data.port=0;
			lc_data.bIsLog=true;
			if(CLogSettingAddEditSrvEditWnd::run(&lc_data,*this,_T("确定"),_T("取消"))==0)
			{
				this->addServerPortListView(lc_data.port,lc_data.strFileName.c_str(),lc_data.bIsLog);
			}
			return ;
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_WINDOWINIT)==0)
	{
		this->initListServerView();
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_MENU)==0)
	{
		if(_msg.pSender==this->pList_portServer)
		{
			int iLcCurItem=this->pList_portServer->GetCurSel();
			if(iLcCurItem>=0)
			{
				this->pMainFrameDlgBasic->getDuiListMenuExt()->run(TxCppPlatform::bind(&CLogSettingWnd::logServerListMenuClick,
					this,iLcCurItem,TxCppPlatform::placeholders::_1),
					_T("修改"),e_list_menu_svr_edit,
					_T("删除"),e_list_menu_svr_del);
			}
		}
	}
	return __super::Notify(_msg);
}

void CLogSettingWnd::initListServerView()
{
	this->pList_portServer->SetTextCallback(&this->m_DuiListWrapExt);
	DuiLib::CListHeaderUI *pLcHead=this->pList_portServer->GetHeader();
	enum { e_lc_port_w=50,e_lc_log_w=50, };
	for(int i=0;;i++)
	{
		LPCTSTR lc_str=NULL;
		int lc_w=10;
		switch(i)
		{
		case 0:
			lc_str=_T("端口");
			lc_w=e_lc_port_w;
			break;
		case 1:
			lc_str=_T("文件");
			lc_w=CRect(this->pList_portServer->GetPos()).Width()-e_lc_log_w-e_lc_port_w-8;
			break;
		case 2:
			lc_str=_T("日志");
			lc_w=e_lc_log_w;
			break;
		default:
			break;
		}
		if(lc_str==NULL) break;

		DuiLib::CListHeaderItemUI *pLcHeadItem=new DuiLib::CListHeaderItemUI();
		pLcHeadItem->SetText(lc_str);
		pLcHeadItem->SetSepImage(_T("list_header_sep.png"));
		pLcHeadItem->SetNormalImage(_T("list_head_normal.bmp"));
		pLcHeadItem->SetPushedImage(_T("list_head_down.bmp"));
		pLcHeadItem->SetSepWidth(1);
		pLcHeadItem->SetFixedWidth(lc_w);
		pLcHead->Add(pLcHeadItem);
	}
	this->pList_portServer->SetVisible(false);
	this->pList_portServer->SetVisible(true);
}



