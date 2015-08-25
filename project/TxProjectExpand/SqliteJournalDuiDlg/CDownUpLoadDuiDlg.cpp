#include "StdAfx.h"
#include "CTaskQueueDownloadClientWrapUi.h"
#include "CDownUpLoadDuiDlg.h"

CDownUpLoadDuiDlg::CDownUpLoadDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic)
	:pMainFrameDlgBasic(_pMainFrameDlgBasic)
{
	this->bInitControlUI=FALSE;
}

CDownUpLoadDuiDlg::~CDownUpLoadDuiDlg()
{
	this->bInitControlUI=FALSE;
	this->removeAllTaskQueue();
}

DuiLib::CDuiString CDownUpLoadDuiDlg::GetSkinFolder()
{
	return DefDuiSkin;
}

DuiLib::CDuiString CDownUpLoadDuiDlg::GetSkinFile()
{
	return _T("down_up_load_dui.xml");
}

UINT CDownUpLoadDuiDlg::GetClassStyle() const
{
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
}

DuiLib::UILIB_RESOURCETYPE CDownUpLoadDuiDlg::GetResourceType() const
{
	return DuiLib::UILIB_FILE;
}

LPCTSTR CDownUpLoadDuiDlg::GetWindowClassName() const
{
	return _T("CDownUpLoadDuiDlg");
}

void CDownUpLoadDuiDlg::InitWindow()
{
	DefAssertTextWrap(this->bInitControlUI==FALSE,"");
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_minwnd,_T("btn_minwnd"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_maxwnd,_T("btn_maxwnd"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_restorewnd,_T("btn_restorewnd"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_closewnd,_T("btn_closewnd"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_tab_main_wnd,_T("tab_main_wnd"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_task_dofailure,_T("Option_task_dofailure"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_task_dofinish,_T("Option_task_dofinish"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_task_doing,_T("Option_task_doing"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_VerticalLayout_task_doing,_T("VerticalLayout_task_doing"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_VerticalLayout_task_dofinish,_T("VerticalLayout_task_dofinish"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_VerticalLayout_task_dofailure,_T("VerticalLayout_task_dofailure"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_list_down_up_load_doing,_T("list_down_up_load_doing"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_list_down_up_load_finish,_T("list_down_up_load_finish"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_list_down_up_load_failure,_T("list_down_up_load_failure"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_select_all_doing,_T("Option_select_all_doing"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_select_all_failure,_T("Option_select_all_failure"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Option_select_all_finish,_T("Option_select_all_finish"));

	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_clear_all_pause_doing,_T("btn_clear_all_pause_doing"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_clear_all_doing,_T("btn_clear_all_doing"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_delete_all_file_finish,_T("btn_delete_all_file_finish"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_clear_all_finish,_T("btn_clear_all_finish"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_restart_all_file_pause,_T("btn_restart_all_file_pause"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_clear_all_failure,_T("btn_clear_all_failure"));

	this->bInitControlUI=TRUE;

	this->p_list_down_up_load_doing->GetHeader()->SetVisible(false);
	this->p_list_down_up_load_finish->GetHeader()->SetVisible(false);
	this->p_list_down_up_load_failure->GetHeader()->SetVisible(false);
	this->m_timer_span_refresh_list_item.reset(0);
	//this->pMainFrameDlgBasic->installTimerTask(500,TxCppPlatform::bind(&CDownUpLoadDuiDlg::onTimerMonitorTaskQueue,this));
}

LRESULT CDownUpLoadDuiDlg::HandleMessage(UINT _uMsg,WPARAM _wParam,LPARAM _lParam)
{
	switch(_uMsg)
	{
	case WM_TIMER:
		break;
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
		this->bInitControlUI=FALSE;
		break;
	default:
		break;
	}
	return __super::HandleMessage(_uMsg,_wParam,_lParam);
}

LRESULT CDownUpLoadDuiDlg::MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled)
{
	switch(_uMsg)
	{
	case WM_KEYDOWN:
		if(_wParam==VK_ESCAPE||_wParam==VK_CANCEL)
		{
			_bHandled=true;
			return S_OK;
		}
		break;
	default:
		break;
	}
	return __super::MessageHandler(_uMsg,_wParam,_lParam,_bHandled);
}

void CDownUpLoadDuiDlg::Notify(DuiLib::TNotifyUI& _msg)
{
	if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0) 
	{
		if(_msg.pSender==this->p_btn_minwnd)
		{
			this->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
		}
		else if(_msg.pSender==this->p_btn_maxwnd)
		{
			this->SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			this->p_btn_maxwnd->SetVisible(false);
			this->p_btn_restorewnd->SetVisible(true);
		}
		else if(_msg.pSender==this->p_btn_restorewnd)
		{
			this->SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
			this->p_btn_maxwnd->SetVisible(true);
			this->p_btn_restorewnd->SetVisible(false);
		}
		else if(_msg.pSender==this->p_btn_closewnd)
		{
			this->ShowWindow(false);
		}
		else if(_msg.pSender==this->p_btn_clear_all_pause_doing)
		{
			this->deal_btn_clear_all_pause_doing();
		}
		else if(_msg.pSender==this->p_btn_clear_all_doing)
		{
			this->deal_btn_clear_all_item_(tagTaskThreadCommonSessionBasic::e_ui_doing,false);
		}
		else if(_msg.pSender==this->p_btn_delete_all_file_finish)
		{
			this->deal_btn_clear_all_item_(tagTaskThreadCommonSessionBasic::e_ui_finish,true);
		}
		else if(_msg.pSender==this->p_btn_clear_all_finish)
		{
			this->deal_btn_clear_all_item_(tagTaskThreadCommonSessionBasic::e_ui_finish,false);
		}
		else if(_msg.pSender==this->p_btn_restart_all_file_pause)
		{
			this->deal_btn_restart_all_file_pause();
		}
		else if(_msg.pSender==this->p_btn_clear_all_failure)
		{
			this->deal_btn_clear_all_item_(tagTaskThreadCommonSessionBasic::e_ui_pause,true);
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_SELECTCHANGED)==0)
	{
		if(_msg.pSender==this->p_Option_task_doing)
		{
			this->p_tab_main_wnd->SelectItem(this->p_VerticalLayout_task_doing);
		}
		else if(_msg.pSender==this->p_Option_task_dofinish)
		{
			this->p_tab_main_wnd->SelectItem(this->p_VerticalLayout_task_dofinish);
		}
		else if(_msg.pSender==this->p_Option_task_dofailure)
		{
			this->p_tab_main_wnd->SelectItem(this->p_VerticalLayout_task_dofailure);
		}
		else if(_msg.pSender==this->p_Option_select_all_doing)
		{
			gListSelectAllTask(this->p_list_down_up_load_doing,this->p_Option_select_all_doing->IsSelected());
		}
		else if(_msg.pSender==this->p_Option_select_all_finish)
		{
			gListSelectAllTask(this->p_list_down_up_load_finish,this->p_Option_select_all_finish->IsSelected());
		}
		else if(_msg.pSender==this->p_Option_select_all_failure)
		{
			gListSelectAllTask(this->p_list_down_up_load_failure,this->p_Option_select_all_failure->IsSelected());
		}
	}
	return __super::Notify(_msg);
}

void CDownUpLoadDuiDlg::addDownLoadTask(const char *_ip,int _port,const char *_sLocalUri,const char *_sRemoteUri)
{
	TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> spLcObject(new CTaskDownloadThreadSession(this,_ip,_port,_sRemoteUri,_sLocalUri));
	spLcObject->createUi(tagTaskThreadCommonSessionBasic::e_ui_doing);
	this->p_list_down_up_load_doing->Add(spLcObject->pUi_doing);
	this->doWakeUpTaskThread();
	//CTaskQueueDownloadClientWrapUi *pLcUi=new CTaskQueueDownloadClientWrapUi(_ip,_port,this,_sLocalUri,_sRemoteUri);
	//pLcUi->setDownUpLoadContent(0,-1,TxBlobString(_sLocalUri).splitSegment("\\","/").back().c_str());
	//this->mListUpDownLoadAvailElemPtrSet.insert(pLcUi);
	//this->p_list_down_up_load_doing->Add(pLcUi);
	////if(this->p_list_down_up_load->GetCount()<e_meanwhile_max_task_queue_count)
	////{
	////	this->awakeOneQueueThreadTask();
	////}
}

void CDownUpLoadDuiDlg::doWakeUpTaskThread()
{
	std::vector<TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic>> v_lc_elem;
	for(int i=0;i<3&&i<this->p_list_down_up_load_doing->GetCount();i++)
	{
		tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase *pLcElem
			=dynamic_cast<tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase*>(this->p_list_down_up_load_doing->GetItemAt(i));
		if(pLcElem->spObject&&!pLcElem->spObject->bUiStartThread)
		{
			v_lc_elem.push_back(pLcElem->spObject);
		}
	}
	for(int i=(int)v_lc_elem.size();i--;)
	{
		v_lc_elem[i]->start();
		v_lc_elem[i]->bUiStartThread=TRUE;
	}
}

void CDownUpLoadDuiDlg::gListSelectAllTask(DuiLib::CListUI *_p_list,bool _b_select)
{
	for(int i=(int)_p_list->GetCount();i--;)
	{
		DuiLib::CControlUI * p1=_p_list->GetItemAt(i);
		assert(p1!=NULL);
		if(p1==NULL) continue;
		tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase *p2=dynamic_cast<tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase*>(p1);
		assert(p2!=NULL);
		if(p2==NULL) continue;
		p2->vf_select(_b_select);
	}
}

void CDownUpLoadDuiDlg::deal_btn_clear_all_item_(tagTaskThreadCommonSessionBasic::E_UI _e,bool _b_delfile)
{
	DuiLib::CListUI *p_lc_list_task=NULL;
	size_t lc_elem_offset=0;
	switch(_e)
	{
	case tagTaskThreadCommonSessionBasic::e_ui_doing:
		p_lc_list_task=this->p_list_down_up_load_doing;
		lc_elem_offset=offsetof(tagTaskThreadCommonSessionBasic,pUi_doing);
		break;
	case tagTaskThreadCommonSessionBasic::e_ui_finish:
		p_lc_list_task=this->p_list_down_up_load_finish;
		lc_elem_offset=offsetof(tagTaskThreadCommonSessionBasic,pUi_finish);
		break;
	case tagTaskThreadCommonSessionBasic::e_ui_pause:
		p_lc_list_task=this->p_list_down_up_load_failure;
		lc_elem_offset=offsetof(tagTaskThreadCommonSessionBasic,pUi_pause);
		break;
	default:
		DefAssertWrap(FALSE);
		break;
	}
	if(p_lc_list_task==NULL) return ;
	for(int i=(int)p_lc_list_task->GetCount();i--;)
	{
		DuiLib::CControlUI * p1_=p_lc_list_task->GetItemAt(i);
		assert(p1_!=NULL);
		if(p1_==NULL) continue;
		tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase *p2=dynamic_cast<tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase*>(p1_);
		assert(p2!=NULL);
		if(p2==NULL) continue;
		TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> spLcSession=p2->spObject;
		assert(!!spLcSession);
		if(!spLcSession) continue;
		spLcSession->stop(false);
		spLcSession->bUiStartThread=FALSE;
		tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase **pp_ui
			=(tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase**)(((unsigned char*)spLcSession.get())+lc_elem_offset);
		if((*pp_ui)==NULL) continue;
		if(!p2->vf_is_select()) continue;
		p2->spObject.reset();
		p_lc_list_task->Remove(*pp_ui);
		(*pp_ui)=NULL;
		spLcSession->assert_ui_empty();
		if(_b_delfile)
		{
			spLcSession->deletefile();
		}
	}
}

void CDownUpLoadDuiDlg::deal_btn_clear_all_pause_doing()
{
	for(int i=(int)this->p_list_down_up_load_doing->GetCount();i--;)
	{
		DuiLib::CControlUI * p1_=this->p_list_down_up_load_doing->GetItemAt(i);
		assert(p1_!=NULL);
		if(p1_==NULL) continue;
		tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase *p2=dynamic_cast<tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase*>(p1_);
		assert(p2!=NULL);
		if(p2==NULL) continue;
		if(!p2->vf_is_select()) continue;
		TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> spLcSession=p2->spObject;
		p2->spObject.reset();
		assert(!!spLcSession);
		if(!spLcSession) continue;
		spLcSession->stop(false);
		spLcSession->bUiStartThread=FALSE;
		assert(spLcSession->pUi_doing==p2);
		this->p_list_down_up_load_doing->Remove(spLcSession->pUi_doing);
		spLcSession->pUi_doing=NULL;
		assert(spLcSession->pUi_doing==NULL);
		spLcSession->createUi(tagTaskThreadCommonSessionBasic::e_ui_pause);
		this->p_list_down_up_load_failure->Add(spLcSession->pUi_pause);
	}
}

void CDownUpLoadDuiDlg::deal_btn_restart_all_file_pause()
{
	for(int i=(int)this->p_list_down_up_load_failure->GetCount();i--;)
	{
		DuiLib::CControlUI * p1_=this->p_list_down_up_load_failure->GetItemAt(i);
		assert(p1_!=NULL);
		if(p1_==NULL) continue;
		tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase *p2=dynamic_cast<tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase*>(p1_);
		assert(p2!=NULL);
		if(p2==NULL) continue;
		if(!p2->vf_is_select()) continue;
		TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> spLcSession=p2->spObject;
		p2->spObject.reset();
		assert(!!spLcSession);
		if(!spLcSession) continue;
		assert(spLcSession->pUi_pause==p2);
		this->p_list_down_up_load_failure->Remove(spLcSession->pUi_pause);
		spLcSession->pUi_pause=NULL;
		spLcSession->createUi(tagTaskThreadCommonSessionBasic::e_ui_doing);
		this->p_list_down_up_load_doing->Add(spLcSession->pUi_doing);
		spLcSession->bUiStartThread=TRUE;
		spLcSession->start();
	}
}

void CDownUpLoadDuiDlg::removeAllTaskQueue()
{
	//DefAssertWrap(this->mListUpDownLoadAvailElemPtrSet.size()==0);
	//std::set<tagDuiListTaskDoingQueueElemBase*>::iterator iter=this->mListUpDownLoadAvailElemPtrSet.begin();
	//std::set<tagDuiListTaskDoingQueueElemBase*>::iterator iter_end=this->mListUpDownLoadAvailElemPtrSet.end();
	//for(;iter!=iter_end;iter++)
	//{
	//	(*iter)->stopTaskThread();
	//}
	//this->mListUpDownLoadAvailElemPtrSet.clear();
	//this->p_list_down_up_load_doing->RemoveAll();
}

//bool CDownUpLoadDuiDlg::onTimerMonitorTaskQueue()
//{
//	if(this->bInitControlUI)
//	{
//		bool bLcTimer_refresh_ui=this->m_timer_span_refresh_list_item.dtctCurTimeResetSpanMsec((long long)(1.5*1000),false);
//		std::vector<tagDuiListTaskDoingQueueElemBase*> v_lc_elem;
//		int iii=(int)this->p_list_down_up_load_doing->GetCount();
//		enum { e_lc_meanwhile_max_task_queue_count=3, };
//		if(iii>e_lc_meanwhile_max_task_queue_count)
//			iii=e_lc_meanwhile_max_task_queue_count;
//		while(iii--)
//		{
//			tagDuiListTaskDoingQueueElemBase *pLcElem=dynamic_cast<tagDuiListTaskDoingQueueElemBase*>(this->p_list_down_up_load_doing->GetItemAt(iii));
//			assert(pLcElem!=NULL);
//			if(pLcElem!=NULL)
//				v_lc_elem.push_back(pLcElem);
//		}
//		for(int xy=0;xy<(int)v_lc_elem.size();xy++)
//		{
//			tagDuiListTaskDoingQueueElemBase::e_status lc_e=v_lc_elem[xy]->getRunStatus();
//			switch(lc_e)
//			{
//			case tagDuiListTaskDoingQueueElemBase::e_nostart:
//				v_lc_elem[xy]->startTaskThread();
//				break;
//			case tagDuiListTaskDoingQueueElemBase::e_running:
//				if(bLcTimer_refresh_ui)
//					v_lc_elem[xy]->onTimerRefresh();
//				break;
//			case tagDuiListTaskDoingQueueElemBase::e_finish:
//				this->mListUpDownLoadAvailElemPtrSet.erase(v_lc_elem[xy]);
//				this->p_list_down_up_load_doing->Remove(v_lc_elem[xy]);
//				if(this->mListUpDownLoadAvailElemPtrSet.size()==0)
//					this->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(_T("œ¬‘ÿÕÍ≥…£°"));
//				break;
//			default:
//				assert(0);
//				break;
//			}
//		}
//		return true;
//	}
//	return false;
//}

void CDownUpLoadDuiDlg::onMyTaskThreadEvent(tagTaskThreadCommonSessionBasic::E_CMD _eStatus,TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> _spThreadSession,const void *_data)
{
	struct
	{
		TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> sp;
		tagTaskThreadCommonSessionBasic::E_CMD eStatus;
		CDownUpLoadDuiDlg *pThis;
		std::string sDelFileName;
		void operator()()
		{
			tagTaskThreadCommonSessionBasic *pLcSesson=this->sp.get();
			if(!this->sp||pLcSesson==NULL) return ;
			switch(this->eStatus)
			{
			case tagTaskThreadCommonSessionBasic::e_cmd_task_failure:
				{
					pLcSesson->stop(false);
					if(pLcSesson->pUi_doing!=NULL)
					{
						this->pThis->p_list_down_up_load_doing->Remove(pLcSesson->pUi_doing);
						pLcSesson->pUi_doing=NULL;
						pLcSesson->createUi(tagTaskThreadCommonSessionBasic::e_ui_pause);
						this->pThis->p_list_down_up_load_failure->Add(pLcSesson->pUi_pause);
						pLcSesson->bUiStartThread=FALSE;
					}
				}
				break;
			case tagTaskThreadCommonSessionBasic::c_cmd_click_pause:
				break;
			case tagTaskThreadCommonSessionBasic::e_cmd_task_finish:
				{
					pLcSesson->stop(true);
					if(pLcSesson->pUi_doing!=NULL)
					{
						this->pThis->p_list_down_up_load_doing->Remove(pLcSesson->pUi_doing);
						pLcSesson->pUi_doing=NULL;
					}
					else assert(0);
					pLcSesson->createUi(tagTaskThreadCommonSessionBasic::e_ui_finish);
					this->pThis->p_list_down_up_load_finish->Add(pLcSesson->pUi_finish);
					pLcSesson->bUiStartThread=FALSE;
				}
				break;
			case tagTaskThreadCommonSessionBasic::c_cmd_task_refreshui:
				if(pLcSesson->pUi_doing!=NULL)
					pLcSesson->pUi_doing->onTimerRefresh();
				break;
			case tagTaskThreadCommonSessionBasic::c_cmd_click_restart:
				{
					pLcSesson->stop(false);
					if(pLcSesson->pUi_pause!=NULL)
					{
						this->pThis->p_list_down_up_load_failure->Remove(pLcSesson->pUi_pause);
						pLcSesson->pUi_pause=NULL;
					}
					else assert(0);
					pLcSesson->createUi(tagTaskThreadCommonSessionBasic::e_ui_doing);
					this->pThis->p_list_down_up_load_doing->Add(pLcSesson->pUi_doing);
					pLcSesson->start();
					pLcSesson->bUiStartThread=TRUE;
				}
				break;
			case tagTaskThreadCommonSessionBasic::c_cmd_click_delfile:
			case tagTaskThreadCommonSessionBasic::c_cmd_click_clearitem:
				{
					pLcSesson->stop(false);
					if(pLcSesson->pUi_doing!=NULL)
					{
						this->pThis->p_list_down_up_load_doing->Remove(pLcSesson->pUi_doing);
						pLcSesson->pUi_doing=NULL;
					}
					if(pLcSesson->pUi_pause!=NULL)
					{
						this->pThis->p_list_down_up_load_failure->Remove(pLcSesson->pUi_pause);
						pLcSesson->pUi_pause=NULL;
					}
					if(pLcSesson->pUi_finish!=NULL)
					{
						this->pThis->p_list_down_up_load_finish->Remove(pLcSesson->pUi_finish);
						pLcSesson->pUi_finish=NULL;
					}
					pLcSesson->bUiStartThread=FALSE;
				}
				break;
			default:
				DefAssertWrap(false);
				break;
			}
			if(this->sDelFileName.length()>0)
			{
				::DeleteFileA(this->sDelFileName.c_str());
			}
			this->pThis->doWakeUpTaskThread();
		}
	} mLcFunc;
	mLcFunc.sp=_spThreadSession;
	mLcFunc.eStatus=_eStatus;
	mLcFunc.pThis=this;
	if(_eStatus==tagTaskThreadCommonSessionBasic::c_cmd_click_delfile)
	{
		mLcFunc.sDelFileName.assign((const char*)_data);
	}
	this->pMainFrameDlgBasic->postTaskMessage(mLcFunc);
}

