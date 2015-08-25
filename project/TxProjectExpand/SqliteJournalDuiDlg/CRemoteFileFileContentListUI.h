#pragma once

#include <UIlib.h>
#include <time.h>
#include <vector>
#include <atlstr.h>
#include <list>
#include <functional>
#include "../common/TxBlobString.hpp"
#include "../include/CSqliteExpandDll.h"
#include "CMainFrameDlgBasic.hpp"

class CRemoteFileFileContentListUI
{
public :
	struct tagFileInfo
	{
		TxBlobString filename;
		long long filesize;
	};
	struct tagListElem: public DuiLib::CListContainerElementUI
	{
		DuiLib::CHorizontalLayoutUI *pLayout;
		DuiLib::CControlUI *a_lyt_ptr[32];
		tagFileInfo mFileInfo;
		int iCurPos;
		CRemoteFileFileContentListUI *const pParent;
		tagListElem(CRemoteFileFileContentListUI *_pParent);
		~tagListElem();
		void insertAppend(DuiLib::CControlUI *_ui);
		virtual void DoEvent(DuiLib::TEventUI& event);
	};
private:
	int iOldListWidth;
	DuiLib::CListUI *p_list;
	CMainFrameDlgBasic *pMainFrameDlgBasic;
	DWORD dw_itembkcolor,dw_itemselectedbkcolor,dw_itemhotbkcolor,dw_itemlinecolor;
	int iSelectIndex;
	TxCppPlatform::function<void()> mDblClickListItem;
public :
	CRemoteFileFileContentListUI(CMainFrameDlgBasic *_pMainFrameDlgBasic);
	bool OnTheRemoteFileListResizeEvent(void* _pParam);
	void initListUi(DuiLib::CListUI *_list,void *_userdata,void (*_dbl_click_cb)(void*));
	void refreshListOneCellWidth(DuiLib::CListUI *_list,tagListElem *_pLayout);
	void refreshListAllCellWidth(DuiLib::CListUI *_list);
	std::list<CRemoteFileFileContentListUI::tagFileInfo> getSelectListFileInfo();
	void addListElem(DuiLib::CListUI *_list,const char *_filename,long long _filesize,long long _time,EnumFilePermissionMode _permission);
};

