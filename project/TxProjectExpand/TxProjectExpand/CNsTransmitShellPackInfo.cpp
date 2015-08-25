#define _Tx_SqliteExpand_dll_OutPut_
#include "CNsTransmitShellPackInfo.h"
#include "CNsCommonTransmitInfo.hpp"
#include "CNsRemoteFileTransmitInfo.hpp"

struct tagShellPackDataPopResultCallBackInfo
{
	TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> ret;
	unsigned int *uiPkgNumber;
};

void CNsTransmitInfo::CShellPackData::_static_pop_result_call_back(void *_userdata,unsigned int _pkg_num,const unsigned char* _data,int _size)
{
	tagShellPackDataPopResultCallBackInfo *pRet=(tagShellPackDataPopResultCallBackInfo*)_userdata;
	TxUnpackArchive pkg;
	pkg.setStreamBuffer(_data,_size);
	E_TYPE lc_type=e_type_null;
	pkg.bindObject(lc_type);
	tagJournalTransBase* obj=NULL;
	switch(lc_type)
	{
	case e_type_sql_execute_rqt:
		obj=new tagRequestSqlExecute();
		break;
	case e_type_sql_execute_rpd:
		obj=new tagRespondSqlExecute();
		break;
	case e_type_insert_journal_rqt:
		obj=new tagRequestInsertJournal();
		break;
	case e_type_insert_journal_rpd:
		obj=new tagRespondInsertJournal();
		break;
	case e_type_remote_file_find_rqt:
		obj=new tagRequestRemoteFileFind();
		break;
	case e_type_remote_file_find_rpd:
		obj=new tagRespondRemoteFileFind();
		break;
	case e_type_remote_file_delete_rqt:
		obj=new tagRequestRemoteFileDeleteFile();
		break;
	case e_type_remote_file_delete_rpd:
		obj=new tagRespondRemoteFileDeleteFile();
		break;
	case e_type_remote_file_readstream_rqt:
		obj=new tagRequestRemoteFileReadDataStream();
		break;
	case e_type_remote_file_readstream_rpd:
		obj=new tagRespondRemoteFileReadDataStream();
		break;
	case e_type_remote_file_writestream_rqt:
		obj=new tagRequestRemoteFileWriteDataStream();
		break;
	case e_type_remote_file_writestream_rpd:
		obj=new tagRespondRemoteFileWriteDataStream();
		break;
	default:
		DefAssertWrap(FALSE);
		break;
	}
	if(obj!=NULL)
	{
		if(obj->serialize(&pkg))
		{
			pRet->uiPkgNumber[0]=_pkg_num;
			obj->initBasic(lc_type);
			pRet->ret.reset(obj);
		}
		else
		{
			delete obj;
			obj=NULL;
			DefAssertWrap(FALSE);
		}
	}
}

void CNsTransmitInfo::CShellPackData::_static_pack_call_back(void *_userdata,const unsigned char* _data,int _size)
{
	DefAssertTextWrap(dynamic_cast<TxIocpTcpApi::TxIocpTransmitSocket*>(((TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic>*)_userdata)->get()),"");
	TxIocpTcpApi::TxIocpTransmitSocket::asyncWrite(*(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic>*)_userdata,_data,_size);
}

TxCppPlatform::shared_ptr<CNsTransmitInfo::tagJournalTransBase> CNsTransmitInfo::CShellPackData::popResult(unsigned int *_uiPkgNumber)
{
	unsigned int __tem_pkg_num;
	if(_uiPkgNumber==NULL) _uiPkgNumber=&__tem_pkg_num;
	tagShellPackDataPopResultCallBackInfo tem;
	tem.uiPkgNumber=_uiPkgNumber;
	__super::popResult(_static_pop_result_call_back,&tem);
	return tem.ret;
}

void CNsTransmitInfo::CShellPackData::pack_and_send(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,tagJournalTransBase *_inData,unsigned int _uiPkgNum)
{
	TxPackageArchive pkg;
	pkg.bindObject(_inData->eType);
	_inData->serialize(&pkg);
	__super::pack(_uiPkgNum,pkg.getStreamBytes(),_static_pack_call_back,&_sp);
}
