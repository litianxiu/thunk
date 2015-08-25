#pragma once

#include "stdafx.h"
#include "resource.h"

#include "CDuiListWrapExt.h"
#include "CListCtrlColumnInfo.h"
#include "CLogItemDetailWnd.h"
#include "../common/TxIocpTcpApi.hpp"
#include "CSqliteLogExec.h"
#include "CLogSettingWnd.h"
#include "CMainFrameDlgBasic.hpp"

class CSqliteJournalDuiDlg: public DuiLib::WindowImplBase// , public DuiLib::CControlUI
{
private:
	DuiLib::CListUI* m_pListLogContent;

	DuiLib::CButtonUI* m_btnSqlMonitor;
	DuiLib::CButtonUI* m_btnSqlSelect;
	DuiLib::CButtonUI* m_btnSqlExec;

	DuiLib::CEditUI *m_Edit_ip;
	DuiLib::CEditUI *m_Edit_port;

	DuiLib::CRichEditUI *m_RichEdit_sqltext;

	DuiLib::COptionUI *p_Option_msglevel;
	DuiLib::COptionUI *p_Option_srcfilename;
	DuiLib::COptionUI *p_Option_msgrand;
	DuiLib::COptionUI *p_Option_ostime;
	DuiLib::COptionUI *p_Option_softname;
	DuiLib::COptionUI *p_Option_srcfuncname;
	DuiLib::COptionUI *p_Option_srclinenumber;
	DuiLib::COptionUI *p_Option_svrtime;
	DuiLib::COptionUI *p_Option_softtime;
	DuiLib::COptionUI *p_Option_msgid;

	DuiLib::CVerticalLayoutUI *m_VerticalLayout_global;

	BOOL bInitControlUI;

	enum { E_LIST_ITEM_SEP_WIDTH=1, };
	enum {
		e_list_menu_log_null=0,
		e_list_menu_log_attr,
		e_list_menu_log_view,
	};

	CListCtrlColumnInfo m_ListCtrlColumnInfo;
	CDuiLogViewListWrapExt m_DuiLogViewListWrapExt;
	int iTimerId;
	TxSystemDependent::TxTimeSpan<false> mTimerSpan_5000;
	long long llMonitorSqlIndex;
	TxCppPlatform::shared_ptr<CDbmsClientSessionDll> spMonitorClientSession;
	DuiLib::CDuiString strMonitorBtnText;
	CMainFrameDlgBasic *const pMainFrameDlgBasic;
private:
	bool dtctOptionShow(DuiLib::COptionUI *_p_Option);
	void doRefreshListItemWidth(int _width=-1);
public :
	CDuiListWrapExt* getLogContentListWrapExt() { return &this->m_DuiLogViewListWrapExt; }
	CListCtrlColumnInfo* getListCtrlColumnInfo() { return &this->m_ListCtrlColumnInfo; }
	DuiLib::CListUI* getListLogContent() { return this->m_pListLogContent; }
public :
	CSqliteJournalDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic);
	~CSqliteJournalDuiDlg();
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	virtual DuiLib::UILIB_RESOURCETYPE GetResourceType() const;
	void initListLogContent();
	virtual void InitWindow();
	void logListMenuClick(int _item,int _clik_index);
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled);
	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam);
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	void clearLogMessageContent();
	bool btnMonitorSqlMessageContent(TxBlobString *_sLcErrorText);
	void doTimerEvent5000();
	bool OnTheLogListResizeEvent(void* _pParam);
};
