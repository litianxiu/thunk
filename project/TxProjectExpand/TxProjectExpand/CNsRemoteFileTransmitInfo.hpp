#pragma once
#include "CNsCommonTransmitInfo.hpp"
#include "CNsTransmitShellPackInfo.h"

namespace CNsTransmitInfo
{
	struct tagRequestRemoteFileFind: public tagJournalTransBase
	{
		TxBlobString strFilePath;
		TxBlobString strWildcard;
	public :
		tagRequestRemoteFileFind()
		{
		}
		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				_sa->bindObject(this->strFilePath);
				_sa->bindObject(this->strWildcard);
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
		void set(const char *_strFilePath,const char *_strWildcard)
		{
			this->strFilePath.assign(_strFilePath);
			this->strWildcard.assign(_strWildcard);
		}
	};

	struct tagRespondRemoteFileFind: public tagJournalTransBase
	{
		struct tagFileInfo
		{
			TxBlobString filename;
			long long filesize;//-1代表文件夹
			long long time;
			EnumFilePermissionMode permission;
			bool operator<(const tagFileInfo& _tem) const
			{
				if(this->filename.compare(".")==0)
					return true;
				else if(_tem.filename.compare(".")==0)
					return false;
				else if(this->filename.compare("..")==0)
					return true;
				else if(_tem.filename.compare("..")==0)
					return false;
				else if(this->filesize<0&&_tem.filesize>=0)
					return true;
				else if(this->filesize>=0&&_tem.filesize<0)
					return false;
				return ::strcmp(TxBlobString(this->filename).upperCaseSelf().c_str(),TxBlobString(_tem.filename).upperCaseSelf().c_str())<0;
			}
		};
		TxBlobString sErrorText;
		TxBlobString strFilePath;
		TxBlobString strWildcard;
		std::list<tagFileInfo> listFileInfo;
	public :

		tagRespondRemoteFileFind()
		{
		}
		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				int lc_count=(int)this->listFileInfo.size();
				_sa->bindObject(this->sErrorText);
				_sa->bindObject(this->strWildcard);
				_sa->bindObject(this->strFilePath);
				_sa->bindObject(lc_count);
				this->listFileInfo.resize(lc_count);
				std::list<tagFileInfo>::iterator iter=this->listFileInfo.begin();
				std::list<tagFileInfo>::iterator iter_end=this->listFileInfo.end();
				for(;iter!=iter_end;iter++)
				{
					_sa->bindObject(iter->filename);
					_sa->bindObject(iter->filesize);
					_sa->bindObject(iter->time);
					_sa->bindObject(iter->permission);
				}
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

		void set(const char *_sErrorText,const char *_strFilePath,const char *_strWildcard)
		{
			this->sErrorText=_sErrorText;
			this->strFilePath=_strFilePath;
			this->strWildcard=_strWildcard;
		}
	};

	struct tagRequestRemoteFileDeleteFile: public tagJournalTransBase
	{
		TxBlobString strFilePath;
		std::list<TxBlobString> listName;
	public :
		tagRequestRemoteFileDeleteFile()
		{
		}
		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				_sa->bindObject(this->strFilePath);
				int iLcCount=(int)this->listName.size();
				_sa->bindObject(iLcCount);
				this->listName.resize(iLcCount);
				std::list<TxBlobString>::iterator iter=this->listName.begin();
				std::list<TxBlobString>::iterator iter_end=this->listName.end();
				for(;iter!=iter_end;iter++)
					_sa->bindObject(*iter);
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
		void set(const char *_strFilePath)
		{
			this->strFilePath.assign(_strFilePath);
		}
	};

	struct tagRespondRemoteFileDeleteFile: public tagJournalTransBase
	{
		TxBlobString sErrorText;
	public :

		tagRespondRemoteFileDeleteFile()
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
			this->sErrorText.assign(_sErrorText);
		}
	};

	struct tagRequestRemoteFileReadDataStream: public tagJournalTransBase
	{
		TxBlobString strFilePath;
		long long llFileDataPos;
		int iReadDataSize;
	public :
		tagRequestRemoteFileReadDataStream()
		{
		}
		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				_sa->bindObject(this->strFilePath);
				_sa->bindObject(this->llFileDataPos);
				_sa->bindObject(this->iReadDataSize);
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
		void set(const char *_strFilePath,long long _llFileDataPos,int _iReadDataSize)
		{
			this->strFilePath.assign(_strFilePath);
			this->llFileDataPos=_llFileDataPos;
			this->iReadDataSize=_iReadDataSize;
		}
	};

	struct tagRespondRemoteFileReadDataStream: public tagJournalTransBase
	{
		TxBlobString sErrorText;
		long long llFileSize;
		TxBlobString mReadFileData;
	public :
		tagRespondRemoteFileReadDataStream()
		{
		}
		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				_sa->bindObject(this->sErrorText);
				_sa->bindObject(this->llFileSize);
				_sa->bindObject(this->mReadFileData);
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
		void set(const char *_sErrorText,long long _llFileSize,
			const void *_pReadFileData,int _iReadDataSize)
		{
			this->sErrorText.assign(_sErrorText);
			this->llFileSize=_llFileSize;
			this->mReadFileData.assign((const char*)_pReadFileData,_iReadDataSize);
		}
	};

	struct tagRequestRemoteFileWriteDataStream: public tagJournalTransBase
	{
		TxBlobString strFilePath;
		long long llFileDataPos;
		TxBlobString mWriteData;
	public :
		tagRequestRemoteFileWriteDataStream()
		{
		}
		virtual bool serialize(TxSerializeArchive *_sa)
		{
			bool ret=true;
			try
			{
				_sa->bindObject(this->strFilePath);
				_sa->bindObject(this->llFileDataPos);
				_sa->bindObject(this->mWriteData);
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
		void set(const char *_strFilePath,long long _llFileDataPos,const void *_data,int _size)
		{
			this->strFilePath.assign(_strFilePath);
			this->llFileDataPos=_llFileDataPos;
			this->mWriteData.assign((const char*)_data,_size);
		}
	};

	struct tagRespondRemoteFileWriteDataStream: public tagJournalTransBase
	{
		TxBlobString sErrorText;
	public :
		tagRespondRemoteFileWriteDataStream()
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
			this->sErrorText.assign(_sErrorText);
		}
	};

};

