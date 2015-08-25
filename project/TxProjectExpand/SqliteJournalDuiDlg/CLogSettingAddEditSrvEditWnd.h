#pragma once

#include "../common/TxBlobString.hpp"
#include "CMainFrameDlgBasic.hpp"

class CLogSettingAddEditSrvEditWnd: public  DuiLib::WindowImplBase
{
public :
	struct tagSettingSrvInfo
	{
		TxBlobString strFileName;
		int port;
		bool bIsLog;
	} ;
private:
	BOOL bInitControlUI;
	DuiLib::CEditUI *pEdit_svr_port;
	DuiLib::CEditUI *pEdit_db_filename;
	DuiLib::COptionUI *pOption_is_log;
	DuiLib::CButtonUI *pbtn_0;
	DuiLib::CButtonUI *pbtn_1;
	int result;
private:
	CLogSettingAddEditSrvEditWnd();
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	DuiLib::UILIB_RESOURCETYPE GetResourceType() const;
	virtual void InitWindow();
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled);
public :
	static int run(tagSettingSrvInfo *_data,HWND hwndParent,LPCTSTR _btn0,LPCTSTR _btn1);
};

