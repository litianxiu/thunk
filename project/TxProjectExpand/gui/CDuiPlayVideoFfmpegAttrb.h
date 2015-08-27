#pragma once

#include <UIlib.h>

class CDuiPlayVideoFfmpegAttrb: public DuiLib::WindowImplBase
{
public :
	BOOL bInitControlUI;

	DuiLib::CLabelUI *p_label_file_ShuRuLeiXing;
	DuiLib::CLabelUI *p_label_file_FengZhuangGeShi;
	DuiLib::CLabelUI *p_label_file_BiTeLv;
	DuiLib::CLabelUI *p_label_file_ShiChang;
	DuiLib::CRichEditUI *p_re_file_MetaData;

	DuiLib::CLabelUI *p_label_video_ShuChuXiangSuGeShi;
	DuiLib::CLabelUI *p_label_video_BianMaFangShi;
	DuiLib::CLabelUI *p_label_video_ZhenLv;
	DuiLib::CLabelUI *p_label_video_HuaMianDaXiao;

	DuiLib::CLabelUI *p_label_audio_CaiYangLv;
	DuiLib::CLabelUI *p_label_audio_BianMaFangShi;
	DuiLib::CLabelUI *p_label_audio_ShengDaoShu;

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
	CDuiPlayVideoFfmpegAttrb()
	{
		this->bInitControlUI=FALSE;
	}
	virtual LPCTSTR GetWindowClassName() const
	{
		return _T("CDuiPlayVideoFfmpegAttrb");
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
			this->buildSaftWidgetPtr(&this->p_label_file_ShuRuLeiXing,_T("label_file_ShuRuLeiXing"));
			this->buildSaftWidgetPtr(&this->p_label_file_FengZhuangGeShi,_T("label_file_FengZhuangGeShi"));
			this->buildSaftWidgetPtr(&this->p_label_file_BiTeLv,_T("label_file_BiTeLv"));
			this->buildSaftWidgetPtr(&this->p_label_file_ShiChang,_T("label_file_ShiChang"));
			this->buildSaftWidgetPtr(&this->p_re_file_MetaData,_T("re_file_MetaData"));
			this->buildSaftWidgetPtr(&this->p_label_video_ShuChuXiangSuGeShi,_T("label_video_ShuChuXiangSuGeShi"));
			this->buildSaftWidgetPtr(&this->p_label_video_BianMaFangShi,_T("label_video_BianMaFangShi"));
			this->buildSaftWidgetPtr(&this->p_label_video_ZhenLv,_T("label_video_ZhenLv"));
			this->buildSaftWidgetPtr(&this->p_label_video_HuaMianDaXiao,_T("label_video_HuaMianDaXiao"));
			this->buildSaftWidgetPtr(&this->p_label_audio_CaiYangLv,_T("label_audio_CaiYangLv"));
			this->buildSaftWidgetPtr(&this->p_label_audio_BianMaFangShi,_T("label_audio_BianMaFangShi"));
			this->buildSaftWidgetPtr(&this->p_label_audio_ShengDaoShu,_T("label_audio_ShengDaoShu"));
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
	void setParameter(AVFormatContext *_pFormatCtx,int _iVideoIdx,int _iAudioIdx)
	{
	}
};
