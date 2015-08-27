#include "stdafx.h"
#include "CPlayVideoDuiDlg.h"

CPlayVideoDuiDlg::CPlayVideoDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic)
	:pMainFrameDlgBasic(_pMainFrameDlgBasic)
{
}

CPlayVideoDuiDlg::~CPlayVideoDuiDlg()
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
	case WM_CREATE:
		this->_addVideoPlayer__(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile>());
		this->showTabWindow(false);
		break;
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

void CPlayVideoDuiDlg::_addVideoPlayer__(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> _spReadFile)
{
	class T_LW_R: public CPlayVideoUnitWndBase, public CDuiFfmpegPlayWndManager::IThreadEvent
	{
	private:
		CDuiFfmpegPlayWndManager m_DuiFfmpegPlayWndManager;
		CPlayVideoDuiDlg * pParent;
	public:
		T_LW_R(CPlayVideoDuiDlg *_pParent,TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> &_spReadFile)
			:m_DuiFfmpegPlayWndManager(_pParent->GetHWND(),this)
		{
			this->pParent=_pParent;
			if(_spReadFile)
				m_DuiFfmpegPlayWndManager.start(_spReadFile);
		}
		virtual void vfMoveWindow(int _x,int _y,int _w,int _h)
		{
			this->m_DuiFfmpegPlayWndManager.moveWindow(_x,_y,_w,_h);
		}
		virtual void vfSetVisible(bool _bShow)
		{
			this->m_DuiFfmpegPlayWndManager.showWindowVisible(_bShow);
		}
		virtual void vfDestroyWnd()
		{
			this->m_DuiFfmpegPlayWndManager.stop(false);
		}
		virtual void vfStartPlayEvent(bool _bGood)
		{
			if(!_bGood)
			{
				this->pParent->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(_T("视频播放失败"));
			}
		}
	};
	T_LW_R *lc_p_wd=new T_LW_R(this,_spReadFile);
	std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>>::iterator iter=this->mListPlayVideoWnd.begin();
	std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>>::iterator iter_end=this->mListPlayVideoWnd.end();
	for(;iter!=iter_end;iter++)
		(*iter)->vfDestroyWnd();
	this->mListPlayVideoWnd.clear();
	this->mListPlayVideoWnd.push_back(TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>(lc_p_wd));
	this->_refresh_ui_size_event_();
}

void CPlayVideoDuiDlg::addLocalVideoPlayer(const char *_strFile)
{
	class T_LocalFile_Read: public CDuiFfmpegPlayWndBasic::IThreadReadFile
	{
	private:
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
	};
	this->_addVideoPlayer__(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile>(new T_LocalFile_Read(_strFile)));
}

void CPlayVideoDuiDlg::addRemoteVideoPlayer(const char *_ip,int _port,const char *_strRemoteFile)
{
	class T_RemoteRead: public CDuiFfmpegPlayWndBasic::IThreadReadFile
	{
	private:
		CRemoteNetworkFileHandle m_RemoteNetworkFileHandle;
	public :
		T_RemoteRead(const char *_ip,int _port,const char *_strRemoteFile)
			:m_RemoteNetworkFileHandle(_ip,_port,_strRemoteFile)
		{
		}
		virtual int vfReadStream(void *_buff,int _size,long long _llFilePos)
		{
			if(_size>0&&_llFilePos>=0)
			{
				TxBlobString lc_data;
				TxBlobString lc_err;
				this->m_RemoteNetworkFileHandle.vf_read(&lc_data,&lc_err,NULL,_llFilePos,_size,4*1000);
				if(_size>lc_data.size())
					_size=lc_data.size();
				::memcpy(_buff,lc_data.data(),_size);
			}
			return _size;
		}
	};
	this->_addVideoPlayer__(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile>(new T_RemoteRead(_ip,_port,_strRemoteFile)));
	this->pMainFrameDlgBasic->vfSelectTabWidget(2);
}
