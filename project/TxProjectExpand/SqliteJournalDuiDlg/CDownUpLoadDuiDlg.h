#pragma once

#include <map>
#include <set>
#include <memory>
#include <UIlib.h>
#include "CMainFrameDlgBasic.hpp"
#include "../include/CSqliteExpandDll.h"

struct tagTaskThreadCommonSessionBasic: public TxCppPlatform::enable_shared_from_this<tagTaskThreadCommonSessionBasic>
{
	enum E_CMD
	{
		e_cmd_null1213231=0,
		e_cmd_task_failure,
		e_cmd_task_finish,
		c_cmd_task_refreshui,
		c_cmd_click_restart,
		c_cmd_click_pause,
		c_cmd_click_delfile,
		c_cmd_click_clearitem,
	};
	enum E_UI
	{
		e_ui_null12121212=0,
		e_ui_doing,
		e_ui_finish,
		e_ui_pause,
	};
	struct tagTaskQueueCommonElemBase: public DuiLib::CListContainerElementUI
	{
		TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> spObject;
		virtual ~tagTaskQueueCommonElemBase() { }
		virtual void onTimerRefresh() { }
		virtual void vf_select(bool _b)=0;
		virtual bool vf_is_select()=0;
	};
	BOOL bUiStartThread;
	tagTaskQueueCommonElemBase *pUi_doing;
	tagTaskQueueCommonElemBase *pUi_pause;
	tagTaskQueueCommonElemBase *pUi_finish;
public :
	tagTaskThreadCommonSessionBasic() { this->bUiStartThread=FALSE; this->pUi_doing=this->pUi_finish=this->pUi_pause=NULL; }
	virtual ~tagTaskThreadCommonSessionBasic() { }
	void assert_ui_empty() const { assert(this->pUi_doing==NULL&&this->pUi_finish==NULL&&this->pUi_pause==NULL); }
	virtual void start()=0;
	virtual void stop(bool _bWaitFinish)=0;
	virtual void createUi(tagTaskThreadCommonSessionBasic::E_UI _e)=0;
	virtual void deletefile()=0;
};

class CDownUpLoadDuiDlg: public  DuiLib::WindowImplBase
{
private:
	DuiLib::CButtonUI *p_btn_minwnd;
	DuiLib::CButtonUI *p_btn_maxwnd;
	DuiLib::CButtonUI *p_btn_restorewnd;
	DuiLib::CButtonUI *p_btn_closewnd;
	DuiLib::CTabLayoutUI *p_tab_main_wnd;
	DuiLib::COptionUI *p_Option_task_doing;
	DuiLib::COptionUI *p_Option_task_dofinish;
	DuiLib::COptionUI *p_Option_task_dofailure;
	DuiLib::CVerticalLayoutUI *p_VerticalLayout_task_doing;
	DuiLib::CVerticalLayoutUI *p_VerticalLayout_task_dofinish;
	DuiLib::CVerticalLayoutUI *p_VerticalLayout_task_dofailure;

	DuiLib::CListUI *p_list_down_up_load_doing;
	DuiLib::CListUI *p_list_down_up_load_finish;
	DuiLib::CListUI *p_list_down_up_load_failure;
	DuiLib::COptionUI *p_Option_select_all_doing;
	DuiLib::COptionUI *p_Option_select_all_failure;
	DuiLib::COptionUI *p_Option_select_all_finish;

	DuiLib::CButtonUI *p_btn_clear_all_pause_doing;
	DuiLib::CButtonUI *p_btn_clear_all_doing;
	DuiLib::CButtonUI *p_btn_delete_all_file_finish;
	DuiLib::CButtonUI *p_btn_clear_all_finish;
	DuiLib::CButtonUI *p_btn_restart_all_file_pause;
	DuiLib::CButtonUI *p_btn_clear_all_failure;

	BOOL bInitControlUI;
	TxSystemDependent::TxTimeSpan<false> m_timer_span_refresh_list_item;
public :
	CMainFrameDlgBasic *const pMainFrameDlgBasic;
//public :
	//std::map<tagTaskThreadCommonSessionBasic*,TxCppPlatform::weak_ptr<tagTaskThreadCommonSessionBasic>> mListUpDownLoadAvailElemPtrMap;
	//std::set<tagDuiListTaskDoingQueueElemBase*> mListUpDownLoadAvailElemPtrSet;//所有Elem
	//enum { e_meanwhile_max_task_queue_count=1, };//同时下载或者上传的任务最大数量
public :
	CDownUpLoadDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic);
	~CDownUpLoadDuiDlg();
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual UINT GetClassStyle() const;
	virtual DuiLib::UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetWindowClassName() const;
	virtual void InitWindow();
	virtual LRESULT MessageHandler(UINT _uMsg, WPARAM _wParam, LPARAM _lParam, bool& _bHandled);
	virtual LRESULT HandleMessage(UINT _uMsg,WPARAM _wParam,LPARAM _lParam);
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	//static void CALLBACK _static_download_call_back_(void *_userdata,const void *_tem);
	//void _download_call_back_(const tagDownLoadServerFileRespondData &_tem);
	void addDownLoadTask(const char *_ip,int _port,const char *_sLocalUri,const char *_sRemoteUri);
	void removeAllTaskQueue();
	//void awakeOneQueueThreadTask();
	//bool onTimerMonitorTaskQueue();
	void onMyTaskThreadEvent(tagTaskThreadCommonSessionBasic::E_CMD _eStatus,TxCppPlatform::shared_ptr<tagTaskThreadCommonSessionBasic> _spThreadSession,const void *_data=NULL);
	void doWakeUpTaskThread();
	static void gListSelectAllTask(DuiLib::CListUI *_p_list,bool _b_select);
	void deal_btn_clear_all_item_(tagTaskThreadCommonSessionBasic::E_UI _e,bool _b_delfile);
	void deal_btn_clear_all_pause_doing();
	void deal_btn_restart_all_file_pause();
};


