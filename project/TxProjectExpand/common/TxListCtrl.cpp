#include "stdafx.h"
#include "TxListCtrl.h"

// TxListCtrl

IMPLEMENT_DYNAMIC(TxListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(TxListCtrl, CListCtrl)
	//ON_WM_PAINT()
	//ON_WM_TIMER()
	//ON_WM_LBUTTONUP()
	//ON_WM_LBUTTONDOWN()
	//ON_WM_ACTIVATE()
	//ON_WM_MOUSEACTIVATE()
	//ON_WM_KILLFOCUS()
	//ON_WM_LBUTTONDBLCLK()
	//ON_WM_MOUSEMOVE()
	//ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

TxListCtrl::TxListCtrl()
{
}

TxListCtrl::~TxListCtrl()
{
}

BOOL TxListCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_RETURN || pMsg-> wParam == VK_ESCAPE || pMsg->wParam == VK_SPACE)
		{
			CWnd *pWnd = GetParent();
			if(pWnd!=NULL&&pWnd->m_hWnd)
				::PostMessage(pWnd->m_hWnd,WM_KEYDOWN,VK_ESCAPE,0);
			return TRUE;
		}
		break;
	default:
		break;
	}

	return __super::PreTranslateMessage(pMsg);
}

