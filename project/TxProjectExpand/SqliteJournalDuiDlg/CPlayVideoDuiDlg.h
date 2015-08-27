#pragma once

#include "stdafx.h"
#include "CMainFrameDlgBasic.hpp"
#include "CCustomFileHandleRealize.h"
#include "../gui/CDuiFfmpegPlayWndManager.h"

class CPlayVideoDuiDlg: public DuiLib::CWindowWnd
{
public:
	class CPlayVideoUnitWndBase
	{
	public :
		virtual ~CPlayVideoUnitWndBase() { }
		virtual void vfMoveWindow(int _x,int _y,int _w,int _h)=0;
		virtual void vfSetVisible(bool _bShow)=0;
		virtual void vfStartPlay(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> &_spReadFile)=0;
	};
private:
	std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>> mListPlayVideoWnd;
	CMainFrameDlgBasic *const pMainFrameDlgBasic;
private:
	void _refresh_ui_size_event_();
	void _addVideoPlayer__(TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> _spReadFile);
public :
	CPlayVideoDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic);
	~CPlayVideoDuiDlg();
	virtual LPCTSTR GetWindowClassName() const;
	virtual LRESULT HandleMessage(UINT _uMsg,WPARAM _wParam,LPARAM _lParam);
	void myDoPlayDropEvent(HDROP _hDrop);
	void addRemoteVideoPlayer(const char *_ip,int _port,const char *_strRemoteFile);
	void showTabWindow(bool _bShow);
};
