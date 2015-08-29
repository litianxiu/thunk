#include "stdafx.h"
#include "CDirectDrawFrameFormat.h"


void CDirectDrawFrameFormat::load_data(const void *_data,int _datasize,DWORD _dwWidth, DWORD _dwHeight)
{
	this->mOrgBuffer.assign((const char*)_data,_datasize);
	this->iWidth=(int)_dwWidth;
	this->iHeight=(int)(_dwHeight);
}

bool CDirectDrawFrameFormat::execute(I_DdCallBack *_cb)
{
	return (this->*this->m_fun_exec)(_cb);
}

bool CDirectDrawFrameFormat::init_YV12(I_DdCallBack *_cb)
{
	if(_cb==NULL)
	{
		this->m_fun_exec=&CDirectDrawFrameFormat::init_YV12;
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
	lc_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','V','1','2');

	if(!_cb->vfLock(&lc_ddsd))
		return false;

	const unsigned char *lc_pY	= (const unsigned char*)this->mOrgBuffer.data();
	const unsigned char *lc_pU	= lc_pY+(this->iWidth*this->iHeight);
	const unsigned char *lc_pV	= lc_pU+((this->iWidth*this->iHeight)>>2);

	int dwLineSize0=(int)this->iWidth;
	int dwLineSize1=(int)(this->iWidth>>1);
	int dwLineSize2=(int)(this->iWidth>>1);

	unsigned char *lc_lpSurface=(unsigned char*)lc_ddsd.lpSurface;

	for (int i=0; i<(int)this->iHeight; ++i)
	{
		::memcpy(lc_lpSurface, lc_pY,dwLineSize0);
		lc_pY += dwLineSize0;
		lc_lpSurface += lc_ddsd.lPitch;
	}
	for (int i=0; i<(int)(this->iHeight>>1); ++i)
	{
		::memcpy(lc_lpSurface, lc_pV,dwLineSize1);
		lc_pV += dwLineSize1;
		lc_lpSurface += lc_ddsd.lPitch >> 1;
	}
	for (int i=0; i<(int)(this->iHeight>>1); ++i)
	{
		::memcpy(lc_lpSurface, lc_pU,dwLineSize2);
		lc_pU += dwLineSize2;
		lc_lpSurface += lc_ddsd.lPitch >> 1;
	}

	assert((size_t)lc_pV<=(size_t)(this->mOrgBuffer.data()+this->mOrgBuffer.size()));
	return true;
}

bool CDirectDrawFrameFormat::init_I420(I_DdCallBack *_cb)
{
	if(_cb==NULL)
	{
		this->m_fun_exec=&CDirectDrawFrameFormat::init_I420;
		return true;
	}
	DDSURFACEDESC lc_ddsd={0};
	lc_ddsd.dwSize		= sizeof(lc_ddsd);
	lc_ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
	lc_ddsd.dwFlags	= DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT;
	lc_ddsd.dwWidth	=this->iWidth;
	lc_ddsd.dwHeight	=this->iHeight;
	lc_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	lc_ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_YUV;
	lc_ddsd.ddpfPixelFormat.dwYUVBitCount = 8;
	lc_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('I','4','2','0');

	if(!_cb->vfLock(&lc_ddsd))
		return false;

	const unsigned char *lc_pY	= (const unsigned char*)this->mOrgBuffer.data();
	const unsigned char *lc_pU	= lc_pY+(this->iWidth*this->iHeight);
	const unsigned char *lc_pV	= lc_pU+((this->iWidth*this->iHeight)>>2);

	int dwLineSize0=(int)this->iWidth;
	int dwLineSize1=(int)(this->iWidth>>1);
	int dwLineSize2=(int)(this->iWidth>>1);

	unsigned char *lc_lpSurface=(unsigned char*)lc_ddsd.lpSurface;

	for (int i=0; i<(int)this->iHeight; ++i)
	{
		::memcpy(lc_lpSurface,lc_pY, dwLineSize0);
		lc_pY += dwLineSize0;
		lc_lpSurface += lc_ddsd.lPitch;
	}
	for (int i=0; i<(int)(this->iHeight >> 1); ++i)
	{
		::memcpy(lc_lpSurface,lc_pU, dwLineSize2);
		lc_pU += dwLineSize2;
		lc_lpSurface += lc_ddsd.lPitch >> 1;
	}
	for (int i=0; i<(int)(this->iHeight >> 1); ++i)
	{
		::memcpy(lc_lpSurface,lc_pV, dwLineSize1);
		lc_pV += dwLineSize1;
		lc_lpSurface += lc_ddsd.lPitch >> 1;
	}
	assert((size_t)lc_pV<=(size_t)(this->mOrgBuffer.data()+this->mOrgBuffer.size()));
	return true;
}

bool CDirectDrawFrameFormat::init_YUYV(I_DdCallBack *_cb)
{
	if(_cb==NULL)
	{
		this->m_fun_exec=&CDirectDrawFrameFormat::init_YUYV;
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
	lc_ddsd.ddpfPixelFormat.dwYUVBitCount = 16;
	lc_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','U','Y','2');

	if(!_cb->vfLock(&lc_ddsd))
		return false;

	const unsigned char *lc_pYUY2=(const unsigned char*)this->mOrgBuffer.data();
	int dwLineSize0=(int)this->iWidth<<1;
	unsigned char *lc_lpSurface=(unsigned char*)lc_ddsd.lpSurface;
	for (int i=0; i<(int)this->iHeight; ++i)
	{
		::memcpy(lc_lpSurface, lc_pYUY2, dwLineSize0);
		lc_pYUY2 += dwLineSize0;
		lc_lpSurface+=lc_ddsd.lPitch;
	}
	assert((size_t)lc_pYUY2<=(size_t)(this->mOrgBuffer.data()+this->mOrgBuffer.size()));
	return true;
}

bool CDirectDrawFrameFormat::init_YVYU(I_DdCallBack *_cb)
{
	if(_cb==NULL)
	{
		this->m_fun_exec=&CDirectDrawFrameFormat::init_YVYU;
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
	lc_ddsd.ddpfPixelFormat.dwYUVBitCount = 16;
	lc_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','V','Y','U');

	if(!_cb->vfLock(&lc_ddsd))
		return false;

	const unsigned char *lc_pYVYU=(const unsigned char*)this->mOrgBuffer.data();
	int dwLineSize0=(int)this->iWidth<<1;
	unsigned char *lc_lpSurface=(unsigned char*)lc_ddsd.lpSurface;

	for (int i=0; i<(int)this->iHeight; ++i)
	{
		::memcpy(lc_lpSurface,lc_pYVYU, dwLineSize0);
		lc_pYVYU += dwLineSize0;
		lc_lpSurface += lc_ddsd.lPitch;
	}
	assert((size_t)lc_pYVYU<=(size_t)(this->mOrgBuffer.data()+this->mOrgBuffer.size()));
	return true;
}

bool CDirectDrawFrameFormat::init_UYVY(I_DdCallBack *_cb)
{
	if(_cb==NULL)
	{
		this->m_fun_exec=&CDirectDrawFrameFormat::init_UYVY;
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
	lc_ddsd.ddpfPixelFormat.dwYUVBitCount = 16;
	lc_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('U','Y','V','Y');

	if(!_cb->vfLock(&lc_ddsd))
		return false;

	const unsigned char *lc_pYUY2=(const unsigned char*)this->mOrgBuffer.data();
	int dwLineSize0=(int)this->iWidth<<1;
	unsigned char *lc_lpSurface=(unsigned char*)lc_ddsd.lpSurface;
	for (int i=0; i<(int)this->iHeight; ++i)
	{
		::memcpy(lc_lpSurface, lc_pYUY2, dwLineSize0);
		lc_pYUY2 += dwLineSize0;
		lc_lpSurface+=lc_ddsd.lPitch;
	}
	assert((size_t)lc_pYUY2<=(size_t)(this->mOrgBuffer.data()+this->mOrgBuffer.size()));
	return true;
}

