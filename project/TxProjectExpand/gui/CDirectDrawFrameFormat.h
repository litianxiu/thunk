#pragma once

#include <Windows.h>
#include <string.h>
#include <assert.h>
#include <list>
#include <ddraw.h>
#include "../common/TxBlobString.hpp"

class CDirectDrawFrameFormat: public TxForbidDirectCopyObject
{
public :
	class I_DdCallBack
	{
	public:
		virtual bool vfLock(DDSURFACEDESC *_ddsd)=0;
	};
private:
	TxBlobString mOrgBuffer;
	int iWidth,iHeight;
	typedef bool (CDirectDrawFrameFormat::*t_func_exec)(I_DdCallBack*);
	t_func_exec m_fun_exec;;
public:
	//DDSURFACEDESC* getDdSurfaceDescPtr() { return &this->m_ddsd; }
	//TxBlobString* getFrameStreamPtr() { return &this->data; }
	void load_data(const void *_data,int _datasize,DWORD _dwWidth, DWORD _dwHeight);
	bool execute(I_DdCallBack *_cb);
	bool init_YV12(I_DdCallBack *_cb);
	bool init_I420(I_DdCallBack *_cb);
	bool init_YUYV(I_DdCallBack *_cb);
	bool init_YVYU(I_DdCallBack *_cb);
	bool init_UYVY(I_DdCallBack *_cb);

	bool init_NV12(I_DdCallBack *_cb)
	{
		if(_cb==NULL)
		{
			this->m_fun_exec=&CDirectDrawFrameFormat::init_NV12;
			return true;
		}
		DDSURFACEDESC lc_ddsd={0};
		lc_ddsd.dwSize		= sizeof(lc_ddsd);
		lc_ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
		lc_ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
		lc_ddsd.dwWidth	= this->iWidth;
		lc_ddsd.dwHeight	= this->iHeight;
		lc_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		lc_ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_YUV;
		lc_ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
		lc_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('N','V','1','2');

		if(!_cb->vfLock(&lc_ddsd))
			return false;

		const unsigned char *lc_pSrcY = (const unsigned char*)this->mOrgBuffer.data();
		const unsigned char *lc_pSrcUV =  lc_pSrcY+(this->iWidth*this->iHeight);

		int dwLineSize0=(int)this->iWidth;

		unsigned char *lc_lpSurface=(unsigned char*)lc_ddsd.lpSurface;

		if (lc_pSrcY != NULL) {
			for (int i=0; i<(int)this->iHeight; ++i) {
				::memcpy(lc_lpSurface,lc_pSrcY,dwLineSize0);
				lc_pSrcY += dwLineSize0;
				lc_lpSurface += lc_ddsd.lPitch;
			}
		}
		if (lc_pSrcUV != NULL) {
			for (int i=0; i<(int)(this->iHeight>>1); ++i) {
				::memcpy(lc_lpSurface,lc_pSrcUV,dwLineSize0);
				lc_pSrcUV +=dwLineSize0;
				lc_lpSurface += lc_ddsd.lPitch;
			}
		}
		assert((size_t)lc_pSrcUV<=(size_t)(this->mOrgBuffer.data()+this->mOrgBuffer.size()));
		return true;
	}

	bool init_NV21(I_DdCallBack *_cb)
	{
		if(_cb==NULL)
		{
			this->m_fun_exec=&CDirectDrawFrameFormat::init_NV21;
			return true;
		}
		DDSURFACEDESC lc_ddsd={0};
		lc_ddsd.dwSize = sizeof(lc_ddsd);
		lc_ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
		lc_ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
		lc_ddsd.dwWidth	= this->iWidth;
		lc_ddsd.dwHeight	= this->iHeight;
		lc_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		lc_ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_YUV;
		lc_ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
		lc_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('N','V','2','1');

		if(!_cb->vfLock(&lc_ddsd))
			return false;

		const unsigned char *lc_pSrcY = (const unsigned char*)this->mOrgBuffer.data();
		const unsigned char *lc_pSrcVU =  lc_pSrcY+(this->iWidth*this->iHeight);

		int dwLineSize0=(int)this->iWidth;

		unsigned char *lc_lpSurface=(unsigned char*)lc_ddsd.lpSurface;

		if (lc_pSrcY != NULL) {
			for (int i=0; i<(int)this->iHeight; ++i) {
				::memcpy(lc_lpSurface, lc_pSrcY, dwLineSize0);
				lc_pSrcY += dwLineSize0;
				lc_lpSurface += lc_ddsd.lPitch;
			}
		}
		if (lc_pSrcVU != NULL) {
			for (int i=0; i<(int)(this->iHeight>>1); ++i) {
				::memcpy(lc_lpSurface, lc_pSrcVU, dwLineSize0);
				lc_pSrcVU += dwLineSize0;
				lc_lpSurface += lc_ddsd.lPitch;
			}
		}
		assert((size_t)lc_pSrcVU<=(size_t)(this->mOrgBuffer.data()+this->mOrgBuffer.size()));
		return true;
	}

	bool init_PAL8(I_DdCallBack *_cb)
	{
		if(_cb==NULL)
		{
			this->m_fun_exec=&CDirectDrawFrameFormat::init_PAL8;
			return true;
		}
		DDSURFACEDESC lc_ddsd={0};
		lc_ddsd.dwSize		= sizeof(lc_ddsd);
		lc_ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
		lc_ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
		lc_ddsd.dwWidth	= this->iWidth;
		lc_ddsd.dwHeight	= this->iHeight;
		lc_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		lc_ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB|DDPF_PALETTEINDEXED8;
		lc_ddsd.ddpfPixelFormat.dwRGBBitCount = 8;
		//lc_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('P','A','L','8');

		if(!_cb->vfLock(&lc_ddsd))
			return false;

		const unsigned char *lc_pSrcRGB = (const unsigned char*)this->mOrgBuffer.data();

		int dwLineSize0=(int)this->iWidth;

		unsigned char *lc_lpSurface=(unsigned char*)lc_ddsd.lpSurface;

		for (int i=0; i<(int)this->iHeight; ++i) 
		{
			::memcpy(lc_lpSurface, lc_pSrcRGB,(int)(this->iWidth<<2));
			lc_pSrcRGB += dwLineSize0;
			lc_lpSurface += lc_ddsd.lPitch;
		}

		assert((size_t)lc_pSrcRGB<=(size_t)(this->mOrgBuffer.data()+this->mOrgBuffer.size()));
		return true;
	}

	bool init_RGB555(I_DdCallBack *_cb)
	{
		enum
		{
			RBitMask = 0x7C00, 
			GBitMask = 0x03E0, 
			BBitMask = 0x001F, 
		};
		if(_cb==NULL)
		{
			this->m_fun_exec=&CDirectDrawFrameFormat::init_RGB555;
			return true;
		}
		DDSURFACEDESC lc_ddsd={0};
		lc_ddsd.dwSize		= sizeof(lc_ddsd);
		lc_ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
		lc_ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
		lc_ddsd.dwWidth	= this->iWidth;
		lc_ddsd.dwHeight	= this->iHeight;
		lc_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		lc_ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		lc_ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		lc_ddsd.ddpfPixelFormat.dwRBitMask = RBitMask;
		lc_ddsd.ddpfPixelFormat.dwGBitMask = GBitMask;
		lc_ddsd.ddpfPixelFormat.dwBBitMask = BBitMask;
		lc_ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
		//lc_ddsd.dwSize		= sizeof(lc_ddsd);
		//lc_ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
		//lc_ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
		//lc_ddsd.dwWidth	= this->iWidth;
		//lc_ddsd.dwHeight	= this->iHeight;
		//lc_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		//lc_ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_YUV;
		//lc_ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
		//lc_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('N','V','1','2');

		if(!_cb->vfLock(&lc_ddsd))
			return false;

		const unsigned short *lc_pSrcRGB = (const unsigned short*)this->mOrgBuffer.data();

		int dwLineSize0=(int)this->iWidth<<1;

		unsigned short *lc_lpSurface=(unsigned short*)lc_ddsd.lpSurface;

		for (int i=0; i<(int)this->iHeight; ++i)
		{
			::memset(lc_lpSurface,0,this->iWidth*sizeof(unsigned short));
			for (int j=0; j<(int)this->iWidth;++j)
			{
				unsigned int lcSrcRgb=lc_pSrcRGB[j];
				unsigned int lcDesRgb=0;
				lcDesRgb |= (lcSrcRgb & RBitMask) >> 10;
				lcDesRgb |= (lcSrcRgb & GBitMask);
				lcDesRgb |= (lcSrcRgb & BBitMask) << 10;
				lc_lpSurface[j] = (unsigned short)lcDesRgb;
			}
			lc_pSrcRGB += dwLineSize0 / sizeof(unsigned short);
			lc_lpSurface += lc_ddsd.lPitch / sizeof(unsigned short);
		}
		assert((size_t)lc_pSrcRGB<=(size_t)(this->mOrgBuffer.data()+this->mOrgBuffer.size()));
		return true;
	}

	//struct FormatRGB565
	//{
	//	enum
	//	{
	//		RBitMask = 0xF800, 
	//		GBitMask = 0x07E0, 
	//		BBitMask = 0x001F, 
	//	};

	//	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	//	{
	//		ZeroMemory(&ddsd, sizeof(ddsd));

	//		ddsd.dwSize		= sizeof(ddsd);
	//		ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
	//		ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	//		ddsd.dwWidth	= dwWidth;
	//		ddsd.dwHeight	= dwHeight;
	//		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

	//		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	//		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;

	//		ddsd.ddpfPixelFormat.dwRBitMask = RBitMask;
	//		ddsd.ddpfPixelFormat.dwGBitMask = GBitMask;
	//		ddsd.ddpfPixelFormat.dwBBitMask = BBitMask;

	//		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	//	}
	//	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	//	{
	//		PSHORT lpSrcRGB = (PSHORT) imageSpace.lpRGB;
	//		PSHORT lpDstRGB = (PSHORT) lpSurface;

	//		for (DWORD i=0; i<dwHeight; ++i) {
	//			ZeroMemory(lpDstRGB, dwWidth * sizeof(SHORT));

	//			for (DWORD j=0; j<dwWidth; ++j) {
	//				lpDstRGB[j] |= (lpSrcRGB[j] & RBitMask) >> 11;
	//				lpDstRGB[j] |= (lpSrcRGB[j] & GBitMask);
	//				lpDstRGB[j] |= (lpSrcRGB[j] & BBitMask) << 11;
	//			}

	//			lpSrcRGB += imageSpace.dwLineSize[0] / sizeof(SHORT);
	//			lpDstRGB += lPitch / sizeof(SHORT);
	//		}
	//	}
	//};

	//struct FormatBGR24
	//{
	//	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	//	{
	//		ZeroMemory(&ddsd, sizeof(ddsd));

	//		ddsd.dwSize		= sizeof(ddsd);
	//		ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
	//		ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	//		ddsd.dwWidth	= dwWidth;
	//		ddsd.dwHeight	= dwHeight;
	//		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

	//		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	//		ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
	//		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	//	}
	//	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	//	{
	//		LPBYTE lpSrcRGB	= imageSpace.lpRGB;
	//		LPDWORD lpDstRGB= (LPDWORD) lpSurface;

	//		for (DWORD i=0; i<dwHeight; ++i) {
	//			ZeroMemory(lpDstRGB, dwWidth * sizeof(DWORD));

	//			for (DWORD j=0; j<dwWidth; ++j) {
	//				lpDstRGB[j] |= (lpSrcRGB[3 * j]);
	//				lpDstRGB[j] |= (lpSrcRGB[3 * j + 1] << 8);
	//				lpDstRGB[j] |= (lpSrcRGB[3 * j + 2] << 16);
	//			}

	//			lpSrcRGB += imageSpace.dwLineSize[0];
	//			lpDstRGB += lPitch / sizeof(DWORD);
	//		}
	//	}
	//};

	//struct FormatRGB32
	//{
	//	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	//	{
	//		ZeroMemory(&ddsd, sizeof(ddsd));

	//		ddsd.dwSize		= sizeof(ddsd);
	//		ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
	//		ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	//		ddsd.dwWidth	= dwWidth;
	//		ddsd.dwHeight	= dwHeight;
	//		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

	//		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	//		ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
	//		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	//	}
	//	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	//	{
	//		LPBYTE lpSrcRGB	= imageSpace.lpRGB;
	//		LPDWORD lpDstRGB= (LPDWORD) lpSurface;

	//		for (DWORD i=0; i<dwHeight; ++i) {
	//			ZeroMemory(lpDstRGB, dwWidth * sizeof(DWORD));

	//			for (DWORD j=0; j<dwWidth; ++j) {
	//				lpDstRGB[j] |= (lpSrcRGB[4 * j]);
	//				lpDstRGB[j] |= (lpSrcRGB[4 * j + 1] << 8);
	//				lpDstRGB[j] |= (lpSrcRGB[4 * j + 2] << 16);
	//				lpDstRGB[j] |= (lpSrcRGB[4 * j + 3] << 24);
	//			}

	//			lpSrcRGB += imageSpace.dwLineSize[0];
	//			lpDstRGB += lPitch / sizeof(DWORD);
	//		}
	//	}
	//};

	//struct FormatBGR32
	//{
	//	static VOID Build(DDSURFACEDESC2 &ddsd, DWORD dwWidth, DWORD dwHeight)
	//	{
	//		ZeroMemory(&ddsd, sizeof(ddsd));

	//		ddsd.dwSize		= sizeof(ddsd);
	//		ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
	//		ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	//		ddsd.dwWidth	= dwWidth;
	//		ddsd.dwHeight	= dwHeight;
	//		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

	//		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
	//		ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
	//		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	//	}
	//	static VOID Copy(LPBYTE lpSurface, const ImageSpace &imageSpace, DWORD dwWidth, DWORD dwHeight, LONG lPitch)
	//	{
	//		LPBYTE lpSrcRGB	= imageSpace.lpRGB;
	//		LPDWORD lpDstRGB= (LPDWORD) lpSurface;

	//		for (DWORD i=0; i<dwHeight; ++i) {
	//			ZeroMemory(lpDstRGB, dwWidth * sizeof(DWORD));

	//			for (DWORD j=0; j<dwWidth; ++j) {
	//				lpDstRGB[j] |= (lpSrcRGB[4 * j] << 16);
	//				lpDstRGB[j] |= (lpSrcRGB[4 * j + 1] << 8);
	//				lpDstRGB[j] |= (lpSrcRGB[4 * j + 2]);
	//				lpDstRGB[j] |= (lpSrcRGB[4 * j + 3] << 24);
	//			}

	//			lpSrcRGB += imageSpace.dwLineSize[0];
	//			lpDstRGB += lPitch / sizeof(DWORD);
	//		}
	//	}
	//};

};
