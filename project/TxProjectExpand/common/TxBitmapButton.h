#pragma once

// TxBitmapButton

#include <atlimage.h>
#include <functional>

class TxBitmapButton : public CButton
{
	DECLARE_DYNAMIC(TxBitmapButton)

public:
	enum EnumTypeStatus
	{
		eStatus_Null=0,
		eStatus_Normal,
		eStatus_Focus,
		eStatus_Disable,
	} ;
	struct tagCreateInfo
	{
		CString imgNormal;
		CString imgOver;
		CString imgFocus;
		CString imgDisable;
		CString csButtonText;
		CString csTooltipText;
		int iFontHeight;
		BOOL bFontBold;
		BOOL bEnableCheckable;
		COLORREF textNormalColor;
		COLORREF textFocusColor;
		tagCreateInfo(LPCTSTR _imgNormal,LPCTSTR _imgOver,LPCTSTR _imgFocus,LPCTSTR _imgDisable,
			LPCTSTR _csButtonText,LPCTSTR _csTooltipText,int _iFontHeight,BOOL _bFontBold,BOOL _bEnableCheckable,
			COLORREF _textNormalColor,COLORREF _textFocusColor)
		{
			this->imgNormal=(_imgNormal==NULL?_T(""):_imgNormal);
			this->imgOver=(_imgOver==NULL?_T(""):_imgOver);
			this->imgFocus=(_imgFocus==NULL?_T(""):_imgFocus);
			this->imgDisable=(_imgDisable==NULL?_T(""):_imgDisable);
			this->csButtonText=(_csButtonText==NULL?_T(""):_csButtonText);
			this->csTooltipText=(_csTooltipText==NULL?_T(""):_csTooltipText);
			this->iFontHeight=_iFontHeight;
			this->bFontBold=_bFontBold;
			this->textNormalColor=_textNormalColor;
			this->textFocusColor=_textFocusColor;
			this->bEnableCheckable=_bEnableCheckable;
		}
	};
public:
	TxBitmapButton();
	virtual ~TxBitmapButton();
	void initialize(const TxBitmapButton::tagCreateInfo &_args);
	void setButtonStatus(TxBitmapButton::EnumTypeStatus _eStatus);
	void setButtonText(LPCTSTR _text,TxBitmapButton::EnumTypeStatus _eStatus=eStatus_Null);
private:
	static void _static_load_image__(CImage *_outImg,LPCTSTR _imgPath);
	void _refresh_icon_status_(EnumTypeStatus _status);
	void _my_mouse_inter_event();
	void _my_mouse_leave_event();
	void _my_mouse_down_event();
	void _my_mouse_up_event();
protected:
	//功能：把窗口排到指定窗口的后面或下面
	//pFront:前面一个窗口的指针
	//nXSpace:x方向需要留出的间隔，-1指定非x方向排列
	//nySpace:y方向需要留出的间隔，-1指定非y方向排列
	virtual void LineUp(CWnd *pFront,int nXSpace,int nYSpace);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	std::tr1::function<void(bool)> cbLButtonDownEvent;
	CToolTipCtrl m_TooltipCtrl;
private:
	CImage imgNormal;
	CImage imgOver;
	CImage imgFocus;
	CImage imgDisable;
	CString csButtonText;
	CString csTooltipText;
	int iFontHeight;
	BOOL bFontBold;
	BOOL bEnableCheckable;
	COLORREF textNormalColor;
	COLORREF textFocusColor;

	BOOL bMouseEnter;
	BOOL bBtnCheckMark;
	CImage *curImage;
	BOOL bChangeCurImage;
	UINT nTimerIDEvent;
	CRect mOldRect;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	afx_msg void OnLButtonUp(UINT nFlags,CPoint point );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};


