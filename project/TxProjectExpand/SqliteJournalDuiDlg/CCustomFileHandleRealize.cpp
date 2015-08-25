#include "StdAfx.h"
#include "CCustomFileHandleRealize.h"


void CALLBACK CRemoteNetworkFileHandle::g_read_op_(void *_userdata,const void *_data)
{
	tagReadCbInfo* pLcUserdata=(tagReadCbInfo*)_userdata;
	const tagRemoteFileReadRespondData* pLcMsg=(const tagRemoteFileReadRespondData*)_data;
	if(pLcMsg->error!=NULL)
	{
		pLcUserdata->err->assign(pLcMsg->error);
		pLcUserdata->bGood=0;
	}
	else
	{
		pLcUserdata->blob->assign(pLcMsg->data,pLcMsg->size);
		pLcUserdata->bGood=1;
		*pLcUserdata->llTotalSize=pLcMsg->llTotalSize;
	}
}

void CALLBACK CRemoteNetworkFileHandle::g_write_op_(void *_userdata,const void *_data)
{
	tagWriteCbInfo* pLcUserdata=(tagWriteCbInfo*)_userdata;
	const tagRemoteFileWriteRespondData* pLcMsg=(const tagRemoteFileWriteRespondData*)_data;
	if(pLcMsg->error!=NULL)
	{
		pLcUserdata->err->assign(pLcMsg->error);
		pLcUserdata->bGood=0;
	}
	else
	{
		pLcUserdata->bGood=1;
	}
}

CRemoteNetworkFileHandle::CRemoteNetworkFileHandle(const char *_ip,int _port,const char *_sFileUri)
	:m_DbmsClientSessionDll(_ip,_port)
{
	this->strFileUri.assign(_sFileUri);
}

void CRemoteNetworkFileHandle::vf_read(TxBlobString *_outData,TxBlobString *_err,long long *_filesize,long long _pos,int _size,int _i_time_msec)
{
	tagReadCbInfo lc_args={0};
	lc_args.bGood=0;
	lc_args.err=_err;
	lc_args.blob=_outData;
	lc_args.llTotalSize=_filesize;
	this->m_DbmsClientSessionDll.executeReadRemoteFile(this->strFileUri.c_str(),_i_time_msec,_pos,_size,&lc_args,g_read_op_);
	if(!lc_args.bGood&&_err->size()<=0)
		_err->assign("Î´Öª´íÎó");
}

void CRemoteNetworkFileHandle::vf_write(TxBlobString *_err,const void *_buff,long long _pos,int _size,int _i_time_msec)
{
	tagWriteCbInfo lc_args={0};
	lc_args.bGood=0;
	lc_args.err=_err;
	this->m_DbmsClientSessionDll.executeWriteRemoteFile(this->strFileUri.c_str(),_i_time_msec,_buff,_pos,_size,&lc_args,g_write_op_);
	if(!lc_args.bGood&&_err->size()<=0)
		_err->assign("Î´Öª´íÎó");
}

void CRemoteNetworkFileHandle::vf_release()
{
}


