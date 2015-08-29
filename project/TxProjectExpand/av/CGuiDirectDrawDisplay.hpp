#pragma once

#include <Windows.h>
#include <string.h>
#include <assert.h>
#include <list>
#include <ddraw.h>
#include <atlstr.h>
#include "CDirectDrawFrameFormat.h"

class CGuiDirectDrawDisplay
{
private:
	DDSURFACEDESC			m_ddsd;
	DDSURFACEDESC			mRecordOrgDdsd;
	HMODULE					hDdrawHodule;
	LPDIRECTDRAW			lpDD;// DirectDraw 对象指针
	LPDIRECTDRAWSURFACE		lpDDSPrimary;// DirectDraw 主表面指针
	LPDIRECTDRAWSURFACE		lpDDSOffScr;// DirectDraw 离屏表面指针
	LPDIRECTDRAWCLIPPER		lpClipper;
private:
	static void _show_directx_error()
	{
		DWORD dwErr=::GetLastError();
		CString cs;
		cs.Format(_T("无法加载Ddraw.dll库，请安装Directx7.0[%d]"),(int)dwErr);
		::MessageBox(NULL,cs,_T("严重错误"),MB_OK);
	}
public:
	CGuiDirectDrawDisplay(HWND _hWnd)
	{
		::memset(&this->mRecordOrgDdsd,0,sizeof(this->mRecordOrgDdsd));
		::memset(&this->m_ddsd,0,sizeof(this->m_ddsd));
		this->lpDDSOffScr=NULL;
		this->lpClipper=NULL;
		this->hDdrawHodule=::LoadLibrary(_T("Ddraw.dll"));
		if(this->hDdrawHodule==NULL)
			_show_directx_error();

		{
			typedef HRESULT WINAPI T_My_DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
			T_My_DirectDrawCreate *lcFuncDirectDrawCreate=(T_My_DirectDrawCreate*)::GetProcAddress(this->hDdrawHodule,_T("DirectDrawCreate"));
			if(lcFuncDirectDrawCreate(NULL, &this->lpDD, NULL) != DD_OK)
				_show_directx_error();
		}

		{// 设置协作层
			if (this->lpDD->SetCooperativeLevel(_hWnd,DDSCL_NORMAL | DDSCL_NOWINDOWCHANGES) != DD_OK)
				_show_directx_error();
		}

		{// 创建主表面
			DDSURFACEDESC lc_ddsd_1={0};// DirectDraw 表面描述
			lc_ddsd_1.dwSize = sizeof(lc_ddsd_1);
			lc_ddsd_1.dwFlags = DDSD_CAPS;
			lc_ddsd_1.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			if (lpDD->CreateSurface(&lc_ddsd_1, &lpDDSPrimary, NULL) != DD_OK)
				_show_directx_error();
		}
		{
			this->lpClipper=NULL;
			if(lpDD->CreateClipper(0,&this->lpClipper, NULL)!=DD_OK||this->lpClipper==NULL)
				_show_directx_error();
			if(this->lpClipper->SetHWnd(0,_hWnd)!=DD_OK)
				_show_directx_error();
			if(lpDDSPrimary->SetClipper(this->lpClipper)!=DD_OK)
				_show_directx_error();
		}
	}
	~CGuiDirectDrawDisplay()
	{
		if(this->lpDDSOffScr != NULL)
		{
			this->lpDDSOffScr->Release();
			this->lpDDSOffScr = NULL;
		}
		if(this->lpClipper!=NULL)
		{
			this->lpClipper->Release();
			this->lpClipper=NULL;
		}
		if(this->lpDDSPrimary != NULL)
		{
			this->lpDDSPrimary->Release();
			this->lpDDSPrimary = NULL;
		}
		if(this->lpDD != NULL)
		{
			this->lpDD->Release();
			this->lpDD = NULL;
		}
		if(this->hDdrawHodule!=NULL)
		{
			::FreeLibrary(this->hDdrawHodule);
			this->hDdrawHodule=NULL;
		}
	}
	bool draw(CDirectDrawFrameFormat *_pDirectDrawData,int _wnd_left,int _wnd_top,int _wnd_width,int _wnd_height)
	{
		struct T_CB_R: public CDirectDrawFrameFormat::I_DdCallBack
		{
			CGuiDirectDrawDisplay *pParent;
			virtual bool vfLock(DDSURFACEDESC *_ddsd)
			{
				{// 创建离屏表面对象
					DDSURFACEDESC *lc_pddsd=_ddsd;
					if(this->pParent->lpDDSOffScr==NULL||::memcmp(&this->pParent->mRecordOrgDdsd,lc_pddsd,sizeof(this->pParent->mRecordOrgDdsd))!=0)
					{
						if(this->pParent->lpDDSOffScr != NULL)
						{
							this->pParent->lpDDSOffScr->Release();
							this->pParent->lpDDSOffScr = NULL;
						}
						this->pParent->mRecordOrgDdsd=*lc_pddsd;
						this->pParent->m_ddsd=*lc_pddsd;
						HRESULT lc_res=this->pParent->lpDD->CreateSurface(lc_pddsd, &this->pParent->lpDDSOffScr, NULL);
						if (lc_res != DD_OK)
							CGuiDirectDrawDisplay::_show_directx_error();
					}
				}
				HRESULT lcRval=DD_OK;
				do {
					lcRval = this->pParent->lpDDSOffScr->Lock(NULL,&this->pParent->m_ddsd,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
				} while(lcRval == DDERR_WASSTILLDRAWING);
				if(lcRval != DD_OK)
				{
					CGuiDirectDrawDisplay::_show_directx_error();
					return false;
				}
				*_ddsd=this->pParent->m_ddsd;
				return true;
			}
		} m_lc_cb;
		m_lc_cb.pParent=this;
		_pDirectDrawData->execute(&m_lc_cb);
		lpDDSOffScr->Unlock(NULL);

		// Blt到主表面上
		RECT lc_rctSour={0};
		lc_rctSour.left=0;
		lc_rctSour.top=0;
		lc_rctSour.right=lc_rctSour.left+this->m_ddsd.dwWidth;
		lc_rctSour.bottom=lc_rctSour.top+this->m_ddsd.dwHeight;

		RECT lc_rcDest={0};
		lc_rcDest.left=_wnd_left;
		lc_rcDest.top=_wnd_top;
		lc_rcDest.right=lc_rcDest.left+_wnd_width;
		lc_rcDest.bottom=lc_rcDest.top+_wnd_height;

		HRESULT lcRval=DD_OK;
		do {
			lcRval=lpDDSPrimary->Blt(&lc_rcDest,lpDDSOffScr, &lc_rctSour, DDBLT_WAIT, NULL);
		} while(lcRval == DDERR_WASSTILLDRAWING);
		if(lcRval != DD_OK)
			return false;
		return true;
	}
};
