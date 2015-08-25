#pragma once
#include "CNsTransmitShellPackInfo.h"

namespace CNsTransmitInfo
{
	struct tagRequestInsertJournal: public tagJournalTransBase
	{
		TxBlobString msgText;
		TxBlobString sProgramName;
		TxBlobString sCodeFile;
		TxBlobString sCodeFunc;
		char msgTime[16];
		char osTime[16];
		int iCodeLine;
		unsigned int uiRand;
		EnumSqliteJournalLevel eLogLevel;
	public :

		tagRequestInsertJournal()
		{
			this->iCodeLine=-1;
			this->msgTime[0]=0;
			this->osTime[0]=0;
			this->eLogLevel=eSqliteJournalLevelNull;
		}

		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				_sa->bindObject(this->msgText);
				_sa->bindObject(this->sProgramName);
				_sa->bindObject(this->sCodeFile);
				_sa->bindObject(this->sCodeFunc);
				_sa->bindArrayString(this->msgTime);
				_sa->bindArrayString(this->osTime);
				_sa->bindObject(this->iCodeLine);
				_sa->bindObject(this->eLogLevel);
				_sa->bindObject(this->uiRand);
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
		void set(const char *_msgText,const char *_sProgramName,const char *_sCodeFile,const char *_sCodeFunc,int _iCodeLine,
			const char *_msgTime,const char *_osTime,EnumSqliteJournalLevel _eLogLevel,unsigned int _uiRand)
		{
			this->sProgramName=_sProgramName;
			this->msgText.assign(_msgText);
			this->sCodeFile.assign(_sCodeFile);
			this->sCodeFunc.assign(_sCodeFunc);
			TxBlobString::txStrNCopy(this->msgTime,_msgTime!=NULL?_msgTime:_osTime);
			TxBlobString::txStrNCopy(this->osTime,_osTime);
			this->iCodeLine=_iCodeLine;
			this->eLogLevel=_eLogLevel;
			this->uiRand=_uiRand;
		}
	};

	struct tagRespondInsertJournal: public tagJournalTransBase
	{
		TxBlobString sErrorText;
	public :

		tagRespondInsertJournal()
		{
		}
		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				_sa->bindObject(this->sErrorText);
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

		void set(const char *_sErrorText)
		{
			this->sErrorText=_sErrorText;
		}
	};

	struct tagRequestSqlExecute: public tagJournalTransBase
	{
		TxBlobString sql;
	public :

		tagRequestSqlExecute()
		{
		}
		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				_sa->bindObject(this->sql);
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

		void set(const char *_sql)
		{
			this->sql=_sql;
		}
	};

	struct tagRespondSqlExecute: public tagJournalTransBase
	{
		struct tagGroupInfo
		{
			struct tagItemInfo
			{
				TxBlobString sName;
				TxBlobString sValue;
				tagItemInfo(const char *_name=NULL,const char *_value=NULL)
				{
					if(_name!=NULL) this->sName=_name;
					if(_value!=NULL) this->sValue=_value;
				}
			} ;
			std::vector<tagGroupInfo::tagItemInfo> vArryItem;
		} ;
		TxBlobString sErrorText;
		std::list<tagRespondSqlExecute::tagGroupInfo> listGroupInfo;
	public :

		tagRespondSqlExecute()
		{
		}

		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				_sa->bindObject(this->sErrorText);
				int iLcYCount=(int)this->listGroupInfo.size();
				_sa->bindObject(iLcYCount);
				this->listGroupInfo.resize(iLcYCount);
				std::list<tagGroupInfo>::iterator iter=this->listGroupInfo.begin();
				std::list<tagGroupInfo>::iterator iter_end=this->listGroupInfo.end();
				for(;iter!=iter_end;iter++)
				{
					std::vector<tagGroupInfo::tagItemInfo> *pArry=&iter->vArryItem;
					int iXCount=(int)pArry->size();
					_sa->bindObject(iXCount);
					if(iXCount!=(int)pArry->size()) pArry->resize(iXCount);
					for(int x=0;x<iXCount;x++)
					{
						_sa->bindObject(pArry->at(x).sName);
						_sa->bindObject(pArry->at(x).sValue);
					}
				}
			}
			catch (TxSerializeArchive::TxSuccessException&)
			{
				this->sErrorText="catch expected";
			}
			catch(...)
			{
				ret=false;
			}
			return ret;
		}

		void clear()
		{
			this->listGroupInfo.clear();
			this->sErrorText.clear();
		}

		void init(const char *_sErrorText)
		{
			this->sErrorText=_sErrorText;
		}

		int push(int _n_column,char **_column_name,char **_column_value)
		{
			int ret=0;
			this->listGroupInfo.push_back(tagGroupInfo());
			std::vector<tagGroupInfo::tagItemInfo> *pArry=&this->listGroupInfo.back().vArryItem;
			if(_n_column!=(int)pArry->size()) pArry->resize(_n_column);
			for(int i=0;i <_n_column;i++ )
			{
				pArry->at(i).sName.assign(_column_name[i]!=NULL?_column_name[i]:"");
				pArry->at(i).sValue.assign(_column_value[i]!=NULL?_column_value[i]:"");
				ret+=pArry->at(i).sName.length();
				ret+=pArry->at(i).sValue.length();
			}
			return ret;
		}
	};
};

