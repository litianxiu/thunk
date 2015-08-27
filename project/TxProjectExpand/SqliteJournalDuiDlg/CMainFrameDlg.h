#pragma once

#include "CSqliteJournalDuiDlg.h"
#include "CBrowseRemoteDuiDlg.h"
#include "CLogSettingWnd.h"
#include "CPlayVideoDuiDlg.h"
#include "CMainFrameDlgBasic.hpp"

class CMainFrameDlg: public CMainFrameDlgBasic, public DuiLib::CControlUI
{
private:
	DuiLib::CButtonUI* m_pCloseBtn;
	DuiLib::CButtonUI* m_pMaxBtn;
	DuiLib::CButtonUI* m_pRestoreBtn;
	DuiLib::CButtonUI* m_pMinBtn;
	DuiLib::CButtonUI* m_pSettingBtn;

	DuiLib::CHorizontalLayoutUI *pMainLayout;
	DuiLib::COptionUI *p_Option_viewLog;
	DuiLib::COptionUI *p_Option_remotFile;
	DuiLib::COptionUI *p_Option_playVideo;
	BOOL bInitControlUI;
private:
	CBrowseRemoteDuiDlg m_BrowseRemoteDuiDlg;
	CSqliteJournalDuiDlg m_SqliteJournalDuiDlg;
	CPlayVideoDuiDlg m_PlayVideoDuiDlg;
	CLogSettingWnd m_LogSettingWnd;
	enum {
		e_menu_option_null=0,
		e_menu_option_setting,
		e_menu_option_task,
	};
public :
	CMainFrameDlg(const tagConfigSettingData &_mConfigSettingData);
	~CMainFrameDlg();
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	DuiLib::UILIB_RESOURCETYPE GetResourceType() const;
	bool OnLayoutWndSizeEvent(void* _pParam);
	void menuOptionClickEvent(int _id);
	virtual void InitWindow();
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled);
	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam);
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	virtual void vfSelectTabWidget(int _iIndex);
	CPlayVideoDuiDlg* getPlayVideoDuiDlgPtr() { return &this->m_PlayVideoDuiDlg; }
};

