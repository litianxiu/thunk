#pragma once

#include <UIlib.h>

class CDuiPlayVideoFfmpegAttrb: public DuiLib::WindowImplBase
{
public :
	BOOL bInitControlUI;

	DuiLib::CLabelUI *p_label_file_FengZhuangGeShi;
	DuiLib::CLabelUI *p_label_file_BiTeLv;
	DuiLib::CLabelUI *p_label_file_ShiChang;
	DuiLib::CRichEditUI *p_re_file_MetaData;

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
			this->buildSaftWidgetPtr(&this->p_label_file_FengZhuangGeShi,_T("label_file_FengZhuangGeShi"));
			this->buildSaftWidgetPtr(&this->p_label_file_BiTeLv,_T("label_file_BiTeLv"));
			this->buildSaftWidgetPtr(&this->p_label_file_ShiChang,_T("label_file_ShiChang"));
			this->buildSaftWidgetPtr(&this->p_re_file_MetaData,_T("re_file_MetaData"));
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
	void setParameter(AVFormatContext *_pFormatCtx)
	{
		int lc_iVideoIdx=-1;
		int lc_iAudioIdx=-1;
		for(int i=0;i<(int)_pFormatCtx->nb_streams;i++)
		{
			switch(_pFormatCtx->streams[i]->codec->codec_type)
			{
			case AVMEDIA_TYPE_VIDEO:
				lc_iVideoIdx=i;
				break;
			case AVMEDIA_TYPE_AUDIO:
				lc_iAudioIdx=i;
				break;
			default:
				assert(0);
				break;
			}
		}
		CString lc_cs,lc_cs_default_text=_T("未知");
		this->p_label_file_FengZhuangGeShi->SetText(lc_cs_default_text);
		this->p_label_file_BiTeLv->SetText(lc_cs_default_text);
		this->p_label_file_ShiChang->SetText(lc_cs_default_text);
		this->p_re_file_MetaData->SetText(lc_cs_default_text);
		this->p_label_video_BianMaFangShi->SetText(lc_cs_default_text);
		this->p_label_video_ZhenLv->SetText(lc_cs_default_text);
		this->p_label_video_HuaMianDaXiao->SetText(lc_cs_default_text);
		this->p_label_audio_CaiYangLv->SetText(lc_cs_default_text);
		this->p_label_audio_BianMaFangShi->SetText(lc_cs_default_text);
		this->p_label_audio_ShengDaoShu->SetText(lc_cs_default_text);
		{
			if(_pFormatCtx->iformat!=NULL&&_pFormatCtx->iformat->long_name!=NULL)
				this->p_label_file_FengZhuangGeShi->SetText(_pFormatCtx->iformat->long_name);
		}
		{
			lc_cs.Format(_T("%5.2fkbps"),(float)(_pFormatCtx->bit_rate/1000.0));
			this->p_label_file_BiTeLv->SetText(lc_cs);
		}
		if(_pFormatCtx->duration>=0)
		{
			int lc_tns,lc_thh,lc_tmm,lc_tss;
			lc_tns  = (int)((_pFormatCtx->duration)/AV_TIME_BASE);
			lc_thh  = (int)(lc_tns/3600);
			lc_tmm  = (int)(lc_tns%3600)/60;
			lc_tss  = (int)(lc_tns%60);
			lc_cs.Format(_T("%02d:%02d:%02d"),(int)lc_thh,(int)lc_tmm,(int)lc_tss);
			this->p_label_file_ShiChang->SetText(lc_cs);
		}
		{
			CString lc_meta;
			CString lc_key;
			CString lc_value;
			AVDictionaryEntry *lc_m = NULL;
			while(lc_m=av_dict_get(_pFormatCtx->metadata,"",lc_m,AV_DICT_IGNORE_SUFFIX))
			{
				lc_key.Format(_T("%s"),lc_m->key);
				lc_value.Format(_T("%s"),lc_m->value);
				lc_meta.AppendFormat(_T("%s\t:%s\r\n"),lc_key,lc_value);
			}
			this->p_re_file_MetaData->SetText(lc_meta);
		}
		if(lc_iVideoIdx>=0&&_pFormatCtx->streams[lc_iVideoIdx]->codec!=NULL)
		{
			AVStream *pAvStream = _pFormatCtx->streams[lc_iVideoIdx];
			AVCodecContext *pCodecCtx = _pFormatCtx->streams[lc_iVideoIdx]->codec;
				if(pCodecCtx->codec->long_name!=NULL)
				{
					lc_cs.Format(_T("%s"),pCodecCtx->codec->long_name);
					this->p_label_video_BianMaFangShi->SetText(lc_cs);
				}
			{
				lc_cs.Format(_T("%5.2ffps"),(float)((double)pAvStream->r_frame_rate.num/(double)pAvStream->r_frame_rate.den));
				this->p_label_video_ZhenLv->SetText(lc_cs);
			}
			{
				lc_cs.Format(_T("%d x %d"),(int)pCodecCtx->width,(int)pCodecCtx->height);
				this->p_label_video_HuaMianDaXiao->SetText(lc_cs);
			}
		}
		if(lc_iAudioIdx>=0&&_pFormatCtx->streams[lc_iAudioIdx]->codec!=NULL)
		{
			AVStream *pAvStream = _pFormatCtx->streams[lc_iAudioIdx];
			AVCodecContext *pCodecCtx = _pFormatCtx->streams[lc_iAudioIdx]->codec;
			{
				lc_cs.Format(_T("%d"),(int)pCodecCtx->sample_rate);
				this->p_label_audio_CaiYangLv->SetText(lc_cs);
			}
			{
				lc_cs.Format(_T("%s"),pCodecCtx->codec->long_name);
				this->p_label_audio_BianMaFangShi->SetText(lc_cs);
			}
			{
				lc_cs.Format(_T("%d"),(int)pCodecCtx->channels);
				this->p_label_audio_ShengDaoShu->SetText(lc_cs);
			}
		}
	}
};
