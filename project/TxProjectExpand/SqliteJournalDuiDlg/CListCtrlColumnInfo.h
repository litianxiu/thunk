#pragma once

#include <vector>
#include <atlstr.h>
#include <utility>
#include <Windows.h>
#include "CDuiListWrapExt.h"

class CListCtrlColumnInfo
{
public :
	enum {
		e_listctrl_column_start_temp_=-1,
		e_listctrl_column_src_id,
		e_listctrl_column_src_file,
		e_listctrl_column_src_func,
		e_listctrl_column_src_line,
		e_listctrl_column_os_time,
		e_listctrl_column_soft_time,
		e_listctrl_column_svr_time,
		e_listctrl_column_proc_name,
		e_listctrl_column_msg_level,
		e_listctrl_column_msg_rand,
		e_listctrl_column_msg_content,
	};
private:
	struct tagInfo
	{
		CString name;
		int width;
		BOOL bEnableCheck;
	};
	std::vector<tagInfo> mListCtrlColumnInfo;
private:
	void _setSafeForceListCtrlColumnName(int _n,LPCTSTR _str,int _width);
public:
	CListCtrlColumnInfo();
	int getColumnCount() const;
	const CString& getColumnName(int _index) const;
	int getColumnWidth(int _index) const;
	bool getColumnVisible(int _index) const;
	void setColumnWidth(int _index,int _width);
	void enableCheckShowItem(int _index,bool _bEnable);
	void refreshTotalColumnWidth(int _g_width,int _sep_width);
};

class CDuiLogViewListWrapExt: public CDuiListWrapExt
{
private:
	CListCtrlColumnInfo *const pListCtrlColumnInfo;
public:
	CDuiLogViewListWrapExt(CListCtrlColumnInfo *_pListCtrlColumnInfo);
	void sort_all(int _iItem);
	virtual LPCTSTR GetItemText(DuiLib::CControlUI* _pList, int _iItem, int _iSubItem);
};
