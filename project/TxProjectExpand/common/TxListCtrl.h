#pragma once

// TxListCtrl

#include <atlimage.h>
#include <functional>

class TxListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(TxListCtrl)

public:
	TxListCtrl();
	virtual ~TxListCtrl();
private:
	//º¯Êý
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	//±äÁ¿
protected:
	DECLARE_MESSAGE_MAP()
	//afx_msg void OnPaint();
	//afx_msg void OnKillFocus(CWnd* pNewWnd);
	//afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
	//afx_msg int  OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	//afx_msg void OnLButtonUp(UINT nFlags,CPoint point );
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnMouseLeave();
};


