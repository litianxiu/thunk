#pragma once

#include <ShellApi.h>
#include <list>
#include "CMainFrameDlgBasic.hpp"
#include "CRemoteFileFileContentListUI.h"
#include "CDownUpLoadDuiDlg.h"

class CBrowseRemoteDuiDlg: public  DuiLib::WindowImplBase
{
public :
	struct tagBrowseRemoteDeleteFileCallBackInfo
	{
		CBrowseRemoteDuiDlg *pThis;
		TxBlobString sErrorText;
	};
	enum {
		e_remote_file_list_menu_null=0,
		e_remote_file_list_menu_attr,
		e_remote_file_list_menu_delete,
		e_remote_file_list_menu_download,
		e_remote_file_list_menu_play_video,
	};
private:
	BOOL bInitControlUI;
	DuiLib::CEditUI *p_Edit_ip;
	DuiLib::CEditUI *p_Edit_port;
	DuiLib::CButtonUI *p_btn_parent_directory;
	//DuiLib::CButtonUI *p_btn_back_directory;
	DuiLib::CButtonUI *p_btn_master_directory;
	//DuiLib::CButtonUI *p_btn_forward_directory;
	DuiLib::CEditUI *p_Edit_complex_file_path;
	DuiLib::CEditUI *p_Edit_wildcard_string;
	DuiLib::CButtonUI *p_btn_search_remotefile;
	DuiLib::CListUI *p_list_file_content;
	CRemoteFileFileContentListUI m_RemoteFileFileContentListUI;
	CMainFrameDlgBasic *const pMainFrameDlgBasic;
	CDownUpLoadDuiDlg mDownUpLoadDuiDlg;
	TxBlobString sServerIp;
	int iServerPort;
	CString csOpenFolderInitPath;
public :
	CBrowseRemoteDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic);
	~CBrowseRemoteDuiDlg();
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	DuiLib::UILIB_RESOURCETYPE GetResourceType() const;
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam);
	virtual void Notify(DuiLib::TNotifyUI& _msg);
	static void CALLBACK gTSqliteCallBackDllFuncFindFile(void *_userdata,const void *_data);
	static void CALLBACK gTSqliteCallBackDllFuncDeleteFile(void *_userdata,const void *_data);
	void myDoMessageEvent(HDROP _hDrop);
	void dealDropFile(const std::list<std::string> &_list_string);
	void remoteFileListContentMenuClick(int _item,int _i_clck_index);
	static void gTheDoubleClickListItem(void *_userdata);
	void showDownUpLoadTaskWnd();
	void myClickDownLoadOp();
	int getAllListFileByDir(std::list<TxBlobString> *_outFileUri,CDbmsClientSessionDll *_pSession,const char* _remote_dir);
};


