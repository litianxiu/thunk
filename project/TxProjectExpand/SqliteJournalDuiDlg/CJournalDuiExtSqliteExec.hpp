#pragma once

#include <assert.h>
#include "CSqliteJournalDuiDlg.h"

class CJournalDuiExtSqliteExec
{
private:
	CSqliteJournalDuiDlg *pSqliteJournalDuiDlg;
	CMainFrameDlgBasic *pMainFrameDlgBasic;
	TxSystemDependent::TxTimeSpan<false> m_time_span;
	long long ll_id_max;
	TxBlobString sErrorText;
	int iLstGainRowCount;
public :

	CJournalDuiExtSqliteExec(CSqliteJournalDuiDlg *_pSqliteJournalDuiDlg,CMainFrameDlgBasic *_pMainFrameDlgBasic)
	{
		this->pSqliteJournalDuiDlg=_pSqliteJournalDuiDlg;
		this->pMainFrameDlgBasic=_pMainFrameDlgBasic;
		this->ll_id_max=-1;
		this->iLstGainRowCount=0;
	}

	void reset()
	{
		this->ll_id_max=-1;
		this->sErrorText.clear();
	}
	inline const TxBlobString& getErrorText() const
	{
		return this->sErrorText;
	}
	inline long long getMsgMaxId() const
	{
		return this->ll_id_max;
	}

	static void CALLBACK g_refreshLogElem_sqlite_cb(void *_userdata,const void *_data)
	{
		CJournalDuiExtSqliteExec *pThis=(CJournalDuiExtSqliteExec*)_userdata;
		tagSqliteExecuteRespondData *p_data=(tagSqliteExecuteRespondData*)_data;
		if(p_data->iCurRow<0)
		{
			if(p_data->iColCount==1) pThis->sErrorText.assign(p_data->value[0]);
			else DefAssertWrap(FALSE);
		}
		else if(p_data->iCurRow<p_data->iRowCount)
		{
			pThis->iLstGainRowCount++;
			pThis->refreshLogElem(p_data);
		}
		else
		{
			DefAssertWrap(FALSE);
		}
	}

	void refreshLogElem(tagSqliteExecuteRespondData *_data)
	{
		TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> lcNewElem=this->pSqliteJournalDuiDlg->getLogContentListWrapExt()->insertFront(new DuiLib::CListTextElementUI());
		lcNewElem->mItemText.resize(this->pSqliteJournalDuiDlg->getListCtrlColumnInfo()->getColumnCount());
		for(int i=(int)_data->iColCount;i--;)
		{
			int nLcIndex=-1;
			if(::strcmp(_data->name[i],"id")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_src_id;
			else if(::strcmp(_data->name[i],"rand")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_msg_rand;
			else if(::strcmp(_data->name[i],"line")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_src_line;
			else if(::strcmp(_data->name[i],"src_file")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_src_file;
			else if(::strcmp(_data->name[i],"message")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_msg_content;
			else if(::strcmp(_data->name[i],"function")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_src_func;
			else if(::strcmp(_data->name[i],"msg_time")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_soft_time;
			else if(::strcmp(_data->name[i],"os_time")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_os_time;
			else if(::strcmp(_data->name[i],"svr_time")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_svr_time;
			else if(::strcmp(_data->name[i],"log_level")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_msg_level;
			else if(::strcmp(_data->name[i],"program_name")==0)
				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_proc_name;
			else
			{
				DefAssertWrap(FALSE);
			}
			lcNewElem->mItemText[nLcIndex]=_data->value[i];
		}
		long long lc_ll_id=TxBlobString((const char*)lcNewElem->mItemText[CListCtrlColumnInfo::e_listctrl_column_src_id]).toDatumRet<long long>(-1);
		if(this->ll_id_max<lc_ll_id) this->ll_id_max=lc_ll_id;
		this->pSqliteJournalDuiDlg->getListLogContent()->Add(lcNewElem->ui);
		if(this->m_time_span.dtctCurTimeResetSpanMsec(1000,true)||(_data->iCurRow+1)==_data->iRowCount)
		{
			DuiLib::CDuiString str;
			str.Format(_T("正在努力打印日志\n[%02d%%]\n"),(int)((_data->iCurRow+1)*100/_data->iRowCount));
			this->pMainFrameDlgBasic->updateAutoWaitDlgText(str);
		}
		assert(_data->iCurRow<(int)_data->iRowCount);
	}

	void inquireNewestLogData(const char *_ip,int _port,int _log_count,int _i_time_msec)
	{
		CDbmsClientSessionDll mConObj(_ip,_port);
		long long ll_id=CSqliteLogExec::execNewestLogDataId(&mConObj,&this->sErrorText);
		this->iLstGainRowCount=0;
		if(ll_id>=0)
		{
			mConObj.executeSql((TxBlobString("select * from journal where id>")
				+TxBlobString::fromDatum(ll_id-_log_count)
				+" limit "+TxBlobString::fromDatum(_log_count+100)+";").c_str(),_i_time_msec,this,g_refreshLogElem_sqlite_cb);
		}
	}

	void executeSql(const char *_ip,int _port,const char *_sql,int _i_time_msec)
	{
		CDbmsClientSessionDll mConObj(_ip,_port);
		this->iLstGainRowCount=0;
		return this->executeSql(&mConObj,_sql,_i_time_msec);
	}

	void executeSql(CDbmsClientSessionDll *_pSqliteDbmsClientSession,const char *_sql,int _i_time_msec)
	{
		this->iLstGainRowCount=0;
		_pSqliteDbmsClientSession->executeSql(_sql,_i_time_msec,this,g_refreshLogElem_sqlite_cb);
	}
	
	inline int getLstGainRowCount() const
	{
		return this->iLstGainRowCount;
	}
};

//
//long long CSqliteJournalDuiDlg::refreshLogMessageContent(TxCppPlatform::shared_ptr<CNsTransmitInfo::tagRespondSqlExecute> &_data,TxAutoWaitDuiDialog *_pWaitWnd)
//{
//	this->clearLogMessageContent();
//	long long ret=-1;
//	std::list<CNsTransmitInfo::tagRespondSqlExecute::tagGroupInfo>::iterator iter=_data->listGroupInfo.begin(),
//		iter_end=_data->listGroupInfo.end();
//	int iLcTotalIndex=0;
//	TxSystemDependent::TxTimeSpan<false> lc_time_span;
//	lc_time_span.reset(1000000);
//	for(;iter!=iter_end;iter++)
//	{
//		CNsTransmitInfo::tagRespondSqlExecute::tagGroupInfo::tagItemInfo *pLcArryItem=iter->vArryItem.data();
//		TxCppPlatform::shared_ptr<CDuiListWrapExt::tagListItem> lcNewElem=this->m_DuiLogViewListWrapExt.insertFront(new DuiLib::CListTextElementUI());
//		lcNewElem->mItemText.resize(this->m_ListCtrlColumnInfo.getColumnCount());
//		for(int i=(int)iter->vArryItem.size();i--;)
//		{
//			int nLcIndex=-1;
//			if(pLcArryItem[i].sName.compare("id")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_src_id;
//			else if(pLcArryItem[i].sName.compare("rand")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_msg_rand;
//			else if(pLcArryItem[i].sName.compare("line")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_src_line;
//			else if(pLcArryItem[i].sName.compare("src_file")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_src_file;
//			else if(pLcArryItem[i].sName.compare("message")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_msg_content;
//			else if(pLcArryItem[i].sName.compare("function")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_src_func;
//			else if(pLcArryItem[i].sName.compare("msg_time")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_soft_time;
//			else if(pLcArryItem[i].sName.compare("os_time")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_os_time;
//			else if(pLcArryItem[i].sName.compare("svr_time")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_svr_time;
//			else if(pLcArryItem[i].sName.compare("log_level")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_msg_level;
//			else if(pLcArryItem[i].sName.compare("program_name")==0)
//				nLcIndex=CListCtrlColumnInfo::e_listctrl_column_proc_name;
//			else
//			{
//				DefAssertWrap(FALSE);
//			}
//			lcNewElem->mItemText[nLcIndex]=pLcArryItem[i].sValue.c_str();
//		}
//		long long aaa=TxBlobString((const char*)lcNewElem->mItemText[CListCtrlColumnInfo::e_listctrl_column_src_id]).toDatumRet<long long>(-1);
//		if(ret<aaa) ret=aaa;
//		this->m_pListLogContent->Add(lcNewElem->ui);
//		iLcTotalIndex++;
//		if(_pWaitWnd!=NULL&&lc_time_span.dtctCurTimeResetSpanMsec(1000,true)||iLcTotalIndex==_data->listGroupInfo.size())
//		{
//			DuiLib::CDuiString str;
//			str.Format(_T("正在努力打印日志\n[%02d%%]\n"),(int)(iLcTotalIndex*100/_data->listGroupInfo.size()));
//			_pWaitWnd->updateText(str);
//		}
//		assert(iLcTotalIndex<=(int)_data->listGroupInfo.size());
//	}
//	return ret;
//}
