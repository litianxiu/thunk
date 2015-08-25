// WaitDialog.cpp : 实现文件
//
#include "stdafx.h"
#include "TxAutoWaitDialog.h"


// MyAutoWaitDlgR_ 对话框
IMPLEMENT_DYNAMIC(MyAutoWaitDlgR_,CDialog)

MyAutoWaitDlgR_::MyAutoWaitDlgR_(UINT _nIDTemplate,TxIocpTcpApi::TxIocpWindowsEvent *_p_event,LPCTSTR _strText)
	:CDialog(_nIDTemplate),p_event(_p_event)
{
	this->mStrText=_strText;
	this->iTimerId=0;
	this->nHitPos=0;
}

BEGIN_MESSAGE_MAP(MyAutoWaitDlgR_, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(e_My_PostMessage, &MyAutoWaitDlgR_::OnMyMessage)
END_MESSAGE_MAP()


// MyAutoWaitDlgR_ message handlers

BOOL MyAutoWaitDlgR_::OnInitDialog()
{
	CDialog::OnInitDialog();

	::SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,::GetWindowLongPtr(GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_LAYERED);
	this->SetLayeredWindowAttributes(0,(int)(255*0.8+0.5),LWA_ALPHA);

	this->mTextWnd.Create(this->mStrText,WS_CHILD|WS_VISIBLE|DT_CENTER,CRect(0,0,1,1),this);
	this->mTextFont.CreateFont(22,      // nHeight
		0,								// nWidth
		0,								// nEscapement
		0,								// nOrientation
		FW_BOLD,						// nWeight
		FALSE,							// bItalic
		FALSE,							// bUnderline
		0,								// cStrikeOut
		ANSI_CHARSET,					// nCharSet
		OUT_DEFAULT_PRECIS,				// nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		DEFAULT_QUALITY,				// nQuality
		DEFAULT_PITCH | FF_SWISS,		// nPitchAndFamily
		_T("Arial"));					// lpszFacename	
	this->mTextWnd.SetFont(&this->mTextFont);
	CPaintDC dc(&this->mTextWnd);
	CFont* def_font = dc.SelectObject(&this->mTextFont);
	CRect lcRealTextRect(0,0,::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN));
	CString sssss=this->mStrText;
	this->nHitPos=0;
	for (int iii=e_hit_max_pos;--iii;)
		sssss+=_T("。");
	dc.DrawText(sssss, &lcRealTextRect, DT_CALCRECT|DT_CENTER|DT_EDITCONTROL|DT_WORDBREAK );
	int i_lc_tag=lcRealTextRect.Height()/2;
	this->MoveWindow(CRect(CPoint(0,0),CSize(i_lc_tag*2+lcRealTextRect.Width(),i_lc_tag*2+lcRealTextRect.Height())));
	this->CenterWindow(NULL);
	this->mTextWnd.MoveWindow(CRect(CPoint(i_lc_tag,i_lc_tag),lcRealTextRect.Size()));
	dc.SelectObject(def_font);

	enum { e_lc_timer_id=1234, };
	this->iTimerId=this->SetTimer(e_lc_timer_id,420,NULL);
	if(this->iTimerId!=e_lc_timer_id)
		this->iTimerId=0;

	this->p_event->setEvent();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void MyAutoWaitDlgR_::myRefreshText_()
{
	CString ss;
	for(int i=this->nHitPos;i--;)
		ss+=_T("。");
	this->mTextWnd.SetWindowText(this->mStrText+ss);
}

void MyAutoWaitDlgR_::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);
	if((int)nIDEvent==(int)this->iTimerId)
	{
		this->myRefreshText_();
		this->nHitPos=(this->nHitPos+1)%e_hit_max_pos;
	}
}

BOOL MyAutoWaitDlgR_::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN )
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void MyAutoWaitDlgR_::OnDestroy()
{
	if(this->iTimerId!=0)
	{
		this->KillTimer(this->iTimerId);
		this->iTimerId=0;
	}
	__super::OnDestroy();
}

LRESULT MyAutoWaitDlgR_::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	assert(lParam==NULL);
	this->mStrText=(LPCTSTR)wParam;
	this->myRefreshText_();
	return S_OK;
}

///////////////////////////////////以下是TxAutoWaitDialog类

void TxAutoWaitDialog::_static_call_back_(void *_arg1,void*)
{
	((TxAutoWaitDialog*)_arg1)->_call_back_();
}

void TxAutoWaitDialog::_call_back_()
{
	MyAutoWaitDlgR_ m_WaitDlg(nTmplDlgId,&this->mEvent,csText);
	this->pWaitDlg=&m_WaitDlg;
	m_WaitDlg.DoModal();
}

TxAutoWaitDialog::TxAutoWaitDialog(UINT _nTmplDlgId,LPCTSTR _strText):nTmplDlgId(_nTmplDlgId)
{
	this->csText=_strText;
	this->bThreadRunning=TRUE;
	mThread.create(_static_call_back_,this,NULL);
	this->mEvent.waitEvent();
}

TxAutoWaitDialog::~TxAutoWaitDialog()
{
	this->bThreadRunning=FALSE;
	this->pWaitDlg->PostMessage(WM_QUIT);
	this->mThread.join();
}

void TxAutoWaitDialog::setText(LPCTSTR _strText)
{
	this->pWaitDlg->SendMessage(MyAutoWaitDlgR_::e_My_PostMessage,(WPARAM)_strText,NULL);
}

