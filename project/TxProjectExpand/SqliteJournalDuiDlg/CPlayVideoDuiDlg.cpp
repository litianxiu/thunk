#include "stdafx.h"
#include "CPlayVideoDuiDlg.h"

CPlayVideoDuiDlg::CPlayVideoDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic)
	:pMainFrameDlgBasic(_pMainFrameDlgBasic)
{
}

LPCTSTR CPlayVideoDuiDlg::GetWindowClassName() const
{
	return _T("CPlayVideoDuiDlg");
}

LRESULT CPlayVideoDuiDlg::HandleMessage(UINT _uMsg,WPARAM _wParam,LPARAM _lParam)
{
	(void)_uMsg; (void)_wParam; (void)_lParam;
	switch(_uMsg)
	{
	case WM_MOVE:
	case WM_SIZE:
		this->_refresh_ui_size_event_();
		break;
	case WM_DROPFILES:
		this->myDoPlayDropEvent((HDROP)_wParam);
		break;
	default:
		break;
	}
	return __super::HandleMessage(_uMsg,_wParam,_lParam);
}

void CPlayVideoDuiDlg::myDoPlayDropEvent(HDROP _hDrop)
{
	std::list<std::string> lc_list_string;
	CMainFrameDlgBasic::getDropFileListName(&lc_list_string,_hDrop);
	DragFinish(_hDrop);
	if(lc_list_string.size()!=1)
	{
		TxDuiMessageBox::run(this->GetHWND(),_T("错误"),_T("只能选择一个文件"),_T("确定"));
	}
	else
	{
		struct _stat64 lc_file_info={0};
		std::string lc_str_filename=lc_list_string.front();
		if(0==::_stat64(lc_str_filename.c_str(),&lc_file_info))
		{
			//文件大小
			if(lc_file_info.st_mode&S_IFDIR)
			{
				TxDuiMessageBox::run(this->GetHWND(),_T("错误"),_T("只能选择文件\n不能选择文件夹\n\n"),_T("确定"));
			}
			else if(lc_file_info.st_size==0)
			{
				TxDuiMessageBox::run(this->GetHWND(),_T("错误"),_T("这是一个空文件"),_T("确定"));
			}
			else if((lc_file_info.st_mode&S_IREAD)==0)
			{
				TxDuiMessageBox::run(this->GetHWND(),_T("错误"),_T("此文件没有可读权限"),_T("确定"));
			}
			else 
			{
				this->addLocalVideoPlayer(lc_str_filename.c_str());
			}
		}
	}
}

void CPlayVideoDuiDlg::_refresh_ui_size_event_()
{
	RECT rrr={0};
	BOOL lc_b=::GetWindowRect(this->GetHWND(),&rrr);
	(void)lc_b;
	assert(lc_b);
	if(lc_b)
	{
		std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>>::iterator iter=this->mListPlayVideoWnd.begin();
		std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>>::iterator iter_end=this->mListPlayVideoWnd.end();
		int posx1=0;
		for(;iter!=iter_end;iter++)
		{
			int posx2=posx1+(int)(rrr.right-rrr.left)/(int)this->mListPlayVideoWnd.size();
			(*iter)->vfMoveWindow(posx1,0,posx2-posx1,(int)(rrr.bottom-rrr.top));
			posx1=posx2;
			//(*iter)->vfMoveWindow(rrr.left,rrr.top,(int)(rrr.right-rrr.left),(int)(rrr.bottom-rrr.top));
			//(*iter)->vfMoveWindow(rrr.left,rrr.top,rrr.right,rrr.bottom);
		}
	}
}

void CPlayVideoDuiDlg::showTabWindow(bool _bShow)
{
	std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>>::iterator iter=this->mListPlayVideoWnd.begin();
	std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>>::iterator iter_end=this->mListPlayVideoWnd.end();
	for(;iter!=iter_end;iter++)
	{
		(*iter)->vfSetVisible(_bShow);
	}
	this->ShowWindow(_bShow);
}

void CPlayVideoDuiDlg::addLocalVideoPlayer(const char *_strFile)
{
	class T_LocalFile_Read: public CDuiFfmpegPlayWndBasic::IThreadReadFile
	{
	private:
		//std::string mFileNameExtension;
		HANDLE hFileHandle;
	public :
		T_LocalFile_Read(const char *_file)
		{
			this->hFileHandle=::CreateFileA(
				_file,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL
				);
			assert(this->hFileHandle!=INVALID_HANDLE_VALUE);
			//this->mFileNameExtension.assign(TxBlobString(_file).splitSegment(".").back().c_str());
		}
		~T_LocalFile_Read()
		{
			assert(this->hFileHandle!=INVALID_HANDLE_VALUE);
			if(this->hFileHandle!=INVALID_HANDLE_VALUE)
			{
				::CloseHandle(this->hFileHandle);
				this->hFileHandle=INVALID_HANDLE_VALUE;
			}
		}
		virtual int vfReadStream(void *_buff,int _size,long long _llFilePos)
		{
			assert(this->hFileHandle!=INVALID_HANDLE_VALUE);
			int ret=0;
			if(this->hFileHandle!=INVALID_HANDLE_VALUE)
			{
				DWORD dwCount=0;
				OVERLAPPED lv_ovlpd={0};
				lv_ovlpd.Offset=(DWORD)_llFilePos;
				lv_ovlpd.OffsetHigh=(DWORD)(((unsigned long long)_llFilePos)>>(8*sizeof(DWORD)));
				if(::ReadFile(this->hFileHandle,_buff,(DWORD)_size,&dwCount,&lv_ovlpd)||dwCount>0)
					ret=(int)dwCount;
			}
			return ret;
		}
		//virtual std::string vfFileNameExtension() const
		//{
		//	return mFileNameExtension;
		//}
	};
	class T_LW_R: public CPlayVideoUnitWndBase
	{
	private:
		CDuiFfmpegPlayWndManager m_DuiFfmpegPlayWndManager;
		CPlayVideoDuiDlg *pParent;
	public:
		T_LW_R(CPlayVideoDuiDlg *_pParent,const char *_strLoaclFile)
			:pParent(_pParent),m_DuiFfmpegPlayWndManager(_pParent->GetHWND())
		{
			TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> spReadFile(new T_LocalFile_Read(_strLoaclFile));
			m_DuiFfmpegPlayWndManager.start(spReadFile);
			//this->m_DuiFfmpegPlayWndBasic.create(this->pParent->GetHWND());
		}
		virtual void vfMoveWindow(int _x,int _y,int _w,int _h)
		{
			this->m_DuiFfmpegPlayWndManager.moveWindow(_x,_y,_w,_h);
		}
		virtual void vfSetVisible(bool _bShow)
		{
			this->m_DuiFfmpegPlayWndManager.showWindowVisible(_bShow);
		}
	};
	T_LW_R *lc_p_wd=new T_LW_R(this,_strFile);
	mListPlayVideoWnd.push_back(TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>(lc_p_wd));
	this->_refresh_ui_size_event_();
}

void CPlayVideoDuiDlg::addRemoteVideoPlayer(const char *_ip,int _port,const char *_strRemoteFile)
{
	class T_Read: public CDuiFfmpegPlayWndBasic::IThreadReadFile
	{
	private:
		CRemoteNetworkFileHandle m_RemoteNetworkFileHandle;
		std::string mFileNameExtension;
	public :
		T_Read(const char *_ip,int _port,const char *_strRemoteFile)
			:m_RemoteNetworkFileHandle(_ip,_port,_strRemoteFile)
		{
			this->mFileNameExtension.assign(TxBlobString(_strRemoteFile).splitSegment(".").back().c_str());
		}
		virtual int vfReadStream(void *_buff,int _size,long long _llFilePos)
		{
			return this->vfReadStream(_buff,_size,_llFilePos);
		}
		virtual std::string vfFileNameExtension() const 
		{
			return this->mFileNameExtension;
		}
	};
	class T_R: public CPlayVideoUnitWndBase
	{
	private:
		T_Read m_ReadFile;
		CDuiFfmpegPlayWndBasic m_DuiFfmpegPlayWndBasic;
		CPlayVideoDuiDlg *pParent;
	public:
		T_R(CPlayVideoDuiDlg *_pParent,const char *_ip,int _port,const char *_strRemoteFile)
			:pParent(_pParent),
			m_ReadFile(_ip,_port,_strRemoteFile),
			m_DuiFfmpegPlayWndBasic(&m_ReadFile)
		{
			this->m_DuiFfmpegPlayWndBasic.create(this->pParent->GetHWND());
		}
		virtual void vfMoveWindow(int _x,int _y,int _w,int _h)
		{
			this->m_DuiFfmpegPlayWndBasic.moveWindow(_x,_y,_w,_h);
		}
		virtual void vfSetVisible(bool _bShow)
		{
			this->m_DuiFfmpegPlayWndBasic.showWindowVisible(_bShow);
		}
	};
	T_R *lc_p_wd=new T_R(this,_ip,_port,_strRemoteFile);
	mListPlayVideoWnd.push_back(TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>(lc_p_wd));
	this->_refresh_ui_size_event_();
}
