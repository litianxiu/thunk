#pragma once

#include "stdafx.h"
#include "CMainFrameDlgBasic.hpp"
#include "../gui/CDuiFfmpegPlayWndBasic.h"
#include "CCustomFileHandleRealize.h"

class CPlayVideoDuiDlg: public DuiLib::CWindowWnd
{
public:
	class CPlayVideoUnitWndBase
	{
	public :
		virtual void vfMoveWindow(int _x,int _y,int _w,int _h)=0;
		virtual void vfSetVisible(bool _bShow)=0;
	};
private:
	std::list<TxCppPlatform::shared_ptr<CPlayVideoUnitWndBase>> mListPlayVideoWnd;
	CMainFrameDlgBasic *const pMainFrameDlgBasic;
private:
	void _refresh_ui_size_event_();
public :
	CPlayVideoDuiDlg(CMainFrameDlgBasic *_pMainFrameDlgBasic);
	virtual LPCTSTR GetWindowClassName() const;
	virtual LRESULT HandleMessage(UINT _uMsg,WPARAM _wParam,LPARAM _lParam);
	void myDoPlayDropEvent(HDROP _hDrop);
	void addRemoteVideoPlayer(const char *_ip,int _port,const char *_strRemoteFile);
	void addLocalVideoPlayer(const char *_strFile);
	void showTabWindow(bool _bShow);
};
