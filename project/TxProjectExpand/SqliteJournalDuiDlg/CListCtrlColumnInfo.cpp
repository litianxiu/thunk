#include "stdafx.h"
#include <algorithm>
#include <vector>
#include "../common/TxAssertWrap.hpp"
#include "CListCtrlColumnInfo.h"

void CListCtrlColumnInfo::_setSafeForceListCtrlColumnName(int _n,LPCTSTR _str,int _width)
{
	tagInfo info_item;
	info_item.bEnableCheck=FALSE;
	info_item.width=0;
	while((int)this->mListCtrlColumnInfo.size()<=_n)
		this->mListCtrlColumnInfo.push_back(info_item);
	info_item.bEnableCheck=TRUE;
	info_item.width=_width;
	info_item.name=_str;
	this->mListCtrlColumnInfo[_n]=info_item;
}

CListCtrlColumnInfo::CListCtrlColumnInfo()
{
	this->mListCtrlColumnInfo.clear();
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_src_id,_T("id"),80);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_src_file,_T("文件名"),125);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_src_func,_T("函数名"),125);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_src_line,_T("行号"),30);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_os_time,_T("系统时间"),115);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_soft_time,_T("软件时间"),115);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_svr_time,_T("服务器时间"),115);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_proc_name,_T("软件名"),100);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_msg_level,_T("消息等级"),60);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_msg_rand,_T("随机数"),80);
	this->_setSafeForceListCtrlColumnName(e_listctrl_column_msg_content,_T("消息文本"),60);
}

int CListCtrlColumnInfo::getColumnCount() const
{
	return (int)this->mListCtrlColumnInfo.size();
}

const CString& CListCtrlColumnInfo::getColumnName(int _index) const
{
	if(0<=_index&&_index<(int)this->mListCtrlColumnInfo.size())
		return this->mListCtrlColumnInfo[_index].name;
	DefAssertWrap(FALSE);
	return this->mListCtrlColumnInfo[0].name;
}

int CListCtrlColumnInfo::getColumnWidth(int _index) const
{
	int ret=0;
	if(0<=_index&&_index<(int)this->mListCtrlColumnInfo.size()&&this->mListCtrlColumnInfo[_index].bEnableCheck)
		ret=this->mListCtrlColumnInfo[_index].width;
	if(ret<0) ret=0;
	return ret;
}

bool CListCtrlColumnInfo::getColumnVisible(int _index) const
{
	if(0<=_index&&_index<(int)this->mListCtrlColumnInfo.size())
		return !!this->mListCtrlColumnInfo[_index].bEnableCheck;
	return false;
}

void CListCtrlColumnInfo::setColumnWidth(int _index,int _width)
{
	if(0<=_index&&_index<(int)this->mListCtrlColumnInfo.size()&&this->mListCtrlColumnInfo[_index].bEnableCheck&&_width>0)
		this->mListCtrlColumnInfo[_index].width=(_width>0?_width:0);
}

void CListCtrlColumnInfo::enableCheckShowItem(int _index,bool _bEnable)
{
	if(0<=_index&&_index<(int)this->mListCtrlColumnInfo.size())
		this->mListCtrlColumnInfo[_index].bEnableCheck=(!!_bEnable);
}

void CListCtrlColumnInfo::refreshTotalColumnWidth(int _g_width,int _sep_width)
{
	int www=_sep_width;
	for(int i=(int)this->mListCtrlColumnInfo.size();i--;)
	{
		if(i!=e_listctrl_column_msg_content&&this->mListCtrlColumnInfo[i].bEnableCheck)
			www+=this->mListCtrlColumnInfo[i].width+_sep_width;
	}
	this->setColumnWidth(e_listctrl_column_msg_content,_g_width-www-4);
}


CDuiLogViewListWrapExt::CDuiLogViewListWrapExt(CListCtrlColumnInfo *_pListCtrlColumnInfo)
	:pListCtrlColumnInfo(_pListCtrlColumnInfo)
{
}

void CDuiLogViewListWrapExt::sort_all(int _iItem)
{
	struct T_SOFT_ELEM
	{
		int index;
		int item;
		CDuiListWrapExt *pThis;
		BOOL bAscendingSign;
		BOOL bSoftLengthSign;
		static int comp(const CString &_str1,const CString &_str2,BOOL _bSoftLengthSign)
		{
			return _bSoftLengthSign?comp_len(_str1,_str2):comp_dic(_str1,_str2);
		}
		static int comp_len(const CString &_str1,const CString &_str2)
		{
			if(_str1.GetLength()<_str2.GetLength())
				return -1;
			else if(_str1.GetLength()>_str2.GetLength())
				return 1;
			return _str1.Compare(_str2);
		}
		static int comp_dic(const CString &_str1,const CString &_str2)
		{
			return _str1.Compare(_str2);
		}
		bool operator<(const T_SOFT_ELEM &_tem) const
		{
			int lc_item=this->item;
			tagListItem *pItem1=pThis->mDataInfo[this->index].get();
			tagListItem *pItem2=pThis->mDataInfo[_tem.index].get();
			if(pItem1==NULL) return true;
			else if(pItem2==NULL) return false;

			int val=comp(pItem1->mItemText[lc_item],pItem2->mItemText[lc_item],this->bSoftLengthSign);
			if(!this->bAscendingSign)
				val=-val;
			if(val<0) return true;
			else if(val>0) return false;

			return comp_len(pItem1->mItemText[CListCtrlColumnInfo::e_listctrl_column_src_id],
				pItem2->mItemText[CListCtrlColumnInfo::e_listctrl_column_src_id])<0;
		}
	};

	int iLcSoftLengthSign=-1;
	switch(_iItem)
	{
	case CListCtrlColumnInfo::e_listctrl_column_src_id:
	case CListCtrlColumnInfo::e_listctrl_column_src_line:
	case CListCtrlColumnInfo::e_listctrl_column_msg_level:
	case CListCtrlColumnInfo::e_listctrl_column_msg_rand:
		iLcSoftLengthSign=1;
		break;
	case CListCtrlColumnInfo::e_listctrl_column_src_file:
	case CListCtrlColumnInfo::e_listctrl_column_src_func:
	case CListCtrlColumnInfo::e_listctrl_column_os_time:
	case CListCtrlColumnInfo::e_listctrl_column_soft_time:
	case CListCtrlColumnInfo::e_listctrl_column_svr_time:
	case CListCtrlColumnInfo::e_listctrl_column_proc_name:
		iLcSoftLengthSign=0;
		break;
	default:
		DefAssertWrap(FALSE);
		break;
	}
	if(iLcSoftLengthSign>=0&&this->mDataInfo.size()>1)
	{
		int iLcAscendingSign=T_SOFT_ELEM::comp(this->mDataInfo.front()->mItemText[_iItem],this->mDataInfo.back()->mItemText[_iItem],iLcSoftLengthSign);
		if(iLcAscendingSign<=0) iLcAscendingSign=0;
		else iLcAscendingSign=1;
		std::vector<T_SOFT_ELEM> vv;
		vv.resize(this->mDataInfo.size());
		for(int i=(int)vv.size();i--;)
		{
			vv[i].bAscendingSign=(!!iLcAscendingSign);
			vv[i].bSoftLengthSign=(!!iLcSoftLengthSign);
			vv[i].index=i;
			vv[i].item=_iItem;
			vv[i].pThis=this;
		}
		std::sort(vv.begin(),vv.end());
		std::vector<std::vector<TS>> mLcRecordTs;
		mLcRecordTs.resize(vv.size());
		for(int i=(int)mLcRecordTs.size();i--;)
			mLcRecordTs[i]=this->mDataInfo[vv[i].index]->mItemText;
		for(int i=(int)this->mDataInfo.size();i--;)
			this->mDataInfo[i]->mItemText=mLcRecordTs[i];
	}
}

LPCTSTR CDuiLogViewListWrapExt::GetItemText(DuiLib::CControlUI* _pList, int _iItem, int _iSubItem)
{
	LPCTSTR ret=_T("");
	if(0<=_iItem&&_iItem<(int)this->mDataInfo.size()&&0<=_iSubItem&&_iSubItem<(int)this->mDataInfo[_iItem]->mItemText.size()
		&&_iSubItem<(int)pListCtrlColumnInfo->getColumnCount()&&pListCtrlColumnInfo->getColumnVisible(_iSubItem))
	{
		ret=this->mDataInfo[_iItem]->mItemText.at(_iSubItem);
	}
	return ret;
}
