#pragma once
#include <atlimage.h>
#include <vector>
#include "../common/TxIocpTcpApi.hpp"


// MyAutoWaitDlgR_ ¶Ô»°¿ò

class MyAutoWaitDlgR_ : public CDialog
{
	DECLARE_DYNAMIC(MyAutoWaitDlgR_)
public:
	MyAutoWaitDlgR_(UINT _nIDTemplate,TxIocpTcpApi::TxIocpWindowsEvent *_p_event,LPCTSTR _strText);   // standard constructor
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	void myRefreshText_();
private:
	TxIocpTcpApi::TxIocpWindowsEvent *const p_event;
	CStatic mTextWnd;
	CString mStrText;
	CFont mTextFont;
	int nHitPos;
	enum { e_hit_max_pos=6, };
	int iTimerId;
public:
	enum { _e_start_temp_=WM_APP+64,
		e_My_PostMessage,
	};
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
};

class TxAutoWaitDialog
{
private:
	TxIocpTcpApi::TxIocpThread mThread;
	const UINT nTmplDlgId;
	BOOL bThreadRunning;
	MyAutoWaitDlgR_ *pWaitDlg;
	CString csText;
	TxIocpTcpApi::TxIocpWindowsEvent mEvent;
private:
	static void _static_call_back_(void *_arg1,void*);
	void _call_back_();
public :
	TxAutoWaitDialog(UINT _nTmplDlgId,LPCTSTR _strText);
	void setText(LPCTSTR _strText);
	~TxAutoWaitDialog();
};

