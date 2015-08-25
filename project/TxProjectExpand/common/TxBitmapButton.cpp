#include "stdafx.h"
#include "TxBitmapButton.h"

// TxBitmapButton

IMPLEMENT_DYNAMIC(TxBitmapButton, CButton)

BEGIN_MESSAGE_MAP(TxBitmapButton, CButton)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_ACTIVATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

TxBitmapButton::TxBitmapButton()
{
	this->curImage=NULL;
	this->bChangeCurImage=FALSE;
	this->nTimerIDEvent=0;
	this->bMouseEnter=FALSE;
	this->iFontHeight=12;
	this->bFontBold=FALSE;
	this->textNormalColor=0;
	this->textFocusColor=0;
	this->bEnableCheckable=FALSE;
	this->bBtnCheckMark=FALSE;
}

TxBitmapButton::~TxBitmapButton()
{
}

void TxBitmapButton::_static_load_image__(CImage *_outImg,LPCTSTR _imgPath)
{
	if(_imgPath!=NULL)
	{
		HRESULT lc_res=_outImg->Load(_imgPath);
		assert(lc_res==S_OK);
		(void)lc_res;
	}
	else
	{
		_outImg->Destroy();
	}
}

void TxBitmapButton::_refresh_icon_status_(EnumTypeStatus _status)
{
	switch(_status)
	{
	case eStatus_Focus:
		if(this->curImage==&this->imgDisable)
		{
			this->EnableWindow(TRUE);
		}
		if(this->curImage!=&this->imgFocus)
		{
			this->curImage=&this->imgFocus;
			this->bChangeCurImage=TRUE;
			this->EnableWindow(TRUE);
			this->Invalidate();
		}
		break;
	case eStatus_Disable:
		if(this->curImage!=&this->imgDisable)
		{
			this->curImage=&this->imgDisable;
			this->bChangeCurImage=TRUE;
			this->EnableWindow(FALSE);
			this->Invalidate();
		}
		break;
	case eStatus_Normal:
		{
			CRect rect;
			this->GetClientRect(&rect);
			CPoint pos;
			if(GetCursorPos(&pos))
			{
				GetDesktopWindow()->MapWindowPoints(this,&pos,1);
				if(this->curImage==&this->imgDisable)
				{
					this->EnableWindow(TRUE);
				}
				if(rect.PtInRect(pos))
				{
					if(this->curImage!=&this->imgOver)
					{
						this->curImage=&this->imgOver;
						this->bChangeCurImage=TRUE;
						this->Invalidate();
					}
				}
				else
				{
					if(this->curImage!=&this->imgNormal)
					{
						this->curImage=&this->imgNormal;
						this->bChangeCurImage=TRUE;
						this->Invalidate();
					}
				}
			}
		}
		break;
	default:
		if(this->curImage==&this->imgNormal||this->curImage==&this->imgOver)
		{
			CRect rect;
			this->GetClientRect(&rect);
			CPoint pos;
			if(GetCursorPos(&pos))
			{
				GetDesktopWindow()->MapWindowPoints(this,&pos,1);
				if(rect.PtInRect(pos))
				{
					if(this->curImage==&this->imgNormal)
					{
						this->curImage=&this->imgOver;
						this->bChangeCurImage=TRUE;
						this->Invalidate();
					}
				}
				else
				{
					if(this->curImage==&this->imgOver||this->curImage==&this->imgFocus)
					{
						this->curImage=&this->imgNormal;
						this->bChangeCurImage=TRUE;
						this->Invalidate();
					}
				}
			}
		}
		break;
	}
}

void TxBitmapButton::initialize(const TxBitmapButton::tagCreateInfo &_args)
{
	this->bBtnCheckMark=FALSE;
	this->bMouseEnter=FALSE;
	this->csTooltipText=_T("");
	if(this->curImage==NULL)
	{
		_static_load_image__(&this->imgNormal,_args.imgNormal);
		_static_load_image__(&this->imgOver,_args.imgOver);
		_static_load_image__(&this->imgFocus,_args.imgFocus);
		_static_load_image__(&this->imgDisable,_args.imgDisable);
		this->bChangeCurImage=TRUE;
		this->curImage=&this->imgNormal;
		//this->nTimerIDEvent=this->SetTimer(1234,200,NULL);

		this->csButtonText=_args.csButtonText;
		this->iFontHeight=_args.iFontHeight;
		this->bFontBold=_args.bFontBold;
		this->bEnableCheckable=_args.bEnableCheckable;
		this->textNormalColor=_args.textNormalColor;
		this->textFocusColor=_args.textFocusColor;

		if(!_args.csTooltipText.IsEmpty())
		{
			EnableToolTips(TRUE);
			m_TooltipCtrl.Create(this);
			m_TooltipCtrl.AddTool(this,_args.csTooltipText);
			//m_TooltipCtrl.SetDelayTime(TTDT_INITIAL, 1); // 设置1ms后显示
			//m_TooltipCtrl.SetDelayTime(TTDT_RESHOW, 1); // 当鼠标移到另外一个控件上时设置1ms后显示
			m_TooltipCtrl.SetDelayTime(TTDT_AUTOPOP,1500); // 设置3s后自动隐藏，后台应该是调用了Pop()方法
			m_TooltipCtrl.Activate(TRUE);
			m_TooltipCtrl.SetWindowPos(this, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			this->csTooltipText=_args.csTooltipText;
		}
	}
	else
		assert(0);
}

void TxBitmapButton::setButtonStatus(TxBitmapButton::EnumTypeStatus _eStatus)
{
	if(this->bEnableCheckable)
	{
		if(_eStatus==eStatus_Focus) this->bBtnCheckMark=TRUE;
		else if(_eStatus==eStatus_Normal) this->bBtnCheckMark=FALSE;
	}
	if(_eStatus!=eStatus_Null)
		this->_refresh_icon_status_(_eStatus);
}

void TxBitmapButton::setButtonText(LPCTSTR _text,TxBitmapButton::EnumTypeStatus _eStatus)
{
	this->bChangeCurImage=TRUE;
	this->csButtonText=_text;
	this->setButtonStatus(_eStatus);
	this->Invalidate();
}

void TxBitmapButton::OnPaint()
{
	CPaintDC dc(this);
	CRect mLcNowRect;
	GetWindowRect(&mLcNowRect);
	if(this->bChangeCurImage
		||this->mOldRect.left!=mLcNowRect.left
		||this->mOldRect.right!=mLcNowRect.right
		||this->mOldRect.top!=mLcNowRect.top
		||this->mOldRect.bottom!=mLcNowRect.bottom
		)
	{
		this->mOldRect=mLcNowRect;
		this->bChangeCurImage=FALSE;
		if(this->curImage!=NULL&&!this->curImage->IsNull())//显示图片
		{
			CRect rt;
			GetWindowRect(&rt);
			this->curImage->Draw(dc.m_hDC,0,0,rt.Width(),rt.Height(),
				0,0,this->curImage->GetWidth(),this->curImage->GetHeight());
		}
		{//显示文字
			CFont mLcTextFont;
			BOOL bLcFont=mLcTextFont.CreateFont(
				this->iFontHeight,					// nHeight
				0,									// nWidth
				0,									// nEscapement
				0,									// nOrientation
				this->bFontBold?FW_BOLD:FW_NORMAL,	// nWeight
				FALSE,								// bItalic
				FALSE,								// bUnderline
				0,									// cStrikeOut
				ANSI_CHARSET,						// nCharSet
				OUT_DEFAULT_PRECIS,					// nOutPrecision
				CLIP_DEFAULT_PRECIS,				// nClipPrecision
				DEFAULT_QUALITY,					// nQuality
				DEFAULT_PITCH | FF_SWISS,			// nPitchAndFamily
				_T("Arial"));						// lpszFacename
			(void)bLcFont;
			assert(bLcFont);
			CFont* def_font = dc.SelectObject(&mLcTextFont);
			CRect lc_wnd_rect;
			GetClientRect(&lc_wnd_rect);
			CRect lc_text_rect(lc_wnd_rect);
			//设置文字背景为透明
			dc.SetBkMode(TRANSPARENT);
			dc.SetTextColor((this->curImage==&this->imgFocus)?this->textFocusColor:this->textNormalColor);
			//计算要显示的字符串高度
			dc.DrawText(this->csButtonText, &lc_text_rect, DT_CALCRECT|DT_CENTER|DT_EDITCONTROL|DT_WORDBREAK );
			//垂直、水平居中显示
			lc_wnd_rect.top += (lc_wnd_rect.Height() - lc_text_rect.Height())/2;
			dc.DrawText(this->csButtonText, &lc_wnd_rect, DT_CENTER|DT_EDITCONTROL|DT_WORDBREAK );
			dc.SelectObject(def_font);
			mLcTextFont.DeleteObject();
		}
	}
}

void TxBitmapButton::LineUp(CWnd *pFront,int nXSpace,int nYSpace)
{
	CRect rtFront,rtThis;
	pFront->GetWindowRect(&rtFront);
	CWnd* pParent = GetParent();
	if(pParent)
	{
		GetWindowRect(&rtThis);
		if(nXSpace >= 0)
		{  
			rtThis.MoveToXY(rtFront.right+ nXSpace,rtFront.top);
			pParent->ScreenToClient(&rtThis);
			MoveWindow(&rtThis);
		}

		if(nYSpace >= 0)
		{
			rtThis.MoveToXY(rtFront.left,rtFront.bottom + nYSpace);
			pParent->ScreenToClient(&rtThis);
			MoveWindow(&rtThis);
		}
	}
}

void TxBitmapButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!this->bMouseEnter)
	{
		this->bMouseEnter=TRUE;
		this->_my_mouse_inter_event();
	}
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE;
	_TrackMouseEvent(&tme);

	__super::OnMouseMove(nFlags, point);
}

void TxBitmapButton::_my_mouse_inter_event()
{
	this->_refresh_icon_status_(eStatus_Null);
}

void TxBitmapButton::_my_mouse_leave_event()
{
	if(this->curImage!=&this->imgDisable&&(this->curImage!=&this->imgFocus||!this->bEnableCheckable))
		this->_refresh_icon_status_(eStatus_Normal);
}

void TxBitmapButton::_my_mouse_down_event()
{
	if(this->bEnableCheckable)
	{
		if(this->bBtnCheckMark)
			this->_refresh_icon_status_(eStatus_Normal);
		else
			this->_refresh_icon_status_(eStatus_Focus);
		this->bBtnCheckMark=!this->bBtnCheckMark;
	}
	else
	{
		this->bBtnCheckMark=FALSE;
		this->_refresh_icon_status_(eStatus_Focus);
	}
	if (this->GetParent()!=NULL&&this->cbLButtonDownEvent)
	{
		this->cbLButtonDownEvent(!!this->bBtnCheckMark);
	}
}

void TxBitmapButton::_my_mouse_up_event()
{
	if(!this->bEnableCheckable)
		this->_refresh_icon_status_(eStatus_Normal);
}

void TxBitmapButton::OnMouseLeave()
{
	if(this->bMouseEnter)
	{
		this->bMouseEnter=FALSE;
		this->_my_mouse_leave_event();
	}
	__super::OnMouseLeave();
}

void TxBitmapButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->_my_mouse_down_event();
}

void TxBitmapButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	this->_my_mouse_down_event();
}

void  TxBitmapButton::OnLButtonUp(UINT nFlags,CPoint point )
{
	this->_my_mouse_up_event();
}

void TxBitmapButton::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);

	CRect rect;
	this->GetClientRect(&rect);
	CPoint pos;
	if(GetCursorPos(&pos))
	{
		GetDesktopWindow()->MapWindowPoints(this,&pos,1);
		if(rect.PtInRect(pos))
		{
			if(!this->bMouseEnter)
			{
				this->bMouseEnter=TRUE;
				this->_my_mouse_inter_event();
			}
		}
		else
		{
			if(this->bMouseEnter)
			{
				this->bMouseEnter=FALSE;
				this->_my_mouse_leave_event();
			}
		}
	}
}

void TxBitmapButton::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);
}

void TxBitmapButton::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized)
{
	__super::OnActivate(nState,pWndOther,bMinimized);
}

int TxBitmapButton::OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message)
{	
	__super::OnMouseActivate(pDesktopWnd,nHitTest,message);
	return 0;
}

BOOL TxBitmapButton::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_RETURN || pMsg-> wParam == VK_ESCAPE || pMsg->wParam == VK_SPACE)
		{
			CWnd *pWnd = GetParent();
			if(pWnd->m_hWnd)
				::PostMessage(pWnd->m_hWnd,WM_KEYDOWN,VK_ESCAPE,0);
			return TRUE;
		}
		break;
	default:
		break;
	}

	if(!this->csTooltipText.IsEmpty())
	{
		if(this->bMouseEnter)
		{
			CString cs;
			this->m_TooltipCtrl.GetText(cs,this);
			if(cs!=this->csTooltipText)
				this->m_TooltipCtrl.UpdateTipText(this->csTooltipText,this);
			this->m_TooltipCtrl.RelayEvent(pMsg);
		}
		else
			this->m_TooltipCtrl.Pop();
	}

	return __super::PreTranslateMessage(pMsg);
}

