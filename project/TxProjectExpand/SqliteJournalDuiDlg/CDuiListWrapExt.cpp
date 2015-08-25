#include "StdAfx.h"
#include <algorithm>
#include "../common/TxAssertWrap.hpp"
#include "CDuiListWrapExt.h"


CDuiListWrapExt::CDuiListWrapExt()
{
}

CDuiListWrapExt::~CDuiListWrapExt()
{
	this->removeAll();
}

void CDuiListWrapExt::erase(int _index)
{
	if(_index<(int)this->mDataInfo.size())
	{
		this->mDataInfo.erase(this->mDataInfo.begin()+_index);
	}
	else
	{
		DefAssertWrap(FALSE);
	}
}

void CDuiListWrapExt::removeAll()
{
	this->mDataInfo.clear();
}

TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> CDuiListWrapExt::insertElem(int _index,DuiLib::CListTextElementUI *_ui)
{
	TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> ret(new tagListItem(_ui));
	if(_index<0)
		this->mDataInfo.insert(this->mDataInfo.begin(),ret);
	else if(_index>=(int)this->mDataInfo.size())
		this->mDataInfo.insert(this->mDataInfo.end(),ret);
	else
		this->mDataInfo.insert(this->mDataInfo.begin()+_index,ret);
	return ret;
}

TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> CDuiListWrapExt::insertFront(DuiLib::CListTextElementUI *_ui)
{
	return this->insertElem(0,_ui);
}

TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> CDuiListWrapExt::insertBack(DuiLib::CListTextElementUI *_ui)
{
	return this->insertElem((int)this->mDataInfo.size(),_ui);
}

int CDuiListWrapExt::getItemCount() const
{
	return (int)this->mDataInfo.size();
}

LPCTSTR CDuiListWrapExt::GetItemText(DuiLib::CControlUI* _pList, int _iItem, int _iSubItem)
{
	LPCTSTR ret=_T("");
	if(0<=_iItem&&_iItem<(int)this->mDataInfo.size()&&0<=_iSubItem&&_iSubItem<(int)this->mDataInfo[_iItem]->mItemText.size())
	{
		ret=this->mDataInfo[_iItem]->mItemText.at(_iSubItem);
	}
	return ret;
}

TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> CDuiListWrapExt::at(int _iItem)
{
	TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> ret;
	if(0<=_iItem&&_iItem<(int)this->mDataInfo.size())
		ret=this->mDataInfo[_iItem];
	return ret;
}
