#pragma once

#include <Windows.h>
#include <atlstr.h>

class TxSelectFileDialog
{
public:
	static CString getOpenFileName(HWND _hwndOwner,LPCTSTR _lpstrFilter)
	{
		CString res;
		TCHAR szFile[(FILENAME_MAX|MAX_PATH)+8]={0};
		OPENFILENAME lc_ofn={0};
		lc_ofn.lStructSize = sizeof(lc_ofn);
		lc_ofn.hwndOwner = _hwndOwner;
		lc_ofn.lpstrFile = szFile;
		lc_ofn.lpstrFile[0] = 0;
		lc_ofn.nMaxFile = sizeof(szFile);
		lc_ofn.lpstrFilter = _lpstrFilter;
		lc_ofn.nFilterIndex = 1;
		lc_ofn.lpstrFileTitle = NULL;
		lc_ofn.nMaxFileTitle = 0;
		lc_ofn.lpstrInitialDir = NULL;
		lc_ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		if (::GetOpenFileName(&lc_ofn))
		{
			res=szFile;
		}
		return res;
	}
};
