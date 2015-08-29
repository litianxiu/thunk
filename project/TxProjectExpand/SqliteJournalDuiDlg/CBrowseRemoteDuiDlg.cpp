#include "stdafx.h"

#include "../common/TxSelectFolderDialog.hpp"
#include "CBrowseRemoteDuiDlg.h"
#include "CCustomFileHandleRealize.h"
#include "CMainFrameDlg.h"
#include "../av/CDuiFfmpegPlayWndManager.h"

CBrowseRemoteDuiDlg::CBrowseRemoteDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic)
:pMainFrameDlgBasic(_pMainFrameDlgBasic),mDownUpLoadDuiDlg(_pMainFrameDlgBasic),m_RemoteFileFileContentListUI(_pMainFrameDlgBasic)
{
	this->bInitControlUI=FALSE;
}

CBrowseRemoteDuiDlg::~CBrowseRemoteDuiDlg()
{
}

DuiLib::CDuiString CBrowseRemoteDuiDlg::GetSkinFolder()
{
	return DefDuiSkin;
}

DuiLib::CDuiString CBrowseRemoteDuiDlg::GetSkinFile()
{
	return _T("browse_remote_file.xml");
}

LPCTSTR CBrowseRemoteDuiDlg::GetWindowClassName() const
{
	return _T("CBrowseRemoteDuiDlg");
}

UINT CBrowseRemoteDuiDlg::GetClassStyle() const
{
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
}

DuiLib::UILIB_RESOURCETYPE CBrowseRemoteDuiDlg::GetResourceType() const
{
	return DuiLib::UILIB_FILE;
}

void CBrowseRemoteDuiDlg::gTheDoubleClickListItem(void *_userdata)
{
	CBrowseRemoteDuiDlg *pThis=(CBrowseRemoteDuiDlg*)_userdata;
	TxBlobString str_ip;
	int iLcPort=0;
	std::list<CRemoteFileFileContentListUI::tagFileInfo> lc_list_file=pThis->m_RemoteFileFileContentListUI.getSelectListFileInfo();
	assert(lc_list_file.size()==1);
	if(lc_list_file.size()==1&&CMainFrameDlgBasic::gTransTcpIpPort(&str_ip,&iLcPort,pThis->p_Edit_ip->GetText(),pThis->p_Edit_port->GetText()))
	{
		pThis->sServerIp=str_ip;
		pThis->iServerPort=iLcPort;
		CRemoteFileFileContentListUI::tagFileInfo lc_file_info=lc_list_file.front();
		TxBlobString str((LPCTSTR)pThis->p_Edit_complex_file_path->GetText());
		if(str.backChar()!='\\')
			str+="\\";
		str+=lc_file_info.filename;
		if(lc_file_info.filesize<0)
		{//文件目录
			struct T_R
			{
				CBrowseRemoteDuiDlg *pThis;
				TxBlobString s_filename;
				TxBlobString s_ip;
				int port;
			public:
				void operator()()
				{
					TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(pThis->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在搜索远程文件。")));
					CDbmsClientSessionDll mLcSession(this->s_ip.c_str(),this->port);
					mLcSession.executeFindRemoteFile(this->s_filename.c_str(),NULL,5*1000,this->pThis,gTSqliteCallBackDllFuncFindFile);
					::ReleaseCapture();
				}
			};
			pThis->p_list_file_content->RemoveAll();
			T_R m_t_r;
			m_t_r.s_filename=str;
			m_t_r.pThis=pThis;
			m_t_r.s_ip=str_ip;
			m_t_r.port=iLcPort;
			pThis->pMainFrameDlgBasic->postTaskMessage(m_t_r);
		}
		else
		{//文件名
		}
	}
}

void CBrowseRemoteDuiDlg::InitWindow()
{
	assert(this->bInitControlUI==FALSE);
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Edit_ip,_T("Edit_ip"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Edit_port,_T("Edit_port"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_parent_directory,_T("btn_parent_directory"));
	//CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_back_directory,_T("btn_back_directory"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_master_directory,_T("btn_master_directory"));
	//CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_forward_directory,_T("btn_forward_directory"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Edit_complex_file_path,_T("Edit_complex_file_path"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_Edit_wildcard_string,_T("Edit_wildcard_string"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_btn_search_remotefile,_T("btn_search_remotefile"));
	CMainFrameDlgBasic::buildSaftWidgetPtr(this->m_PaintManager,&this->p_list_file_content,_T("list_file_content"));
	this->bInitControlUI=TRUE;
	this->m_RemoteFileFileContentListUI.initListUi(this->p_list_file_content,this,&CBrowseRemoteDuiDlg::gTheDoubleClickListItem);
	this->mDownUpLoadDuiDlg.Create(this->GetHWND(),NULL,UI_WNDSTYLE_FRAME,0);
	this->mDownUpLoadDuiDlg.ShowWindow(false);
}

LRESULT CBrowseRemoteDuiDlg::HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch(_uMsg)
	{
	case WM_DROPFILES:
		this->myDoMessageEvent((HDROP)_wParam);
		break;
	case WM_TIMER:
		{
			int jklfsd=0;
		}
		break;
	default:
		break;
	}
	return __super::HandleMessage(_uMsg,_wParam,_lParam);
}

void CBrowseRemoteDuiDlg::Notify(DuiLib::TNotifyUI& _msg)
{
	if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0) 
	{
		if(_msg.pSender==this->p_btn_parent_directory)
		{
			TxBlobString str_ip;
			int iLcPort=0;
			if(CMainFrameDlgBasic::gTransTcpIpPort(&str_ip,&iLcPort,this->p_Edit_ip->GetText(),this->p_Edit_port->GetText()))
			{
				this->sServerIp=str_ip;
				this->iServerPort=iLcPort;
				CDbmsClientSessionDll mLcSession(str_ip.c_str(),iLcPort);
				this->p_list_file_content->RemoveAll();
				TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在查询远程文件。")));
				TxBlobString sLcFilePathBasic((LPCTSTR)this->p_Edit_complex_file_path->GetText());
				if(sLcFilePathBasic.backChar()!='\\')
					sLcFilePathBasic+="\\";
				sLcFilePathBasic+="..\\";
				this->p_Edit_complex_file_path->SetText(sLcFilePathBasic.c_str());
				mLcSession.executeFindRemoteFile(sLcFilePathBasic.c_str(),NULL,5*1000,this,gTSqliteCallBackDllFuncFindFile);
			}
		}
		else if(_msg.pSender==this->p_btn_master_directory)
		{
			TxBlobString str_ip;
			int iLcPort=0;
			if(CMainFrameDlgBasic::gTransTcpIpPort(&str_ip,&iLcPort,this->p_Edit_ip->GetText(),this->p_Edit_port->GetText()))
			{
				this->sServerIp=str_ip;
				this->iServerPort=iLcPort;
				CDbmsClientSessionDll mLcSession(str_ip.c_str(),iLcPort);
				this->p_list_file_content->RemoveAll();
				TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在查询远程文件。")));
				this->p_Edit_complex_file_path->SetText(".");
				mLcSession.executeFindRemoteFile(".",NULL,5*1000,this,gTSqliteCallBackDllFuncFindFile);
			}
		}
		else if(_msg.pSender==this->p_btn_search_remotefile)
		{
			TxBlobString str_ip;
			int iLcPort=0;
			if(CMainFrameDlgBasic::gTransTcpIpPort(&str_ip,&iLcPort,this->p_Edit_ip->GetText(),this->p_Edit_port->GetText()))
			{
				this->sServerIp=str_ip;
				this->iServerPort=iLcPort;
				CDbmsClientSessionDll mLcSession(str_ip.c_str(),iLcPort);
				this->p_list_file_content->RemoveAll();
				TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在搜索远程文件。")));
				CString sLcFilePathBasic=(LPCTSTR)this->p_Edit_complex_file_path->GetText();
				CString sLcWildcard=(LPCTSTR)this->p_Edit_wildcard_string->GetText();
				mLcSession.executeFindRemoteFile(sLcFilePathBasic,sLcWildcard,5*1000,this,gTSqliteCallBackDllFuncFindFile);
			}
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_MENU)==0)
	{
		if(_msg.pSender==this->p_list_file_content)
		{
			int iLcCurItem=this->p_list_file_content->GetCurSel();
			if(iLcCurItem>=0)
			{
				this->pMainFrameDlgBasic->getDuiListMenuExt()->run(TxCppPlatform::bind(&CBrowseRemoteDuiDlg::remoteFileListContentMenuClick,
					this,iLcCurItem,TxCppPlatform::placeholders::_1),
					_T("播放"),e_remote_file_list_menu_play_video,
					_T("下载"),e_remote_file_list_menu_download,
					_T("删除"),e_remote_file_list_menu_delete,
					_T("属性"),e_remote_file_list_menu_attr,
					NULL);
			}
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_RETURN)==0)
	{
		//if(p_list_file_content->GetCount()>2)
		//{
		//	DuiLib::CListContainerElementUI *ppp[]=
		//	{
		//		dynamic_cast<DuiLib::CListContainerElementUI*>(p_list_file_content->GetItemAt(0)),
		//	dynamic_cast<DuiLib::CListContainerElementUI*>(p_list_file_content->GetItemAt(1)),
		//	dynamic_cast<DuiLib::CListContainerElementUI*>(p_list_file_content->GetItemAt(2))
		//	};
		//	for(int i=0;i<3;i++)
		//	{
		//		ppp[i]->SetEnabled(true);
		//		ppp[i]->Select(true);
		//	}
		//}
		if(_msg.pSender==this->p_Edit_complex_file_path)
		{
			TxBlobString str_ip;
			int iLcPort=0;
			if(CMainFrameDlgBasic::gTransTcpIpPort(&str_ip,&iLcPort,this->p_Edit_ip->GetText(),this->p_Edit_port->GetText()))
			{
				this->sServerIp=str_ip;
				this->iServerPort=iLcPort;
				CDbmsClientSessionDll mLcSession(str_ip.c_str(),iLcPort);
				this->p_list_file_content->RemoveAll();
				TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在查询远程文件。")));
				CString csLcText=(LPCTSTR)this->p_Edit_complex_file_path->GetText();
				mLcSession.executeFindRemoteFile(csLcText,NULL,5*1000,this,gTSqliteCallBackDllFuncFindFile);
			}
		}
		else if(_msg.pSender==this->p_Edit_wildcard_string)
		{
			TxBlobString str_ip;
			int iLcPort=0;
			if(CMainFrameDlgBasic::gTransTcpIpPort(&str_ip,&iLcPort,this->p_Edit_ip->GetText(),this->p_Edit_port->GetText()))
			{
				this->sServerIp=str_ip;
				this->iServerPort=iLcPort;
				CDbmsClientSessionDll mLcSession(str_ip.c_str(),iLcPort);
				this->p_list_file_content->RemoveAll();
				TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在搜索远程文件。")));
				CString sLcFilePathBasic=(LPCTSTR)this->p_Edit_complex_file_path->GetText();
				CString sLcWildcard=(LPCTSTR)this->p_Edit_wildcard_string->GetText();
				mLcSession.executeFindRemoteFile(sLcFilePathBasic,sLcWildcard,5*1000,this,gTSqliteCallBackDllFuncFindFile);
			}
		}
	}
	else if(_msg.sType.Compare(DUI_MSGTYPE_WINDOWINIT)==0) 
	{
		int lkjdsf=0;
		//if(_msg.pSender==this->m_pCloseBtn)
		//{
		//	this->Close();
		//	return ;
		//}
	}
	return __super::Notify(_msg);
}

void CBrowseRemoteDuiDlg::remoteFileListContentMenuClick(int _item,int _i_clck_index)
{
	(void)_item; (void)_i_clck_index;
	switch(_i_clck_index)
	{
	case e_remote_file_list_menu_attr://属性
		break;
	case e_remote_file_list_menu_download://下载
		this->myClickDownLoadOp();
		break;
	case e_remote_file_list_menu_delete://删除
		{
			TxBlobString str_ip;
			int iLcPort=0;
			if(CMainFrameDlgBasic::gTransTcpIpPort(&str_ip,&iLcPort,this->p_Edit_ip->GetText(),this->p_Edit_port->GetText()))
			{
				this->sServerIp=str_ip;
				this->iServerPort=iLcPort;
				CDbmsClientSessionDll mLcSession(str_ip.c_str(),iLcPort);
				TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在删除远程文件。")));
				std::list<CRemoteFileFileContentListUI::tagFileInfo> list_ssss=this->m_RemoteFileFileContentListUI.getSelectListFileInfo();
				std::vector<LPCTSTR> v_pFileName;
				for(std::list<CRemoteFileFileContentListUI::tagFileInfo>::iterator iter=list_ssss.begin();iter!=list_ssss.end();iter++)
					v_pFileName.push_back(iter->filename.c_str());
				tagBrowseRemoteDeleteFileCallBackInfo lc_cb_res;
				lc_cb_res.pThis=this;
				TxBlobString lcFilePathBasic((LPCTSTR)this->p_Edit_complex_file_path->GetText());
				mLcSession.executeDeleteRemoteFile(lcFilePathBasic.c_str(),
					&v_pFileName.front(),(int)v_pFileName.size(),5*1000,gTSqliteCallBackDllFuncDeleteFile,&lc_cb_res);
				if(lc_cb_res.sErrorText.length()>0)
				{
					this->pMainFrameDlgBasic->clearAutoWaitDuiDialog();
					TxDuiMessageBox::run(this->GetHWND(),_T("删除错误"),lc_cb_res.sErrorText.c_str(),_T("确定"));
				}
				else
				{
					this->pMainFrameDlgBasic->clearAutoWaitDuiDialog();
					TxDuiMessageBox::run(this->GetHWND(),_T("删除文件"),_T("文件删除成功"),_T("确定"));
				}
			}
		}
		break;
	case e_remote_file_list_menu_play_video:
		{
			std::list<CRemoteFileFileContentListUI::tagFileInfo> lc_list_click_file_lef=this->m_RemoteFileFileContentListUI.getSelectListFileInfo();
			if(lc_list_click_file_lef.size()<=0)
			{
				TxDuiMessageBox::run(this->GetHWND(),_T("提示"),_T("最多能选择一个文件播放"),_T("确定"));
			}
			else if(lc_list_click_file_lef.size()>1)
			{
				TxDuiMessageBox::run(this->GetHWND(),_T("提示"),_T("最多能选择一个文件播放"),_T("确定"));
			}
			else if(lc_list_click_file_lef.front().filesize<0)
			{
				TxDuiMessageBox::run(this->GetHWND(),_T("提示"),_T("不能选择文件夹\n只能选择文件\n"),_T("确定"));
			}
			else
			{
				TxBlobString lc_remote_file((LPCTSTR)this->p_Edit_complex_file_path->GetText());
				lc_remote_file.replaceSelf('\\','/');
				if(lc_remote_file.backChar()!='\\') lc_remote_file.append("\\");
				lc_remote_file.append(lc_list_click_file_lef.front().filename);
				CMainFrameDlg *pwnd=dynamic_cast<CMainFrameDlg*>(this->pMainFrameDlgBasic);
				assert(pwnd!=NULL);
				if(pwnd!=NULL)
					pwnd->getPlayVideoDuiDlgPtr()->addRemoteVideoPlayer(this->sServerIp.c_str(),this->iServerPort,lc_remote_file.c_str());
			}
		}
		break;
	default: 
		assert(0);
		break;
	}
}

int CBrowseRemoteDuiDlg::getAllListFileByDir(std::list<TxBlobString> *_outFileUri,CDbmsClientSessionDll *_pSession,const char* _remote_dir)
{
	int iInitPathPos=-1;
	TxBlobString lc_remote_file(_remote_dir);
	lc_remote_file.replaceSelf('\\','/');
	if(lc_remote_file.backChar()!='\\') lc_remote_file.append("\\");
	std::list<TxBlobString> mLcRecordDir;
	mLcRecordDir.push_back(lc_remote_file);
	while(mLcRecordDir.size()>0)
	{
		struct _tagCbInfo
		{
			std::list<TxBlobString> *ret;
			std::list<TxBlobString> *pRecordDir;
			CBrowseRemoteDuiDlg *pParentThis;
			CDbmsClientSessionDll *pSession;
			int *pInitPathPos;
			static void CALLBACK _staticFindRemoteFile(void *_userdata,const void *_data)
			{
				_tagCbInfo *pThis=(_tagCbInfo*)_userdata;
				const tagFindRemotFileRespondData *lc_data=(const tagFindRemotFileRespondData*)_data;
				if(lc_data->iCurRow>=0)
				{
					if((*pThis->pInitPathPos)<0)
						*pThis->pInitPathPos=(int)::strlen(lc_data->strFilePath);
					if(::strcmp(lc_data->filename,"..")!=0&&::strcmp(lc_data->filename,".")!=0)
					{
						if(lc_data->filesize>=0) pThis->ret->push_back(TxBlobString(lc_data->strFilePath)+lc_data->filename);
						else pThis->pRecordDir->push_back(TxBlobString(lc_data->strFilePath)+lc_data->filename+"\\");
					}
				}
				else if(lc_data->strErrorText!=NULL)
				{
					pThis->pParentThis->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(lc_data->strErrorText);
				}
			}
		} m_lc_tem;
		m_lc_tem.pParentThis=this;
		m_lc_tem.ret=_outFileUri;
		m_lc_tem.pSession=_pSession;
		m_lc_tem.pRecordDir=&mLcRecordDir;
		m_lc_tem.pInitPathPos=&iInitPathPos;
		TxBlobString lc_file_uri=mLcRecordDir.front();
		mLcRecordDir.pop_front();
		_pSession->executeFindRemoteFile(lc_file_uri.c_str(),NULL,6*1000,&m_lc_tem,&_tagCbInfo::_staticFindRemoteFile);
	}
	return iInitPathPos;
}

void CBrowseRemoteDuiDlg::myClickDownLoadOp()
{
	CString lc_dir=TxSelectFolderDialog::run(_T("下载"),this->csOpenFolderInitPath,this->GetHWND());
	if(lc_dir.GetLength()>0)
	{
		this->csOpenFolderInitPath=lc_dir;
		TxBlobString lc_remote_file((LPCTSTR)this->p_Edit_complex_file_path->GetText());
		lc_remote_file.replaceSelf('\\','/');
		if(lc_remote_file.backChar()!='\\') lc_remote_file.append("\\");
		std::list<CRemoteFileFileContentListUI::tagFileInfo> lc_list_click_file_lef=this->m_RemoteFileFileContentListUI.getSelectListFileInfo();
		for(std::list<CRemoteFileFileContentListUI::tagFileInfo>::iterator iter=lc_list_click_file_lef.begin();iter!=lc_list_click_file_lef.end();iter++)
		{
			TxBlobString lc_name=iter->filename;
			if(iter->filesize>=0)
			{//文件名
				this->mDownUpLoadDuiDlg.addDownLoadTask(this->sServerIp.c_str(),this->iServerPort,
					(LPCTSTR)(lc_dir+lc_name.c_str()),(lc_remote_file+lc_name).c_str());
			}
			else
			{//文件夹
				std::list<TxBlobString> lc_uri;
				int lc_init_dir=0;
				{
					TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> spLcAutoDlg(this->pMainFrameDlgBasic->getAutoWaitDuiDialog(_T("正在查询远程文件。")));
					CDbmsClientSessionDll mLcSession(this->sServerIp.c_str(),this->iServerPort);
					lc_init_dir=this->getAllListFileByDir(&lc_uri,&mLcSession,(lc_remote_file+iter->filename).c_str());
				}
				for(std::list<TxBlobString>::iterator iter_2=lc_uri.begin();iter_2!=lc_uri.end();iter_2++)
				{
					this->mDownUpLoadDuiDlg.addDownLoadTask(this->sServerIp.c_str(),this->iServerPort,
						(LPCTSTR)(lc_dir+lc_name.c_str()+_T("\\")+(iter_2->c_str()+lc_init_dir)),iter_2->c_str());
				}
			}
		}
		this->showDownUpLoadTaskWnd();
	}
}

void CBrowseRemoteDuiDlg::gTSqliteCallBackDllFuncFindFile(void *_userdata,const void *_data)
{
	CBrowseRemoteDuiDlg *pThis=(CBrowseRemoteDuiDlg*)_userdata;
	const tagFindRemotFileRespondData *lc_data=(const tagFindRemotFileRespondData*)_data;
	if(lc_data->iCurRow>=0)
	{
		if(lc_data->iCurRow==0)
		{
			pThis->p_Edit_complex_file_path->SetText(lc_data->strFilePath);
		}
		pThis->m_RemoteFileFileContentListUI.addListElem(pThis->p_list_file_content,
			lc_data->filename,
			lc_data->filesize,
			lc_data->time,
			lc_data->permission);
	}
	else
	{
		pThis->pMainFrameDlgBasic->clearAutoWaitDuiDialog();
		TxDuiMessageBox::run(pThis->GetHWND(),_T("错误"),lc_data->strErrorText,_T("确定"));
	}
}

void CBrowseRemoteDuiDlg::gTSqliteCallBackDllFuncDeleteFile(void *_userdata,const void *_data)
{
	tagBrowseRemoteDeleteFileCallBackInfo *parg=(tagBrowseRemoteDeleteFileCallBackInfo*)_userdata;
	const char *pLcErrorText=(const char*)_data;
	if(pLcErrorText!=NULL)
		parg->sErrorText.assign(pLcErrorText);
}

void CBrowseRemoteDuiDlg::myDoMessageEvent(HDROP _hDrop)
{
	CPoint ptPoint;
	if(::GetCursorPos(&ptPoint))
	{
		CRect lc_rect=this->p_list_file_content->GetPos();
		CPoint lcTL=lc_rect.TopLeft();
		CPoint lcBR=lc_rect.BottomRight();
		if(::ClientToScreen(*this,&lcTL)&&::ClientToScreen(*this,&lcBR))
		{
			if(CRect(lcTL,lcBR).PtInRect(ptPoint))
			{
				std::list<std::string> lc_list_string;
				CMainFrameDlgBasic::getDropFileListName(&lc_list_string,_hDrop);
				DragFinish(_hDrop);
				if(lc_list_string.size()>0)
					this->dealDropFile(lc_list_string);
			}
		}
	}
}

void CBrowseRemoteDuiDlg::dealDropFile(const std::list<std::string> &_list_string)
{
	int jkl=0;
}

void CBrowseRemoteDuiDlg::showDownUpLoadTaskWnd()
{
	if(!::IsWindowVisible(this->mDownUpLoadDuiDlg.GetHWND()))
	{
		this->mDownUpLoadDuiDlg.ShowWindow(true);
		this->mDownUpLoadDuiDlg.CenterWindow();
	}
}

