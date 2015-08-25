#pragma once

#include <UIlib.h>

class CDuiPlayVideoAttrb: public DuiLib::WindowImplBase
{
private:
	BOOL bInitControlUI;
	DuiLib::CVerticalLayoutUI *p_vlyt_file;
	DuiLib::CVerticalLayoutUI *p_vlyt_video;
	DuiLib::CVerticalLayoutUI *p_vlyt_audio;
	DuiLib::CButtonUI *p_btn_wnd_close;
private:
	template<class T>
	void buildSaftWidgetPtr(T **_obj,LPCTSTR _str)
	{
		DuiLib::CControlUI* p_ui=m_PaintManager.FindControl(_str);
		*_obj=NULL;
		if(p_ui!=NULL)
		{
			*_obj=dynamic_cast<T*>(p_ui);
		}
		if((*_obj)==NULL)
			assert(0);
	}
public :
	CDuiPlayVideoAttrb()
	{
		this->bInitControlUI=FALSE;
	}
	virtual LPCTSTR GetWindowClassName() const
	{
		return _T("CDuiPlayVideoAttribute");
	}
	virtual DuiLib::CDuiString GetSkinFolder()
	{
		return DefDuiSkin;
	}
	virtual DuiLib::CDuiString GetSkinFile()
	{
		return _T("dui_play_video_attr.xml");
	}
	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		return __super::HandleMessage(_uMsg,_wParam,_lParam);
	}
	virtual void InitWindow()
	{
		assert(this->bInitControlUI==FALSE);
		if(!this->bInitControlUI)
		{
			this->buildSaftWidgetPtr(&this->p_vlyt_file,_T("vlyt_file"));
			this->buildSaftWidgetPtr(&this->p_vlyt_video,_T("vlyt_video"));
			this->buildSaftWidgetPtr(&this->p_vlyt_audio,_T("vlyt_audio"));
			this->buildSaftWidgetPtr(&this->p_btn_wnd_close,_T("btn_wnd_close"));
			this->bInitControlUI=TRUE;
		}
		return __super::InitWindow();
	}
	virtual void Notify(DuiLib::TNotifyUI& _msg)
	{
		if(_msg.sType.Compare(DUI_MSGTYPE_CLICK)==0)
		{
			if(_msg.pSender==this->p_btn_wnd_close)
			{
				this->Close();
			}
		}
		return __super::Notify(_msg);
	}
};

