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
};
