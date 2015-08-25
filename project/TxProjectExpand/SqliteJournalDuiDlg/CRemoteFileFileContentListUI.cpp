#include "stdafx.h"
#include <set>
#include "CRemoteFileFileContentListUI.h"

CRemoteFileFileContentListUI::tagListElem::tagListElem(CRemoteFileFileContentListUI *_pParent):pParent(_pParent)
{
	this->iCurPos=0;
	memset(&this->a_lyt_ptr,0,sizeof(this->a_lyt_ptr));
	pLayout=new DuiLib::CHorizontalLayoutUI();
	this->Add(pLayout);
}

CRemoteFileFileContentListUI::tagListElem::~tagListElem()
{
	int dsf=0; dsf=1;
}

void CRemoteFileFileContentListUI::tagListElem::insertAppend(DuiLib::CControlUI *_ui)
{
	this->a_lyt_ptr[this->iCurPos++]=_ui;
	this->pLayout->Add(_ui);
}

void CRemoteFileFileContentListUI::tagListElem::DoEvent(DuiLib::TEventUI& event)
{
	DuiLib::TEventUI *pArg=&event;
	switch(pArg->Type)
	{
	case DuiLib::UIEVENT_MOUSEENTER:
		{
			if(this->GetBkColor()!=this->pParent->dw_itemselectedbkcolor)
				this->SetBkColor(this->pParent->dw_itemhotbkcolor);
		}
		break;
	case DuiLib::UIEVENT_MOUSELEAVE:
		{
			if(this->GetBkColor()!=this->pParent->dw_itemselectedbkcolor)
				this->SetBkColor(this->pParent->dw_itembkcolor);
		}
		break;
	case DuiLib::UIEVENT_DBLCLICK:
		if(this->pParent!=NULL&&this->pParent->mDblClickListItem)
			this->pParent->mDblClickListItem();
		break;
	case DuiLib::UIEVENT_BUTTONDOWN:
		do {
			const int iLcCur=this->pParent->p_list->GetItemIndex(this);
			if(iLcCur<0) break;
			int iLcLstSelect=this->pParent->iSelectIndex;
			this->pParent->iSelectIndex=iLcCur;
			this->SetBkColor(this->pParent->dw_itemselectedbkcolor);
			if(!(::GetKeyState(VK_CONTROL)&0xff00))
			{
				for(int i=this->pParent->p_list->GetCount();i--;)
				{
					tagListElem *pUiElem_2=dynamic_cast<tagListElem*>(this->pParent->p_list->GetItemAt(i));
					if(pUiElem_2!=NULL&&pUiElem_2!=this&&pUiElem_2->GetBkColor()!=this->pParent->dw_itembkcolor)
						pUiElem_2->SetBkColor(this->pParent->dw_itembkcolor);
				}
			}
			if(iLcLstSelect>=0&&!!(::GetKeyState(VK_SHIFT)&0xff00))
			{
				int pos1=(iLcCur<=iLcLstSelect?iLcCur:iLcLstSelect);
				int pos2=(iLcCur>=iLcLstSelect?iLcCur:iLcLstSelect);
				for(;pos1<=pos2;pos1++)
				{
					tagListElem *pUiElem_3=dynamic_cast<tagListElem*>(this->pParent->p_list->GetItemAt(pos1));
					if(pUiElem_3!=NULL&&pUiElem_3!=this&&pUiElem_3->GetBkColor()!=this->pParent->dw_itemselectedbkcolor)
					{
						pUiElem_3->SetBkColor(this->pParent->dw_itemselectedbkcolor);
					}
				}
			}
		} while(0);
		break;
	case DuiLib::UIEVENT_RBUTTONDOWN:
		if(this->GetBkColor()!=this->pParent->dw_itemselectedbkcolor)
		{
			for(int i=this->pParent->p_list->GetCount();i--;)
			{
				tagListElem *pUiElem=dynamic_cast<tagListElem*>(this->pParent->p_list->GetItemAt(i));
				if(pUiElem!=NULL&&pUiElem->GetBkColor()!=this->pParent->dw_itembkcolor)
					pUiElem->SetBkColor(this->pParent->dw_itembkcolor);
			}
			this->SetBkColor(this->pParent->dw_itemselectedbkcolor);
		}
		break;
	default:
		break;
	}
	return __super::DoEvent(event);
}

CRemoteFileFileContentListUI::CRemoteFileFileContentListUI(CMainFrameDlgBasic *_pMainFrameDlgBasic):pMainFrameDlgBasic(_pMainFrameDlgBasic)
{
	this->iOldListWidth=-1;
	this->iSelectIndex=-1;
	this->p_list=NULL;
}

bool CRemoteFileFileContentListUI::OnTheRemoteFileListResizeEvent(void* _pParam)
{
	DuiLib::CListUI *pLcList=dynamic_cast<DuiLib::CListUI*>((DuiLib::CControlUI*)_pParam);
	if(pLcList!=NULL)
	{
		this->refreshListAllCellWidth(pLcList);
		int iLcWidth=(int)CRect(pLcList->GetPos()).Width()-8;
		if(iLcWidth>0&&iLcWidth!=this->iOldListWidth)
		{
			this->iOldListWidth=iLcWidth;
			int iLcRecordWidth=0;
			for(int i=(int)pLcList->GetHeader()->GetCount();i--;)
			{
				DuiLib::CListHeaderItemUI *pLcHeadItem=dynamic_cast<DuiLib::CListHeaderItemUI*>(pLcList->GetHeader()->GetItemAt(i));
				if(i==0) pLcHeadItem->SetFixedWidth(iLcWidth-iLcRecordWidth-pLcHeadItem->GetSepWidth());
				else iLcRecordWidth+=(int)(pLcHeadItem->GetFixedWidth()+pLcHeadItem->GetSepWidth());
			}
		}
	}
	return true;
}


void CRemoteFileFileContentListUI::initListUi(DuiLib::CListUI *_list,void *_userdata,void (*_dbl_click_cb)(void*))
{
	struct tagElemInfo
	{
		CString name;
		int width;
		tagElemInfo(LPCTSTR _name,int _width):name(_name),width(_width) { }
	};
	std::vector<tagElemInfo> vLcColumnData;
	vLcColumnData.push_back(tagElemInfo(_T("文件名"),400));
	vLcColumnData.push_back(tagElemInfo(_T("大小"),180));
	vLcColumnData.push_back(tagElemInfo(_T("时间"),180));
	vLcColumnData.push_back(tagElemInfo(_T("权限"),100));

	for(int i=0;i<(int)vLcColumnData.size();i++)
	{
		DuiLib::CListHeaderItemUI *pLcHeadItem=new DuiLib::CListHeaderItemUI();
		pLcHeadItem->SetText(vLcColumnData[i].name);
		pLcHeadItem->SetSepImage(_T("list_header_sep.png"));
		pLcHeadItem->SetNormalImage(_T("list_head_normal.bmp"));
		pLcHeadItem->SetPushedImage(_T("list_head_down.bmp"));
		pLcHeadItem->SetSepWidth(1);
		pLcHeadItem->SetFixedWidth(vLcColumnData[i].width);
		_list->GetHeader()->Add(pLcHeadItem);
	}
	this->p_list=_list;
	this->dw_itembkcolor=_list->GetItemBkColor();
	this->dw_itemselectedbkcolor=_list->GetSelectedItemBkColor();
	this->dw_itemhotbkcolor=_list->GetHotItemBkColor();
	this->dw_itemlinecolor=_list->GetItemLineColor();
	_list->OnSize+=DuiLib::MakeDelegate(this, &CRemoteFileFileContentListUI::OnTheRemoteFileListResizeEvent);

	this->mDblClickListItem=TxCppPlatform::bind(_dbl_click_cb,_userdata);
}

void CRemoteFileFileContentListUI::refreshListOneCellWidth(DuiLib::CListUI *_list,tagListElem *_pLayout)
{
	int lc_rrr=0;
	for(int i=0;i<(int)_list->GetHeader()->GetCount();i++)
	{
		if(_pLayout->a_lyt_ptr[i]==NULL) break;
		CRect lc_rect(_list->GetHeader()->GetItemAt(i)->GetRelativePos());
		_pLayout->a_lyt_ptr[i]->SetFixedWidth(lc_rect.right-lc_rrr);
		lc_rrr=lc_rect.right;
	}
}

void CRemoteFileFileContentListUI::refreshListAllCellWidth(DuiLib::CListUI *_list)
{
	for(int y=_list->GetCount();y--;)
	{
		tagListElem *pLcLayout=dynamic_cast<tagListElem*>(_list->GetItemAt(y));
		assert(pLcLayout!=NULL);
		if(pLcLayout!=NULL)
			refreshListOneCellWidth(_list,pLcLayout);
	}
}

std::list<CRemoteFileFileContentListUI::tagFileInfo> CRemoteFileFileContentListUI::getSelectListFileInfo()
{
	std::list<tagFileInfo> ret;
	for(int i=0;i<(int)this->p_list->GetCount();i++)
	{
		tagListElem *pLcElem=dynamic_cast<tagListElem*>(this->p_list->GetItemAt(i));
		assert(pLcElem!=NULL);
		if(pLcElem!=NULL)
		{
			if(pLcElem->GetBkColor()==this->dw_itemselectedbkcolor)
				ret.push_back(pLcElem->mFileInfo);
		}
	}
	return ret;
}

void CRemoteFileFileContentListUI::addListElem(DuiLib::CListUI *_list,const char *_filename,long long _filesize,long long _time,EnumFilePermissionMode _permission)
{
	tagListElem *pLayout=new tagListElem(this);
	pLayout->SetBorderColor(this->dw_itemlinecolor);
	pLayout->SetBorderSize(CRect(0,0,0,2));
	pLayout->SetFixedHeight(26);
	{
		DuiLib::CHorizontalLayoutUI *pHorzLayout=new DuiLib::CHorizontalLayoutUI();
		{
			DuiLib::CContainerUI *plyt1=new DuiLib::CContainerUI();
			plyt1->SetFixedWidth(30);
			plyt1->SetInset(CRect(1,1,1,1));
			{
				//DuiLib::CHorizontalLayoutUI *ph1=new DuiLib::CHorizontalLayoutUI();
				//ph1->SetFixedHeight(4);
				//pVlyt1->Add(ph1);
				DuiLib::CLabelUI *pLabel1=new DuiLib::CLabelUI();
				pLabel1->SetBkImage(_filesize<0?"attr_dir_icon.png":"attr_file_icon.png");
				pLabel1->SetFloat(false);
				plyt1->Add(pLabel1);
				//pVlyt1->Add(pLabel1);
				//DuiLib::CHorizontalLayoutUI *ph2=new DuiLib::CHorizontalLayoutUI();
				//ph2->SetFixedHeight(4);
				//pVlyt1->Add(ph2);
			}
			pHorzLayout->Add(plyt1);
		}
		{
			DuiLib::CContainerUI *plyt1_2=new DuiLib::CContainerUI();
			plyt1_2->SetFixedWidth(3);
			pHorzLayout->Add(plyt1_2);
		}
		{
			DuiLib::CLabelUI *pLabel3=new DuiLib::CLabelUI();
			pLabel3->SetText(_filename);
			pHorzLayout->Add(pLabel3);
			pLayout->mFileInfo.filename.assign(_filename);
		}
		pLayout->insertAppend(pHorzLayout);
	}
	{
		DuiLib::CLabelUI *pLabelSize=new DuiLib::CLabelUI();
		TxBlobString lcText;
		pLayout->mFileInfo.filesize=_filesize;
		if(_filesize>=0)
		{
			TxBlobString tttxt(TxBlobString::fromDatum(_filesize));
			for(int i=0;i<tttxt.length();i++)
			{
				if((tttxt.length()-i)%3==0&&lcText.length()>0)
					lcText.append(",");
				lcText.append(tttxt.c_str()+i,1);
			}
		}
		else
		{
			lcText.assign("未知");
		}
		pLabelSize->SetText(lcText.c_str());
		pLabelSize->SetTextStyle(pLabelSize->GetTextStyle()|DT_CENTER);
		pLayout->insertAppend(pLabelSize);
	}
	{
		DuiLib::CLabelUI *pLabelTime=new DuiLib::CLabelUI();
		struct tm lcCurTime={0};
		time_t lcTime_t=(time_t)_time;
		TxBlobString str;
		if(::localtime_s(&lcCurTime,&lcTime_t)==(errno_t)0)
		{
			str.format("%04d/%02d/%02d %02d:%02d:%02d",
				(int)(lcCurTime.tm_year+1900),
				(int)(lcCurTime.tm_mon+1),
				(int)(lcCurTime.tm_mday+0),
				(int)(lcCurTime.tm_hour+0),
				(int)(lcCurTime.tm_min+0),
				(int)(lcCurTime.tm_sec+0));
		}
		else
		{
			str.assign("未知");
		}
		pLabelTime->SetTextStyle(pLabelTime->GetTextStyle()|DT_CENTER);
		pLabelTime->SetText(str.c_str());
		pLayout->insertAppend(pLabelTime);
	}
	{
		DuiLib::CLabelUI *pLabePrms=new DuiLib::CLabelUI();
		TxBlobString str;
		str.append((_permission&eFilePermissionMode_Read)?"r":"-");
		str.append((_permission&eFilePermissionMode_Write)?"w":"-");
		str.append((_permission&eFilePermissionMode_Exec)?"x":"-");
		pLabePrms->SetTextStyle(pLabePrms->GetTextStyle()|DT_CENTER);
		pLabePrms->SetText(str.c_str());
		pLayout->insertAppend(pLabePrms);
	}
	_list->Add(pLayout);
	refreshListOneCellWidth(_list,pLayout);
}


