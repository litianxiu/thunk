#pragma once

#include "../common/TxBlobString.hpp"
#include "../include/CSqliteExpandDll.h"

class CCustomFileHandleInterface
{
public :
	virtual ~CCustomFileHandleInterface() { }
	virtual void vf_read(TxBlobString *_outData,TxBlobString *_err,long long *_filesize,long long _pos,int _size,int _i_time_msec)=0;
	virtual void vf_write(TxBlobString *_err,const void *_buff,long long _pos,int _size,int _i_time_msec)=0;
	virtual void vf_release()=0;
};

class CRemoteNetworkFileHandle: public CCustomFileHandleInterface
{
private:
	struct tagReadCbInfo {
		TxBlobString *blob;
		TxBlobString *err;
		long long *llTotalSize;
		int bGood;
	};
	struct tagWriteCbInfo {
		TxBlobString *err;
		int bGood;
	};
private:
	CDbmsClientSessionDll m_DbmsClientSessionDll;
	std::string strFileUri;
private:
	static void CALLBACK g_read_op_(void *_userdata,const void *_data);
	static void CALLBACK g_write_op_(void *_userdata,const void *_data);
public:
	CRemoteNetworkFileHandle(const char *_ip,int _port,const char *_sFileUri);
	virtual void vf_read(TxBlobString *_outData,TxBlobString *_err,long long *_filesize,long long _pos,int _size,int _i_time_msec);
	virtual void vf_write(TxBlobString *_err,const void *_buff,long long _pos,int _size,int _i_time_msec);
	virtual void vf_release();
};

