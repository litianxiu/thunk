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
	default:
		break;
	}
	return __super::HandleMessage(_uMsg,_wParam,_lParam);
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
		T_LW_R(CPlayVideoDuiDlg *_pParent):m_DuiFfmpegPlayWndManager(_pParent->GetHWND(),this)
		{
			this->pParent=_pParent;
		}
		virtual void vfStartPlay(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> &_spReadFile)
		{
			this->m_DuiFfmpegPlayWndManager.start(_spReadFile);
		}
		virtual void vfMoveWindow(int _x,int _y,int _w,int _h)
		{
			this->m_DuiFfmpegPlayWndManager.moveWindow(_x,_y,_w,_h);
		}
		virtual void vfSetVisible(bool _bShow)
		{
			this->m_DuiFfmpegPlayWndManager.showWindowVisible(_bShow);
		}
		virtual void vfStartPlayEvent(bool _bGood)
		{
			if(!_bGood)
			{
				this->pParent->pMainFrameDlgBasic->getDuiMessageNotifyDlgPtr()->notify(_T("ÊÓÆµ²¥·ÅÊ§°Ü"));
			}
		}
	};
	if(this->mListPlayVideoWnd.size()==0)
		this->mListPlayVideoWnd.push_back(TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>(new T_LW_R(this)));
	if(_spReadFile)
	{
		std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>>::iterator iter=this->mListPlayVideoWnd.begin();
		std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>>::iterator iter_end=this->mListPlayVideoWnd.end();
		for(;iter!=iter_end;iter++)
		{
			(*iter)->vfStartPlay(_spReadFile);
			break;
		}
	}
	this->_refresh_ui_size_event_();
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
			else
			{
				assert(0);
			}
			return _size;
		}
	};
	this->_addVideoPlayer__(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile>(new T_RemoteRead(_ip,_port,_strRemoteFile)));
	this->pMainFrameDlgBasic->vfSelectTabWidget(2);
}
