#pragma once

#include <UIlib.h>
#include "../common/TxDuiListMenuExt.hpp"
#include "CConfigSettingData.hpp"
#include "CLogSettingAddEditSrvEditWnd.h"
#include "CDuiListWrapExt.h"
#include "CMainFrameDlgBasic.hpp"


class CLogSettingWnd: public  DuiLib::WindowImplBase
{
private:
	DuiLib::CEditUI *pEdit_iNetWork_Mtu;//Edit_iNetWork_Mtu
	DuiLib::CEditUI *pEdit_iSvcThreadCount;//Edit_iSvcThreadCount
	DuiLib::COptionUI *pOption_bKillOtherSameNameProcess;//Edit_iSvcThreadCount
	
	DuiLib::CButtonUI *pButton_addPortServer;//Button_addPortServer
	DuiLib::CListUI *pList_portServer;//List_portServer

	DuiLib::COptionUI *pOption_bServerInfo_bEnableRemotFileServer;//Option_bServerInfo_bEnableRemotFileServer
	DuiLib::CEditUI *pEdit_iServerResponseLogMaxCount;//Edit_iServerResponseLogMaxCount
	DuiLib::CEditUI *pEdit_iServerResponseLogMaxMemory;//Edit_iServerResponseLogMaxMemory

	DuiLib::CEditUI *pEdit_iViewLogMaxCount;//Edit_iViewLogMaxCount
	DuiLib::CEditUI *pEdit_iViewLogWaitMaxTime;//Edit_iViewLogWaitMaxTime
	DuiLib::CEditUI *pEdit_iExecSqlWaitMaxTime;//Edit_iExecSqlWaitMaxTime

	DuiLib::CButtonUI *pbtn_ok;
	DuiLib::CButtonUI *pbtn_cancel;
	DuiLib::CButtonUI *pbtn_appy;

	BOOL bInitControlUI;
	CMainFrameDlgBasic *const pMainFrameDlgBasic;
	CDuiListWrapExt m_DuiListWrapExt;
	enum {
		e_list_menu_svr_null=0,
		e_list_menu_svr_del,
		e_list_menu_svr_edit,
	};
public :
	CLogSettingWnd(CMainFrameDlgBasic *_pMainFrameDlgBasic);
	~CLogSettingWnd();
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	DuiLib::UILIB_RESOURCETYPE GetResourceType() const;
	virtual void InitWindow();
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled);
	void dealSettingConfig();
	bool dealSaveConfig();
	tagConfigSettingData getDuiDlgData();
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	void initListServerView();
	void addServerPortListView(int _port,LPCTSTR _filename,bool _blog);
	void logServerListMenuClick(int _item,int _i_clck_index);
};


