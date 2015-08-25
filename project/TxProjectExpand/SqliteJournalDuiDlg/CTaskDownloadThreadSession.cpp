#include "StdAfx.h"
#include "CTaskDownloadThreadSession.h"
#include "CCustomFileHandleRealize.h"
#include "../common/TxSystemDependent.hpp"
#include "../common/TxBlobString.hpp"
#include "CMainFrameDlgBasic.hpp"

void CTaskDownloadThreadSession::_g_thread_cb_(void *_arg1,void *_arg2)
{
	(void)_arg2;
	TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> spLcSharedThis=((CTaskDownloadThreadSession*)_arg1)->shared_from_this();
	((CTaskDownloadThreadSession*)_arg1)->_thread_cb_(spLcSharedThis);
}

void CTaskDownloadThreadSession::_thread_cb_(TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> &_spThis)
{
	CRemoteNetworkFileHandle lcRemoteNetworkFileHandle(this->strServerIp.c_str(),this->iServerPort,this->strRemoteFileUri.c_str());
	long long llLcTotalSize=-1,llLcGainSize=this->m_llGainSize;
	TxBlobString lc_err_text;
	TxSystemDependent::TxTimeSpan<false> mLcTimeSpan(1000000);
	int iLcReadErrorCount=0;
	BOOL bLcFailure=FALSE;
	while(this->bThreadRun)
	{
		TxBlobString lc_data_tem;
		long long llLcTotalSize_tem=-1;
		//int iLcReadSize=3*1024*1024;
		int iLcReadSize=1*1024*1024;
		if((long long)iLcReadSize>(long long)(llLcTotalSize-llLcGainSize)&&llLcTotalSize>=0)
			iLcReadSize=(int)(llLcTotalSize-llLcGainSize);
		lcRemoteNetworkFileHandle.vf_read(&lc_data_tem,&lc_err_text,&llLcTotalSize_tem,llLcGainSize,iLcReadSize,10*1000);
		if(lc_err_text.size()>0)
		{
			if(iLcReadErrorCount==0)
				this->mEvent.waitEvent((int)(5*1000));
			else
			{
				iLcReadErrorCount=0;
				bLcFailure=TRUE;
				break;
			}
			iLcReadErrorCount++;
		}
		else
		{//正确数据
			if(mLcTimeSpan.dtctCurTimeResetSpanMsec(1*1000,true))
			{
				this->pDownUpLoadDuiDlg->onMyTaskThreadEvent(tagTaskThreadCommonSessionBasic::c_cmd_task_refreshui,_spThis);
			}
			llLcTotalSize=llLcTotalSize_tem;
			llLcGainSize+=lc_data_tem.size();
			if(this->m_llTotalSize!=llLcTotalSize)
				this->m_llTotalSize=llLcTotalSize;
			this->m_llGainSize=llLcGainSize;
			FILE *fp=fopen(this->strSaveFileTempUri.c_str(),"ab+");
			if(fp!=NULL)
			{
				if(lc_data_tem.size()>0)
					fwrite(lc_data_tem.data(),1,lc_data_tem.size(),fp);
				fclose(fp);
			}
			if(llLcTotalSize>=0&&llLcTotalSize==llLcGainSize)
			{
				//lc_err_text=(this->strSaveFileUri.splitSegment("/","\\").back()+"\r\n下载中断，未知异常。\r\n");
				break;
			}
		}
	}
	BOOL bLcSuccess=FALSE;
	if(lc_err_text.size()>0)
		this->pDownUpLoadDuiDlg->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(lc_err_text.c_str());
	if(llLcTotalSize>=0&&llLcTotalSize==llLcGainSize)
	{
		bLcSuccess=TRUE;
		::MoveFileA(this->strSaveFileTempUri.c_str(),this->strSaveFileUri.c_str());
		if(0&&llLcTotalSize>1024*1024)
		{
			this->pDownUpLoadDuiDlg->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notifyFilePath(
				(this->strSaveFileUri+"\r\n下载完毕\n").c_str(),this->strSaveFileUri.c_str());
		}
		this->pDownUpLoadDuiDlg->onMyTaskThreadEvent(tagTaskThreadCommonSessionBasic::e_cmd_task_finish,_spThis);
	}
	if(bLcFailure)
	{
		this->pDownUpLoadDuiDlg->onMyTaskThreadEvent(tagTaskThreadCommonSessionBasic::e_cmd_task_failure,_spThis);
	}
	this->bThreadRun=FALSE;
	//if(bLcSuccess)
	//{
	//	struct {
	//		CDownUpLoadDuiDlg *pDownUpLoadDuiDlg;
	//		void operator()() { this->pDownUpLoadDuiDlg->onTimerMonitorTaskQueue(); }
	//	} m_lc_r;
	//	m_lc_r.pDownUpLoadDuiDlg=this->pDownUpLoadDuiDlg;
	//	this->pDownUpLoadDuiDlg->pMainFrameDlgBasic->postTaskMessage(m_lc_r);
	//}
}

CTaskDownloadThreadSession::CTaskDownloadThreadSession(CDownUpLoadDuiDlg *_pDownUpLoadDuiDlg,const char *_ip,int _port,const char *_sRemoteFileUri,const char *_sLocalFileUri)
{
	this->bThreadRun=FALSE;
	this->m_llGainSize=0;
	this->m_llTotalSize=-1;
	this->pDownUpLoadDuiDlg=_pDownUpLoadDuiDlg;
	this->strServerIp.assign(_ip);
	this->iServerPort=_port;

	this->strRemoteFileUri.assign(_sRemoteFileUri);

	this->strSaveFileUri.assign(_sLocalFileUri);
	this->strSaveFileTempUri.assign((TxBlobString(_sLocalFileUri)+".temp").c_str());

	::DeleteFileA(this->strSaveFileTempUri.c_str());

	{
		TxBlobString lc_localdir;
		std::list<TxBlobString> lc_list_node=TxBlobString(_sLocalFileUri).splitSegment("\\","/");
		if(lc_list_node.size()>2)
		{
			std::list<TxBlobString>::iterator iter=lc_list_node.begin();
			std::list<TxBlobString>::iterator iter_end=lc_list_node.end();
			for(--iter_end;iter!=iter_end;iter++)
				lc_localdir.append((*iter)+"\\");
		}
		DWORD lc_attributes =::GetFileAttributesA(lc_localdir.c_str());
		if(lc_attributes==INVALID_FILE_ATTRIBUTES)
		{
			if(!::MakeSureDirectoryPathExists(lc_localdir.c_str()))
				this->pDownUpLoadDuiDlg->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(_T("无法创建下载目录"));
		}
		else if((lc_attributes&FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			::DeleteFileA(lc_localdir.c_str());
			if(!::MakeSureDirectoryPathExists(lc_localdir.c_str()))
				this->pDownUpLoadDuiDlg->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(_T("无法创建下载目录"));
		}
	}
}

//bool CTaskDownloadThreadSession::isThreadRunning()
//{
//	return !!this->bThreadRun;
//}

void CTaskDownloadThreadSession::start()
{
	this->stop(false);
	this->bThreadRun=TRUE;
	this->mThread.create(_g_thread_cb_,this,NULL);
}

void CTaskDownloadThreadSession::createUi(tagTaskThreadCommonSessionBasic::E_UI _e)
{
	this->assert_ui_empty();
	switch(_e)
	{
	case tagTaskThreadCommonSessionBasic::e_ui_pause:
		this->pUi_pause=this->newPauseUi_();
		break;
	case tagTaskThreadCommonSessionBasic::e_ui_finish:
		this->pUi_finish=this->newFinishUi_();
		break;
	case tagTaskThreadCommonSessionBasic::e_ui_doing:
		this->pUi_doing=this->newDoingUi_();
		break;
	default:
		DefAssertWrap(FALSE);
		break;
	}
}

tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase* CTaskDownloadThreadSession::newPauseUi_(const char *_text)
{
	struct T_R_UI: public tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase
	{
		DuiLib::COptionUI *p_option_select;
		TxBlobString tsFileTempPath;
		virtual void vf_select(bool _b)
		{
			this->p_option_select->Selected(_b);
		}
		virtual bool vf_is_select()
		{
			return this->p_option_select->IsSelected();
		}
		bool onOpenFolderEvent(void* pParam)
		{
			DuiLib::TEventUI* pEvent = (DuiLib::TEventUI*)pParam;
			switch(pEvent->Type)
			{
			case DuiLib::UIEVENT_BUTTONDOWN:
				TxSystemDependent::TxRuntimeWrap::openExplorerSelectFile(tsFileTempPath.c_str());
				break;
			default:
				break;
			}
			return true;
		}
	};
	T_R_UI *res=new T_R_UI();
	long long lc_gainsize=-1,lc_totalsize=-1;
	this->getInformation(&lc_gainsize,&lc_totalsize);
	res->spObject=this->shared_from_this();
	res->SetFixedHeight(48);
	res->tsFileTempPath.assign(this->strSaveFileTempUri);
	DuiLib::CVerticalLayoutUI *pVlyt=new DuiLib::CVerticalLayoutUI();
	pVlyt->SetSepHeight(4);
	pVlyt->SetFloat(false);
	{
		DuiLib::CHorizontalLayoutUI *pHlyt1=new DuiLib::CHorizontalLayoutUI();
		{
			DuiLib::CVerticalLayoutUI *pVlyt_1_1=new DuiLib::CVerticalLayoutUI();
			pVlyt_1_1->SetFixedWidth(16);
			pVlyt_1_1->SetFloat(false);
			pHlyt1->Add(pVlyt_1_1);
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_mm_12=new DuiLib::CVerticalLayoutUI();
			pVlyt_mm_12->SetFixedWidth(22);
			pVlyt_mm_12->SetFloat(false);
			res->p_option_select=new DuiLib::COptionUI();
			res->p_option_select->SetFloat(false);
			res->p_option_select->SetFixedHeight(20);
			res->p_option_select->SetNormalImage(_T("CheckBox/unchecked.png"));
			res->p_option_select->SetSelectedImage(_T("CheckBox/checked.png"));
			pVlyt_mm_12->Add(new DuiLib::CHorizontalLayoutUI());
			pVlyt_mm_12->Add(res->p_option_select);
			pVlyt_mm_12->Add(new DuiLib::CHorizontalLayoutUI());
			pHlyt1->Add(pVlyt_mm_12);
		}
		{
			DuiLib::CLabelUI *p_label_filename=new DuiLib::CLabelUI();
			p_label_filename->SetFloat(false);
			p_label_filename->SetText(this->strSaveFileUri.splitSegment("\\","/").back().c_str());
			pHlyt1->Add(p_label_filename);
		}
		if(lc_totalsize>=0)
		{
			DuiLib::CVerticalLayoutUI *pVlyt_open_vv=new DuiLib::CVerticalLayoutUI();
			pVlyt_open_vv->SetFixedWidth(50);
			pVlyt_open_vv->Add(new DuiLib::CHorizontalLayoutUI());
			{
				DuiLib::CButtonUI *p_btn_open_folder=new DuiLib::CButtonUI();
				p_btn_open_folder->SetFloat(false);
				p_btn_open_folder->SetText(_T("文件夹"));
				p_btn_open_folder->SetAttribute(_T("bkcolor"),_T("#FFA8A8A8"));
				p_btn_open_folder->SetAttribute(_T("textcolor"),_T("#FF0000FF"));
				p_btn_open_folder->SetFont(1);
				p_btn_open_folder->OnEvent+=DuiLib::MakeDelegate(res,&T_R_UI::onOpenFolderEvent);
				pVlyt_open_vv->Add(p_btn_open_folder);
			}
			pHlyt1->Add(pVlyt_open_vv);
		}
		{
			DuiLib::CLabelUI *p_label_datasize=new DuiLib::CLabelUI();
			p_label_datasize->SetFloat(false);
			p_label_datasize->SetFixedWidth(160);
			p_label_datasize->SetTextStyle(p_label_datasize->GetTextStyle()|DT_RIGHT);
			pHlyt1->Add(p_label_datasize);
			if(lc_gainsize<0||lc_totalsize<0)
			{
				p_label_datasize->SetText(_T("未知"));
			}
			else
			{
				p_label_datasize->SetText((TxBlobString::fromDatum(lc_gainsize)+" / "+TxBlobString::fromDatum(lc_totalsize)+" K").c_str());
			}
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_1_3=new DuiLib::CVerticalLayoutUI();
			pVlyt_1_3->SetFixedWidth(16);
			pVlyt_1_3->SetFloat(false);
			pHlyt1->Add(pVlyt_1_3);
		}
		pVlyt->Add(pHlyt1);
	}
	{
		DuiLib::CHorizontalLayoutUI *pHlyt2=new DuiLib::CHorizontalLayoutUI();
		pHlyt2->SetFixedHeight(16);
		{
			DuiLib::CVerticalLayoutUI *pVlyt_2_1=new DuiLib::CVerticalLayoutUI();
			pVlyt_2_1->SetFixedWidth(16);
			pVlyt_2_1->SetFloat(false);
			pHlyt2->Add(pVlyt_2_1);
		}

		DuiLib::CDuiString cs_lc_progress_task_text(_T("0 %"));
		int nLcPercent=0;
		if(lc_gainsize<0||lc_totalsize<0)
		{
			nLcPercent=0;
			cs_lc_progress_task_text=_T("0 %");
		}
		else
		{
			nLcPercent=(int)((100.9*(double)lc_gainsize+(1e-20))/((double)lc_totalsize+(1e-20)));
			cs_lc_progress_task_text=(TxBlobString::fromDatum(nLcPercent)+" %").c_str();
		}

		{
			DuiLib::CProgressUI *p_progress_task=new DuiLib::CProgressUI();
			p_progress_task->SetFloat(false);
			p_progress_task->SetBkImage(_T("下载\\下载进度2.png"));
			p_progress_task->SetForeImage(_T("下载\\下载进度1.png"));
			p_progress_task->SetMinValue(0);
			p_progress_task->SetMaxValue(100);
			p_progress_task->SetValue(nLcPercent);
			pHlyt2->Add(p_progress_task);
		}
		{
			DuiLib::CLabelUI *p_label_percent=new DuiLib::CLabelUI();
			p_label_percent->SetFloat(false);
			p_label_percent->SetFixedWidth(40);
			p_label_percent->SetTextStyle(p_label_percent->GetTextStyle()|DT_RIGHT);
			p_label_percent->SetText(cs_lc_progress_task_text);
			pHlyt2->Add(p_label_percent);
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_2_3=new DuiLib::CVerticalLayoutUI();
			pVlyt_2_3->SetFixedWidth(16);
			pVlyt_2_3->SetFloat(false);
			pHlyt2->Add(pVlyt_2_3);
		}
		pVlyt->Add(pHlyt2);
	}
	{
		DuiLib::CLabelUI *lc_sep_label=new DuiLib::CLabelUI();
		lc_sep_label->SetFixedHeight(2);
		lc_sep_label->SetAttribute(_T("bkcolor"),_T("0xFFB0B0E0"));
		lc_sep_label->SetFloat(false);
		pVlyt->Add(lc_sep_label);
	}

	res->Add(pVlyt);
	return res;
}

tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase* CTaskDownloadThreadSession::newFinishUi_()
{
	struct T_R_UI: public tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase
	{
		DuiLib::COptionUI *p_option_select;
		TxBlobString tsFileUri;
		virtual void vf_select(bool _b)
		{
			this->p_option_select->Selected(_b);
		}
		virtual bool vf_is_select()
		{
			return this->p_option_select->IsSelected();
		}
		bool onOpenFolderEvent(void* pParam)
		{
			DuiLib::TEventUI* pEvent = (DuiLib::TEventUI*)pParam;
			switch(pEvent->Type)
			{
			case DuiLib::UIEVENT_BUTTONDOWN:
				TxSystemDependent::TxRuntimeWrap::openExplorerSelectFile(this->tsFileUri.c_str());
				break;
			default:
				break;
			}
			return true;
		}
		bool onOpenFileEvent(void* pParam)
		{
			DuiLib::TEventUI* pEvent = (DuiLib::TEventUI*)pParam;
			switch(pEvent->Type)
			{
			case DuiLib::UIEVENT_BUTTONDOWN:
				TxSystemDependent::TxRuntimeWrap::openDirectoryOrFile(this->tsFileUri.c_str());
				break;
			default:
				break;
			}
			return true;
		}
	};
	T_R_UI *res=new T_R_UI();
	long long lc_totalsize=-1;
	this->getInformation(NULL,&lc_totalsize);
	res->spObject=this->shared_from_this();
	res->SetFixedHeight(38);
	res->tsFileUri.assign(this->strSaveFileUri);
	DuiLib::CVerticalLayoutUI *pVlyt=new DuiLib::CVerticalLayoutUI();
	pVlyt->SetSepHeight(4);
	pVlyt->SetFloat(false);
	{
		DuiLib::CHorizontalLayoutUI *pHlyt1=new DuiLib::CHorizontalLayoutUI();
		{
			DuiLib::CVerticalLayoutUI *pVlyt_1_1=new DuiLib::CVerticalLayoutUI();
			pVlyt_1_1->SetFixedWidth(16);
			pVlyt_1_1->SetFloat(false);
			pHlyt1->Add(pVlyt_1_1);
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_mm_12=new DuiLib::CVerticalLayoutUI();
			pVlyt_mm_12->SetFixedWidth(22);
			pVlyt_mm_12->SetFloat(false);
			res->p_option_select=new DuiLib::COptionUI();
			res->p_option_select->SetFloat(false);
			res->p_option_select->SetFixedHeight(20);
			res->p_option_select->SetNormalImage(_T("CheckBox/unchecked.png"));
			res->p_option_select->SetSelectedImage(_T("CheckBox/checked.png"));
			pVlyt_mm_12->Add(new DuiLib::CHorizontalLayoutUI());
			pVlyt_mm_12->Add(res->p_option_select);
			pVlyt_mm_12->Add(new DuiLib::CHorizontalLayoutUI());
			pHlyt1->Add(pVlyt_mm_12);
		}
		{
			DuiLib::CLabelUI *p_label_filename=new DuiLib::CLabelUI();
			p_label_filename->SetFloat(false);
			p_label_filename->SetText(this->strSaveFileUri.splitSegment("\\","/").back().c_str());
			pHlyt1->Add(p_label_filename);
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_open_vv=new DuiLib::CVerticalLayoutUI();
			pVlyt_open_vv->SetFixedWidth(50);
			pVlyt_open_vv->Add(new DuiLib::CHorizontalLayoutUI());
			{
				DuiLib::CButtonUI *p_btn_open_folder=new DuiLib::CButtonUI();
				p_btn_open_folder->SetFloat(false);
				p_btn_open_folder->SetText(_T("文件"));
				p_btn_open_folder->SetAttribute(_T("bkcolor"),_T("#FFA8A8A8"));
				p_btn_open_folder->SetAttribute(_T("textcolor"),_T("#FF0000FF"));
				p_btn_open_folder->SetFont(1);
				p_btn_open_folder->OnEvent+=DuiLib::MakeDelegate(res,&T_R_UI::onOpenFileEvent);
				pVlyt_open_vv->Add(p_btn_open_folder);
			}
			pVlyt_open_vv->Add(new DuiLib::CHorizontalLayoutUI());
			pHlyt1->Add(pVlyt_open_vv);
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_mm_555=new DuiLib::CVerticalLayoutUI();
			pVlyt_mm_555->SetFixedWidth(8);
			pHlyt1->Add(pVlyt_mm_555);
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_open_vv=new DuiLib::CVerticalLayoutUI();
			pVlyt_open_vv->SetFixedWidth(50);
			pVlyt_open_vv->Add(new DuiLib::CHorizontalLayoutUI());
			{
				DuiLib::CButtonUI *p_btn_open_folder=new DuiLib::CButtonUI();
				p_btn_open_folder->SetFixedHeight(24);
				p_btn_open_folder->SetFloat(false);
				p_btn_open_folder->SetText(_T("文件夹"));
				p_btn_open_folder->SetAttribute(_T("bkcolor"),_T("#FFA8A8A8"));
				p_btn_open_folder->SetAttribute(_T("textcolor"),_T("#FF0000FF"));
				p_btn_open_folder->SetFont(1);
				p_btn_open_folder->OnEvent+=DuiLib::MakeDelegate(res,&T_R_UI::onOpenFolderEvent);
				pVlyt_open_vv->Add(p_btn_open_folder);
			}
			pVlyt_open_vv->Add(new DuiLib::CHorizontalLayoutUI());
			pHlyt1->Add(pVlyt_open_vv);
		}
		{
			DuiLib::CLabelUI *p_label_datasize=new DuiLib::CLabelUI();
			p_label_datasize->SetFloat(false);
			p_label_datasize->SetFixedWidth(100);
			p_label_datasize->SetTextStyle(p_label_datasize->GetTextStyle()|DT_RIGHT);
			pHlyt1->Add(p_label_datasize);
			if(lc_totalsize<0)
			{
				p_label_datasize->SetText(_T("未知"));
			}
			else
			{
				p_label_datasize->SetText((TxBlobString::fromDatum(lc_totalsize)+" K").c_str());
			}
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_mm_20=new DuiLib::CVerticalLayoutUI();
			pVlyt_mm_20->SetFixedWidth(20);
			pHlyt1->Add(pVlyt_mm_20);
		}
		//{
		//	DuiLib::CVerticalLayoutUI *pVlyt_1_3=new DuiLib::CVerticalLayoutUI();
		//	pVlyt_1_3->SetFixedWidth(16);
		//	pVlyt_1_3->SetFloat(false);
		//	pHlyt1->Add(pVlyt_1_3);
		//}
		pVlyt->Add(pHlyt1);
	}
	//if(0)
	//{
	//	DuiLib::CHorizontalLayoutUI *pHlyt2=new DuiLib::CHorizontalLayoutUI();
	//	pHlyt2->SetFixedHeight(16);
	//	{
	//		DuiLib::CVerticalLayoutUI *pVlyt_2_1=new DuiLib::CVerticalLayoutUI();
	//		pVlyt_2_1->SetFixedWidth(16);
	//		pVlyt_2_1->SetFloat(false);
	//		pHlyt2->Add(pVlyt_2_1);
	//	}

	//	DuiLib::CDuiString cs_lc_progress_task_text(_T("0 %"));
	//	int nLcPercent=0;
	//	if(0)//if(lc_gainsize<0||lc_totalsize<0)
	//	{
	//		nLcPercent=0;
	//		cs_lc_progress_task_text=_T("0 %");
	//	}
	//	else
	//	{
	//		nLcPercent=(int)((100.9*(double)lc_gainsize+(1e-20))/((double)lc_totalsize+(1e-20)));
	//		cs_lc_progress_task_text=(TxBlobString::fromDatum(nLcPercent)+" %").c_str();
	//	}

	//	{
	//		DuiLib::CProgressUI *p_progress_task=new DuiLib::CProgressUI();
	//		p_progress_task->SetFloat(false);
	//		p_progress_task->SetBkImage(_T("下载\\下载进度2.png"));
	//		p_progress_task->SetForeImage(_T("下载\\下载进度1.png"));
	//		p_progress_task->SetMinValue(0);
	//		p_progress_task->SetMaxValue(100);
	//		p_progress_task->SetValue(nLcPercent);
	//		pHlyt2->Add(p_progress_task);
	//	}
	//	{
	//		DuiLib::CLabelUI *p_label_percent=new DuiLib::CLabelUI();
	//		p_label_percent->SetFloat(false);
	//		p_label_percent->SetFixedWidth(40);
	//		p_label_percent->SetTextStyle(p_label_percent->GetTextStyle()|DT_RIGHT);
	//		p_label_percent->SetText(cs_lc_progress_task_text);
	//		pHlyt2->Add(p_label_percent);
	//	}
	//	{
	//		DuiLib::CVerticalLayoutUI *pVlyt_2_3=new DuiLib::CVerticalLayoutUI();
	//		pVlyt_2_3->SetFixedWidth(16);
	//		pVlyt_2_3->SetFloat(false);
	//		pHlyt2->Add(pVlyt_2_3);
	//	}
	//	pVlyt->Add(pHlyt2);
	//}
	{
		DuiLib::CLabelUI *lc_sep_label=new DuiLib::CLabelUI();
		lc_sep_label->SetFixedHeight(2);
		lc_sep_label->SetAttribute(_T("bkcolor"),_T("0xFFB0B0E0"));
		lc_sep_label->SetFloat(false);
		pVlyt->Add(lc_sep_label);
	}

	res->Add(pVlyt);
	return res;
}

void CTaskDownloadThreadSession::deletefile()
{
	::DeleteFileA(this->strSaveFileUri.c_str());
	::DeleteFileA(this->strSaveFileTempUri.c_str());
}

tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase* CTaskDownloadThreadSession::newDoingUi_()
{
	struct T_R_UI: public tagTaskThreadCommonSessionBasic::tagTaskQueueCommonElemBase
	{
		DuiLib::CLabelUI *p_label_filename;
		DuiLib::CLabelUI *p_label_datasize;
		DuiLib::CProgressUI *p_progress_task;
		DuiLib::CLabelUI *p_label_percent;
		DuiLib::COptionUI *p_option_select;
		virtual void vf_select(bool _b)
		{
			this->p_option_select->Selected(_b);
		}
		virtual bool vf_is_select()
		{
			return this->p_option_select->IsSelected();
		}
		virtual void onTimerRefresh()
		{
			TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> spLcObject=this->spObject;
			CTaskDownloadThreadSession *pLcParent=dynamic_cast<CTaskDownloadThreadSession*>(spLcObject.get());
			if(pLcParent==NULL) return ;
			long long lc_gainsize=-1,lc_totalsize=-1;
			pLcParent->getInformation(&lc_gainsize,&lc_totalsize);
			if(lc_gainsize<0||lc_totalsize<0)
			{
				this->p_label_datasize->SetText(_T("未知"));
				this->p_label_percent->SetText(_T("0 %"));
				this->p_progress_task->SetValue(0);
			}
			else
			{
				TxBlobString lc_tbs=TxBlobString::fromDatum(lc_gainsize)+" / "+TxBlobString::fromDatum(lc_totalsize)+" K";
				this->p_label_datasize->SetText(lc_tbs.c_str());
				int nLcPercent=(int)((100.9*(double)lc_gainsize+(1e-20))/((double)lc_totalsize+(1e-20)));
				this->p_label_percent->SetText((TxBlobString::fromDatum(nLcPercent)+" %").c_str());
				this->p_progress_task->SetValue(nLcPercent);
			}
		}
	};
	T_R_UI *res=new T_R_UI();
	res->spObject=this->shared_from_this();
	res->SetFixedHeight(38);
	DuiLib::CVerticalLayoutUI *pVlyt=new DuiLib::CVerticalLayoutUI();
	pVlyt->SetSepHeight(4);
	pVlyt->SetFloat(false);
	{
		DuiLib::CHorizontalLayoutUI *pHlyt1=new DuiLib::CHorizontalLayoutUI();
		{
			DuiLib::CVerticalLayoutUI *pVlyt_1_1=new DuiLib::CVerticalLayoutUI();
			pVlyt_1_1->SetFixedWidth(16);
			pVlyt_1_1->SetFloat(false);
			pHlyt1->Add(pVlyt_1_1);
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_mm_12=new DuiLib::CVerticalLayoutUI();
			pVlyt_mm_12->SetFixedWidth(22);
			pVlyt_mm_12->SetFloat(false);
			res->p_option_select=new DuiLib::COptionUI();
			res->p_option_select->SetFloat(false);
			res->p_option_select->SetFixedHeight(20);
			res->p_option_select->SetNormalImage(_T("CheckBox/unchecked.png"));
			res->p_option_select->SetSelectedImage(_T("CheckBox/checked.png"));
			pVlyt_mm_12->Add(new DuiLib::CHorizontalLayoutUI());
			pVlyt_mm_12->Add(res->p_option_select);
			pVlyt_mm_12->Add(new DuiLib::CHorizontalLayoutUI());
			pHlyt1->Add(pVlyt_mm_12);
		}
		{
			res->p_label_filename=new DuiLib::CLabelUI();
			res->p_label_filename->SetFloat(false);
			res->p_label_filename->SetText(this->strSaveFileUri.splitSegment("\\","/").back().c_str());
			pHlyt1->Add(res->p_label_filename);
		}
		{
			res->p_label_datasize=new DuiLib::CLabelUI();
			res->p_label_datasize->SetFloat(false);
			res->p_label_datasize->SetFixedWidth(160);
			res->p_label_datasize->SetTextStyle(res->p_label_datasize->GetTextStyle()|DT_RIGHT);
			pHlyt1->Add(res->p_label_datasize);
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_1_3=new DuiLib::CVerticalLayoutUI();
			pVlyt_1_3->SetFixedWidth(16);
			pVlyt_1_3->SetFloat(false);
			pHlyt1->Add(pVlyt_1_3);
		}
		pVlyt->Add(pHlyt1);
	}
	{
		DuiLib::CHorizontalLayoutUI *pHlyt2=new DuiLib::CHorizontalLayoutUI();
		pHlyt2->SetFixedHeight(16);
		{
			DuiLib::CVerticalLayoutUI *pVlyt_2_1=new DuiLib::CVerticalLayoutUI();
			pVlyt_2_1->SetFixedWidth(16);
			pVlyt_2_1->SetFloat(false);
			pHlyt2->Add(pVlyt_2_1);
		}
		{
			res->p_progress_task=new DuiLib::CProgressUI();
			res->p_progress_task->SetFloat(false);
			res->p_progress_task->SetBkImage(_T("下载\\下载进度2.png"));
			res->p_progress_task->SetForeImage(_T("下载\\下载进度1.png"));
			res->p_progress_task->SetMinValue(0);
			res->p_progress_task->SetMaxValue(100);
			res->p_progress_task->SetValue(0);
			pHlyt2->Add(res->p_progress_task);
		}
		{
			res->p_label_percent=new DuiLib::CLabelUI();
			res->p_label_percent->SetFloat(false);
			res->p_label_percent->SetFixedWidth(40);
			res->p_label_percent->SetTextStyle(res->p_label_percent->GetTextStyle()|DT_RIGHT);
			pHlyt2->Add(res->p_label_percent);
		}
		{
			DuiLib::CVerticalLayoutUI *pVlyt_2_3=new DuiLib::CVerticalLayoutUI();
			pVlyt_2_3->SetFixedWidth(16);
			pVlyt_2_3->SetFloat(false);
			pHlyt2->Add(pVlyt_2_3);
		}
		pVlyt->Add(pHlyt2);
	}
	{
		DuiLib::CLabelUI *lc_sep_label=new DuiLib::CLabelUI();
		lc_sep_label->SetFixedHeight(2);
		lc_sep_label->SetAttribute(_T("bkcolor"),_T("0xFFB0B0E0"));
		lc_sep_label->SetFloat(false);
		pVlyt->Add(lc_sep_label);
	}
	res->Add(pVlyt);
	return res;
}

void CTaskDownloadThreadSession::stop(bool _bWaitFinish)
{
	if(!_bWaitFinish)
		this->bThreadRun=FALSE;
	this->mEvent.setEvent();
	this->mThread.join();
}

void CTaskDownloadThreadSession::getInformation(long long *_gainsize,long long *_totalsize)
{
	if(_gainsize!=NULL)
		*_gainsize=this->m_llGainSize;
	if(_totalsize!=NULL)
		*_totalsize=m_llTotalSize;
}

