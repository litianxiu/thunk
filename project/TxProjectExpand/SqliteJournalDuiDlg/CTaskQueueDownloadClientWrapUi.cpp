#include "StdAfx.h"
#include "CTaskQueueDownloadClientWrapUi.h"
//
//CTaskQueueDownloadClientWrapUi::CTaskQueueDownloadClientWrapUi(const char *_ip,int _port,
//															   CDownUpLoadDuiDlg *_pDownUpLoadDuiDlg, const char *_localFileUri,const char *_remoteFileUri)
//															   :m_TaskDownloadThreadSession(_pDownUpLoadDuiDlg,_ip,_port,_remoteFileUri,_localFileUri)
//{
//	this->pDownUpLoadDuiDlg=_pDownUpLoadDuiDlg;
//	this->initUi();
//}
//
//CTaskQueueDownloadClientWrapUi::~CTaskQueueDownloadClientWrapUi()
//{
//	this->stopTaskThread();
//}
//
//void CTaskQueueDownloadClientWrapUi::initUi()
//{
//	this->SetFixedHeight(38);
//	DuiLib::CVerticalLayoutUI *pVlyt=new DuiLib::CVerticalLayoutUI();
//	pVlyt->SetSepHeight(4);
//	pVlyt->SetFloat(false);
//	{
//		DuiLib::CHorizontalLayoutUI *pHlyt1=new DuiLib::CHorizontalLayoutUI();
//		{
//			DuiLib::CVerticalLayoutUI *pVlyt_1_1=new DuiLib::CVerticalLayoutUI();
//			pVlyt_1_1->SetFixedWidth(16);
//			pVlyt_1_1->SetFloat(false);
//			pHlyt1->Add(pVlyt_1_1);
//		}
//		{
//			DuiLib::CVerticalLayoutUI *pVlyt_mm_12=new DuiLib::CVerticalLayoutUI();
//			pVlyt_mm_12->SetFixedWidth(22);
//			pVlyt_mm_12->SetFloat(false);
//			DuiLib::COptionUI *p_OptionCheck=new DuiLib::COptionUI();
//			p_OptionCheck->SetFloat(false);
//			p_OptionCheck->SetFixedHeight(20);
//
//			p_OptionCheck->SetNormalImage(_T("CheckBox/unchecked.png"));
//			p_OptionCheck->SetSelectedImage(_T("CheckBox/checked.png"));
//			pVlyt_mm_12->Add(new DuiLib::CHorizontalLayoutUI());
//			pVlyt_mm_12->Add(p_OptionCheck);
//			pVlyt_mm_12->Add(new DuiLib::CHorizontalLayoutUI());
//			pHlyt1->Add(pVlyt_mm_12);
//		}
//		{
//			this->p_label_filename=new DuiLib::CLabelUI();
//			this->p_label_filename->SetFloat(false);
//			//this->p_label_filename->SetText(TxBlobString(_localfile).splitSegment("\\","/").back().c_str());
//			pHlyt1->Add(this->p_label_filename);
//		}
//		//{
//		//	DuiLib::CVerticalLayoutUI *pVlyt_1_2=new DuiLib::CVerticalLayoutUI();
//		//	pVlyt_1_2->SetFixedWidth(4);
//		//	pVlyt_1_2->SetFloat(false);
//		//	pHlyt1->Add(pVlyt_1_2);
//		//}
//		{
//			this->p_label_datasize=new DuiLib::CLabelUI();
//			this->p_label_datasize->SetFloat(false);
//			this->p_label_datasize->SetFixedWidth(160);
//			this->p_label_datasize->SetTextStyle(this->p_label_datasize->GetTextStyle()|DT_RIGHT);
//			//this->p_label_datasize->SetText(_T("未知大小"));
//			pHlyt1->Add(this->p_label_datasize);
//		}
//		{
//			DuiLib::CVerticalLayoutUI *pVlyt_1_3=new DuiLib::CVerticalLayoutUI();
//			pVlyt_1_3->SetFixedWidth(16);
//			pVlyt_1_3->SetFloat(false);
//			pHlyt1->Add(pVlyt_1_3);
//		}
//		pVlyt->Add(pHlyt1);
//	}
//	{
//		DuiLib::CHorizontalLayoutUI *pHlyt2=new DuiLib::CHorizontalLayoutUI();
//		pHlyt2->SetFixedHeight(16);
//		{
//			DuiLib::CVerticalLayoutUI *pVlyt_2_1=new DuiLib::CVerticalLayoutUI();
//			pVlyt_2_1->SetFixedWidth(16);
//			pVlyt_2_1->SetFloat(false);
//			pHlyt2->Add(pVlyt_2_1);
//		}
//		{
//			this->p_progress_task=new DuiLib::CProgressUI();
//			this->p_progress_task->SetFloat(false);
//			this->p_progress_task->SetBkImage(_T("下载\\下载进度2.png"));
//			this->p_progress_task->SetForeImage(_T("下载\\下载进度1.png"));
//			this->p_progress_task->SetMinValue(0);
//			this->p_progress_task->SetMaxValue(100);
//			this->p_progress_task->SetValue(0);
//			pHlyt2->Add(this->p_progress_task);
//		}
//		{
//			this->p_label_percent=new DuiLib::CLabelUI();
//			this->p_label_percent->SetFloat(false);
//			this->p_label_percent->SetFixedWidth(40);
//			this->p_label_percent->SetTextStyle(this->p_label_percent->GetTextStyle()|DT_RIGHT);
//			pHlyt2->Add(this->p_label_percent);
//		}
//		{
//			DuiLib::CVerticalLayoutUI *pVlyt_2_3=new DuiLib::CVerticalLayoutUI();
//			pVlyt_2_3->SetFixedWidth(16);
//			pVlyt_2_3->SetFloat(false);
//			pHlyt2->Add(pVlyt_2_3);
//		}
//		pVlyt->Add(pHlyt2);
//	}
//	{
//		DuiLib::CLabelUI *lc_sep_label=new DuiLib::CLabelUI();
//		lc_sep_label->SetFixedHeight(2);
//		lc_sep_label->SetAttribute(_T("bkcolor"),_T("0xFFB0B0E0"));
//		lc_sep_label->SetFloat(false);
//		pVlyt->Add(lc_sep_label);
//	}
//	this->Add(pVlyt);
//}
//
//void CTaskQueueDownloadClientWrapUi::setDownUpLoadContent(long long _currentsize,long long _totalsize,LPCTSTR _filename)
//{
//	if(_filename!=NULL)
//		this->p_label_filename->SetText(_filename);
//	if(_currentsize<0||_totalsize<0)
//	{
//		this->p_label_datasize->SetText(_T("未知"));
//		this->p_label_percent->SetText(_T("0 %"));
//		this->p_progress_task->SetValue(0);
//	}
//	else
//	{
//		TxBlobString lc_tbs=TxBlobString::fromDatum(_currentsize)+" / "+TxBlobString::fromDatum(_totalsize)+" K";
//		this->p_label_datasize->SetText(lc_tbs.c_str());
//		int nLcPercent=(int)((100.9*(double)_currentsize+(1e-20))/((double)_totalsize+(1e-20)));
//		this->p_label_percent->SetText((TxBlobString::fromDatum(nLcPercent)+" %").c_str());
//		this->p_progress_task->SetValue(nLcPercent);
//	}
//}
//
//void CTaskQueueDownloadClientWrapUi::startTaskThread()
//{
//	return this->m_TaskDownloadThreadSession.start();
//}
//
//void CTaskQueueDownloadClientWrapUi::stopTaskThread()
//{
//	return this->m_TaskDownloadThreadSession.stop(false);
//}
//
//CDownUpLoadDuiDlg::tagDuiListTaskDoingQueueElemBase::e_status CTaskQueueDownloadClientWrapUi::getRunStatus()
//{
//	if(this->m_TaskDownloadThreadSession.isThreadRunning())
//		return CDownUpLoadDuiDlg::tagDuiListTaskDoingQueueElemBase::e_running;
//	long long lc_gainsize=-1,lc_totalsize=-1;
//	this->m_TaskDownloadThreadSession.getInformation(&lc_gainsize,&lc_totalsize);
//	if(lc_totalsize>=0&&lc_gainsize==lc_gainsize)
//		return CDownUpLoadDuiDlg::tagDuiListTaskDoingQueueElemBase::e_finish;
//	return CDownUpLoadDuiDlg::tagDuiListTaskDoingQueueElemBase::e_nostart;
//}
//
//void CTaskQueueDownloadClientWrapUi::onTimerRefresh()
//{
//	long long lc_gainsize=-1,lc_totalsize=-1;
//	this->m_TaskDownloadThreadSession.getInformation(&lc_gainsize,&lc_totalsize);
//	this->setDownUpLoadContent(lc_gainsize,lc_totalsize);
//}



