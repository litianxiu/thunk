#pragma once
#include <UIlib.h>
#include <memory>
#include <vector>
#include <utility>
#include <atlstr.h>
#include "../common/TxGlobalConfig.h"

class CDuiListWrapExt: public DuiLib::IListCallbackUI
{
public :
	typedef CString TS;
	struct tagListItem
	{
		DuiLib::CListTextElementUI *const ui;
		std::vector<TS> mItemText;
		tagListItem(DuiLib::CListTextElementUI *_ui):ui(_ui)
		{
			//DuiLib::CListContainerElementUI::Add(
		}
	};
	std::vector<TxCppPlatform::shared_ptr<tagListItem>> mDataInfo;
public :
	CDuiListWrapExt();
	~CDuiListWrapExt();
	void erase(int _index);
	void removeAll();
	TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> insertElem(int _index,DuiLib::CListTextElementUI *_ui);
	TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> insertFront(DuiLib::CListTextElementUI *_ui);
	TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> insertBack(DuiLib::CListTextElementUI *_ui);
	int getItemCount() const;
	virtual LPCTSTR GetItemText(DuiLib::CControlUI* _pList, int _iItem, int _iSubItem);
	TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> at(int _iItem);
};
