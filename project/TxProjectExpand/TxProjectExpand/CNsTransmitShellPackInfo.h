#pragma once
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <ctime>
#include <utility>
#include "../common/TxIocpTcpApi.hpp"
#include "../include/CSqliteExpandDll.h"
#include "../common/TxBlobString.hpp"
#include "../common/TxShellPackage.hpp"
#include "../common/TxSerializeArchive.hpp"

namespace CNsTransmitInfo
{
	enum E_TYPE
	{
		e_type_null=0,

		//sqlite
		e_type_sql_execute_rqt=1,
		e_type_sql_execute_rpd,
		e_type_insert_journal_rqt,
		e_type_insert_journal_rpd,

		//ÎÄ¼þ²Ù×÷
		e_type_remote_file_find_rqt=20,
		e_type_remote_file_find_rpd,
		e_type_remote_file_delete_rqt,
		e_type_remote_file_delete_rpd,

		//e_type_remote_file_fileinfo_rqt,
		//e_type_remote_file_fileinfo_rpd,
		e_type_remote_file_readstream_rqt,
		e_type_remote_file_readstream_rpd,
		e_type_remote_file_writestream_rqt,
		e_type_remote_file_writestream_rpd,
	};

	struct tagJournalTransBase
	{
		E_TYPE eType;
	public :
		tagJournalTransBase() { this->eType=e_type_null; }
		virtual ~tagJournalTransBase() { }
		virtual bool serialize(TxSerializeArchive *_sa)=0;
		template<class T>
		inline T* getOrgThisPointer() const
		{
			DefAssertWrap(dynamic_cast<T*>(const_cast<tagJournalTransBase*>(this))!=NULL);
			return (T*)const_cast<tagJournalTransBase*>(this);
		}
		void initBasic(E_TYPE _eType)
		{
			this->eType=_eType;
		}
	};

	struct tagJournalTransHeartBeat: public tagJournalTransBase
	{
	public:
		tagJournalTransHeartBeat()
		{
			this->eType=e_type_null;
		}
		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
			}
			catch (TxSerializeArchive::TxSuccessException&)
			{
			}
			catch(...)
			{
				ret=false;
			}
			return ret;
		}
	};

	class CShellPackData: public TxShellPackage
	{
	private:
		static void _static_pop_result_call_back(void *_userdata,unsigned int _pkg_num,const unsigned char* _data,int _size);
		static void _static_pack_call_back(void *_userdata,const unsigned char* _data,int _size);
	public :
		TxCppPlatform::shared_ptr<tagJournalTransBase> popResult(unsigned int *_uiPkgNumber);
		static void pack_and_send(TxCppPlatform::shared_ptr<TxIocpTcpApi::TxIocpSocketBasic> &_sp,tagJournalTransBase *_inData,unsigned int _uiPkgNum);
	};

};

