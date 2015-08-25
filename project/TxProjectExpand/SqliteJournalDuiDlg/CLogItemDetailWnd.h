#pragma once

#include "StdAfx.h"
#include "CListCtrlColumnInfo.h"
#include "CDuiListWrapExt.h"
#include "CMainFrameDlgBasic.hpp"

class CLogItemDetailWnd: public  DuiLib::WindowImplBase
{
private:
	BOOL bInitControlUI;
	DuiLib::CEditUI *m_pEdit_softname;
	DuiLib::CEditUI *m_pEdit_filename;
	DuiLib::CEditUI *m_pEdit_funcname;
	DuiLib::CEditUI *m_pEdit_linenumber;
	DuiLib::CEditUI *m_pEdit_ostime;
	DuiLib::CEditUI *m_pEdit_softtime;
	DuiLib::CEditUI *m_pEdit_svrtime;
	DuiLib::CEditUI *m_pEdit_msglevel;
	DuiLib::CEditUI *m_pEdit_msgrand;
	DuiLib::CButtonUI* m_pCloseBtn;
	DuiLib::CButtonUI* m_pMaxBtn;
	DuiLib::CButtonUI* m_pRestoreBtn;
	DuiLib::CButtonUI* m_pMinBtn;
	DuiLib::CRichEditUI *m_pRichEdit_msgcontent;
public :
	CLogItemDetailWnd();
	~CLogItemDetailWnd();
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	virtual DuiLib::UILIB_RESOURCETYPE GetResourceType() const;
	virtual void InitWindow();
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	static DuiLib::CDuiString transGetStandTimeString(const CDuiListWrapExt::tagListItem *_data,int _col_index);
	static DuiLib::CDuiString transGetStandLevelString(const CDuiListWrapExt::tagListItem *_data,int _col_index);
	void set(const CDuiListWrapExt::tagListItem *_data);
};
