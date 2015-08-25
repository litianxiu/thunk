#pragma once

#include <shlobj.h>
#include <Windows.h>
#include <atlstr.h>

class TxSelectFolderDialog
{
private:
	struct tagCbInfo
	{
		LPCTSTR title;
		LPCTSTR dir;
	};
	static int CALLBACK g_BrowseCallBackFun(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{
		tagCbInfo *pLcCbInfo=(tagCbInfo*)lpData;
		switch(uMsg)
		{
		case BFFM_INITIALIZED:
			if(pLcCbInfo->dir!=NULL)
			{
				::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)pLcCbInfo->dir);
				::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)pLcCbInfo->dir);
			}
			::SetWindowText(hwnd,pLcCbInfo->title);
			break;
		case BFFM_SELCHANGED:
			{
				TCHAR pszPath[MAX_PATH]={0};
				::SHGetPathFromIDList((LPCITEMIDLIST)lParam, pszPath);
				::SendMessage(hwnd,BFFM_SETSELECTION, TRUE, (LPARAM)pszPath);
				::SendMessage(hwnd,BFFM_SETSTATUSTEXT, TRUE, (LPARAM)pszPath);
			}
			break;
		}
		return 0;
	}
public:
	static CString run(LPCTSTR _title,LPCTSTR _init_dir,HWND _hwnd)
	{
		CString ret;
		TCHAR pszPath[MAX_PATH]={0};
		tagCbInfo lc_cb_info={0};
		lc_cb_info.title=_title;
		lc_cb_info.dir=_init_dir;
		BROWSEINFO bi={0};
		bi.hwndOwner      = _hwnd;
		bi.pidlRoot       = NULL;
		bi.pszDisplayName = NULL;
		bi.lpszTitle      = _T("\r\n请选择文件目录：");
		bi.ulFlags        = BIF_USENEWUI|BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT;
		bi.lpfn           = g_BrowseCallBackFun;
		bi.lParam         = (LPARAM)&lc_cb_info;
		bi.iImage         = 0;
		LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
		if(pidl!=NULL)
		{
			if(::SHGetPathFromIDList(pidl, pszPath))
			{
				LPMALLOC lpMalloc=NULL;
				if(S_OK==::SHGetMalloc(&lpMalloc)&&lpMalloc!=NULL)
				{
					lpMalloc->Free(pidl);
					ret=pszPath;
				}
			}
		}
		if(ret.GetLength()>0)
		{
			for(int i=(int)ret.GetLength();i--;)
			{
				if(ret.GetAt(i)==_T('/'))
					ret.SetAt(i,_T('\\'));
			}
			if(ret.GetLength()>0&&ret.GetAt(ret.GetLength()-1)!=_T('\\'))
				ret.Append(_T("\\"));
		}
		return ret;
	}
};

