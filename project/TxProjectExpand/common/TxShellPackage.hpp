#pragma once

#include <list>
#include <assert.h>
#include <string.h>
#include "TxBlobString.hpp"
#include "TxAssertWrap.hpp"
#include "TxSystemDependent.hpp"
#include "TxCrc32CheckCode.hpp"

//全包：crc32,num2
//分包：head2,crc4,len2
class TxShellPackage
{
private:
	enum { e_max_unit_size=1024, e_head_ch0=(unsigned int)0xac, e_head_ch1=(unsigned int)0x58, };
	struct tagUnitData
	{
		unsigned char data[e_max_unit_size];
		int dataSize;
		unsigned int crc32;
		int lenght16;
		int bCompleteSign;
		enum {
			e_null=0,
			e_head0,
			e_head1,
			e_crc0,
			e_crc1,
			e_crc2,
			e_crc3,
			e_len0,
			e_len1,
			e_data0,
		} eStatus;
		inline void reset() { this->eStatus=e_head0; }
		//inline void operator=(const tagUnitData&) { }
		//tagUnitData(const tagUnitData&) { }
		//void assign(const tagUnitData &_tem) { ::memcpy(this,&_tem,sizeof(*this)); }
	} mRecordUnitData;
	std::list<tagUnitData> listUnitData;
	int iCompleteCount;
private:
	static void _build_my_crc32_len(unsigned char _crc32[],const void *_data,int _len,const void *_data2=NULL,int _len2=0)
	{
		if(_crc32!=NULL)
		{
			TxCrc32CheckCode m_crc32_obj;
			m_crc32_obj.setFromUint32Value(_len+_len2);
			if(_data!=NULL&&_len>0)
				m_crc32_obj.pushData(_data,_len);
			if(_data2!=NULL&&_len2>0)
				m_crc32_obj.pushData(_data2,_len2);
			m_crc32_obj.getCrc32Buffer(_crc32);
		}
	}
	void _pop_result_(void (*_cb)(void*,unsigned int,const unsigned char*,int),void *_userdata)
	{
		if(this->listUnitData.size()>0&&this->iCompleteCount>0)
		{
			unsigned char __lc_buff[6*1024];
			unsigned char *pBuffPtr=&__lc_buff[0];
			int iBuffSize=0;
			int iLcStrSize=0;
			--this->iCompleteCount;
			for(std::list<tagUnitData>::iterator iter=this->listUnitData.begin();;iter++)
			{
				iLcStrSize+=iter->dataSize;
				if(iter->bCompleteSign) break;
			}
			if(iLcStrSize>sizeof(__lc_buff))
				pBuffPtr=new unsigned char[iLcStrSize+32];
			for(std::list<tagUnitData>::iterator iter=this->listUnitData.begin();;)
			{
				DefAssertTextWrap(iBuffSize+iter->dataSize<=iLcStrSize,"");
				DefAssertTextWrap(iter->dataSize>0,"");
				memcpy(&pBuffPtr[iBuffSize],iter->data,iter->dataSize);
				iBuffSize+=iter->dataSize;
				int bLcCompleteSign=iter->bCompleteSign;
				iter=this->listUnitData.erase(iter);
				if(bLcCompleteSign)
					break;
			}
			DefAssertTextWrap((int)(iBuffSize)==(int)iLcStrSize,"");
			DefAssertTextWrap((int)(iBuffSize)>=(int)8,"");
			unsigned int lc_crc32=0;
			_build_my_crc32_len((unsigned char*)&lc_crc32,pBuffPtr+4,iBuffSize-4);
			if(*(unsigned int*)&lc_crc32==*(unsigned int*)&pBuffPtr[0]&&iBuffSize>=8)
			{//校验通过
				_cb(_userdata,
					(((unsigned int)pBuffPtr[4])<<24)|(((unsigned int)pBuffPtr[5])<<16)|(((unsigned int)pBuffPtr[6])<<8)|(((unsigned int)pBuffPtr[7])<<0),
					pBuffPtr+8,iBuffSize-8);
			}
			else
				DefAssertWrap(FALSE);
			if(pBuffPtr!=&__lc_buff[0])
				delete[] pBuffPtr;
		}
	}
	void _pushData__(const unsigned char *_data,int _count)
	{
		while(_count>0)
		{
			switch(this->mRecordUnitData.eStatus)
			{
			case tagUnitData::e_head0:
				if((unsigned int)*_data==e_head_ch0)
				{
					this->mRecordUnitData.eStatus=tagUnitData::e_head1;
					_data++;
					_count--;
				}
				else
				{
					DefAssertWrap(FALSE);
					const unsigned char *pS=(const unsigned char*)::memchr(_data+1,tagUnitData::e_head0,_count-1);
					if(pS!=NULL)
					{
						_count-=(int)(pS-_data);
						_data=pS;
					}
					else
					{
						_data+=_count;
						_count=0;
					}
				}
				break;
			case tagUnitData::e_head1:
				if((unsigned int)*_data==e_head_ch1)
				{
					this->mRecordUnitData.eStatus=tagUnitData::e_crc0;
					_data++;
					_count--;
				}
				else
				{
					DefAssertWrap(FALSE);
					this->mRecordUnitData.reset();
				}
				break;
			case tagUnitData::e_crc0:
				this->mRecordUnitData.crc32=((unsigned short)*_data)<<24;
				this->mRecordUnitData.eStatus=tagUnitData::e_crc1;
				_data++;
				_count--;
				break;
			case tagUnitData::e_crc1:
				this->mRecordUnitData.crc32|=((unsigned int)*_data)<<16;
				this->mRecordUnitData.eStatus=tagUnitData::e_crc2;
				_data++;
				_count--;
				break;
			case tagUnitData::e_crc2:
				this->mRecordUnitData.crc32|=((unsigned int)*_data)<<8;
				this->mRecordUnitData.eStatus=tagUnitData::e_crc3;
				_data++;
				_count--;
				break;
			case tagUnitData::e_crc3:
				this->mRecordUnitData.crc32|=((unsigned int)*_data);
				this->mRecordUnitData.eStatus=tagUnitData::e_len0;
				_data++;
				_count--;
				break;
			case tagUnitData::e_len0:
				this->mRecordUnitData.lenght16=((unsigned int)*_data)<<8;
				this->mRecordUnitData.eStatus=tagUnitData::e_len1;
				_data++;
				_count--;
				break;
			case tagUnitData::e_len1:
				this->mRecordUnitData.lenght16|=((unsigned int)*_data);
				this->mRecordUnitData.eStatus=tagUnitData::e_data0;
				_data++;
				_count--;
				this->mRecordUnitData.dataSize=0;
				if(this->mRecordUnitData.lenght16>e_max_unit_size)
				{
					DefAssertWrap(FALSE);
					this->mRecordUnitData.reset();
					break;
				}
			case tagUnitData::e_data0:
				{
					//int iLcUnitSize=(int)sizeof(this->mRecordUnitData.data)-this->mRecordUnitData.dataSize;
					int iLcUnitSize=this->mRecordUnitData.lenght16-this->mRecordUnitData.dataSize;
					if(iLcUnitSize>_count) iLcUnitSize=_count;
					DefAssertTextWrap(iLcUnitSize>=0,"");
					if(iLcUnitSize>0)
						memcpy(&this->mRecordUnitData.data[this->mRecordUnitData.dataSize],_data,iLcUnitSize);
					_data+=iLcUnitSize;
					_count-=iLcUnitSize;
					this->mRecordUnitData.dataSize+=iLcUnitSize;
					if(this->mRecordUnitData.lenght16<=this->mRecordUnitData.dataSize)
					{
						unsigned char lc_crc32[4]={0};
						_build_my_crc32_len(lc_crc32,this->mRecordUnitData.data,this->mRecordUnitData.lenght16);
						if(((((unsigned int)lc_crc32[0])<<24)
							|(((unsigned int)lc_crc32[1])<<16)
							|(((unsigned int)lc_crc32[2])<<8)
							|(((unsigned int)lc_crc32[3])<<0))
							==(unsigned int)this->mRecordUnitData.crc32)
						{
							if(this->mRecordUnitData.lenght16<=0)
							{
								if(this->listUnitData.size()>0)
								{
									if(!this->listUnitData.back().bCompleteSign)
									{
										this->iCompleteCount++;
										this->listUnitData.back().bCompleteSign=TRUE;
									}
								}
							}
							else
							{
								this->mRecordUnitData.bCompleteSign=FALSE;
								this->listUnitData.push_back(this->mRecordUnitData);
							}
						}
						else
							DefAssertWrap(FALSE);
						this->mRecordUnitData.reset();
					}
				}
				break;
			default:
				DefAssertWrap(FALSE);
				break;
			}
		}
	}
	static int _build_package_node(unsigned char _out_pack_data[],
		const unsigned char *_data,int _count,const unsigned char *_data2,int _count2)
	{
		//head
		_out_pack_data[0]=(unsigned char)e_head_ch0;
		_out_pack_data[1]=(unsigned char)e_head_ch1;
		//crc32
		_build_my_crc32_len(&_out_pack_data[2],_data,_count,_data2,_count2);
		//length
		_out_pack_data[6]=(unsigned char)((unsigned int)(_count+_count2)>>8);
		_out_pack_data[7]=(unsigned char)((unsigned int)(_count+_count2)>>0);
		if(_count>0) memcpy(&_out_pack_data[8],_data,_count);
		if(_count2>0) memcpy(&_out_pack_data[_count+8],_data2,_count2);
		return _count+_count2+8;
	}
	static void _pack_(unsigned int _pkg_num,const unsigned char *_data,int _count,void (*_cb)(void*,const unsigned char*,int),void *_userdata)
	{
		//crc32--global
		unsigned char g_crc32_pkgnumber[8]={0};
		int g_crc32_pkgnumber_count=sizeof(g_crc32_pkgnumber);
		g_crc32_pkgnumber[4]=(unsigned char)(_pkg_num>>24);
		g_crc32_pkgnumber[5]=(unsigned char)(_pkg_num>>16);
		g_crc32_pkgnumber[6]=(unsigned char)(_pkg_num>>8);
		g_crc32_pkgnumber[7]=(unsigned char)(_pkg_num>>0);
		_build_my_crc32_len(g_crc32_pkgnumber,&g_crc32_pkgnumber[4],4,_data,_count);
		unsigned char __lc_buff[6*1024];
		unsigned char *org_buff=&__lc_buff[0];
		int iBuffSize=0;
		int iLcCalSizeII=_count+sizeof(g_crc32_pkgnumber);
		iLcCalSizeII=iLcCalSizeII+((iLcCalSizeII+e_max_unit_size-1)/e_max_unit_size)*8+8;
		if(sizeof(__lc_buff)<iLcCalSizeII)
			org_buff=new unsigned char[iLcCalSizeII+32];
		unsigned char *pBuffPtr=org_buff;
		while(_count>0)
		{
			int iLcUnitSize=g_crc32_pkgnumber_count+_count;
			if(iLcUnitSize>e_max_unit_size)
				iLcUnitSize=e_max_unit_size;
			iLcUnitSize-=g_crc32_pkgnumber_count;
			//build data node
			pBuffPtr+=_build_package_node(pBuffPtr,g_crc32_pkgnumber,g_crc32_pkgnumber_count,_data,iLcUnitSize);
			_data+=iLcUnitSize;
			_count-=iLcUnitSize;
			g_crc32_pkgnumber_count=0;
		}
		//build empty node
		pBuffPtr+=_build_package_node(pBuffPtr,NULL,0,NULL,0);
		DefAssertTextWrap((int)(pBuffPtr-org_buff)==iLcCalSizeII,"");
		_cb(_userdata,org_buff,(int)(pBuffPtr-org_buff));
		if(org_buff!=(&__lc_buff[0]))
			delete[] org_buff;
	}
public :
	TxShellPackage()
	{
		TxCrc32CheckCode::initialize();
		this->iCompleteCount=0;
		this->mRecordUnitData.reset();
		this->listUnitData.clear();
	}
	~TxShellPackage()
	{
	}
	void pushData(const void *_data,int _count)
	{
		return this->_pushData__((const unsigned char*)_data,_count);
	}
	void pushData(const TxBlobString &_data)
	{
		return this->_pushData__((const unsigned char*)_data.data(),(int)_data.size());
	}
	void popResult(void (*_cb)(void*,unsigned int,const unsigned char*,int),void *_userdata)
	{
		return this->_pop_result_(_cb,_userdata);
	}
	template<class TFUNC>
	void popResult(TFUNC &_mFunc)
	{
		class MFR
		{ TFUNC *const pFunc;
		public:
			MFR(TFUNC *_pFunc):pFunc(_pFunc) { }
			static void _g_call_back_pop_result(void *_userdata,const unsigned char *_data,int _size)
			{ return (*((MFR*)_userdata)->pFunc)(_data,_size); }
		} m_MFR(&_mFunc);
		return this->_pop_result_(&MFR::_g_call_back_pop_result,&m_MFR);
	}
	bool popResultString(TxBlobString *_outData,unsigned int *_pkg_num)
	{
		struct MFR
		{
			TxBlobString *outData;
			unsigned int *numPkg;
			bool *ret;
		public:
			static void _g_call_back_pop_result(void *_userdata,unsigned int  _pkg_num,const unsigned char *_data,int _size)
			{
				*((MFR*)_userdata)->ret=true;
				*((MFR*)_userdata)->numPkg=_pkg_num;
				((MFR*)_userdata)->outData->assign((const char*)_data,_size);
			}
		} m_MFR;
		bool ret=false;
		m_MFR.outData=_outData;
		m_MFR.ret=&ret;
		m_MFR.numPkg=_pkg_num;
		this->_pop_result_(&MFR::_g_call_back_pop_result,&m_MFR);
		return ret;
	}
	TxCppPlatform::shared_ptr<TxBlobString> popResultSharedString(unsigned int *_numPkg)
	{
		struct MFR
		{
			TxCppPlatform::shared_ptr<TxBlobString> *ret;
			unsigned int *numPkg;
		public:
			static void _g_call_back_pop_result(void *_userdata,unsigned int  _pkg_num,const unsigned char *_data,int _size)
			{
				((MFR*)_userdata)->ret->reset(new TxBlobString());
				(*((MFR*)_userdata)->ret)->assign((const char*)_data,_size);
				*((MFR*)_userdata)->numPkg=_pkg_num;
			}
		} m_MFR;
		TxCppPlatform::shared_ptr<TxBlobString> ret;
		m_MFR.ret=&ret;
		m_MFR.numPkg=_numPkg;
		this->_pop_result_(&MFR::_g_call_back_pop_result,&m_MFR);
		return ret;
	}
	static void pack(unsigned int _num_pkg,const void *_data,int _count,void (*_cb)(void*,const unsigned char*,int),void *_userdata)
	{
		return _pack_(_num_pkg,(const unsigned char*)_data,_count,_cb,_userdata);
	}
	static void pack(unsigned int _num_pkg,const TxBlobString &_data,void (*_cb)(void*,const unsigned char*,int),void *_userdata)
	{
		return _pack_(_num_pkg,(const unsigned char*)_data.data(),(int)_data.size(),_cb,_userdata);
	}
	template<class TFUNC>
	static void pack(unsigned int _num_pkg,const void *_data,int _count,TFUNC &_mFunc)
	{
		class MFR
		{ TFUNC *const pFunc;
		public:
			MFR(TFUNC *_pFunc):pFunc(_pFunc) { }
			static void _g_call_back_pop_result(void *_userdata,const unsigned char *_data,int _size)
			{ return (*((MFR*)_userdata)->pFunc)(_data,_size); }
		} m_MFR(&_mFunc);
		return _pack_(_num_pkg,(const unsigned char*)_data,_count,&MFR::_g_call_back_pop_result,&m_MFR);
	}
	template<class TFUNC>
	static void pack(unsigned int _num_pkg,const TxBlobString &_data,TFUNC &_mFunc)
	{
		return pack(_num_pkg,(const void*)_data.data(),(int)_data.size(),_mFunc);
	}
	static void pack(TxBlobString *_outData,unsigned int _num_pkg,const void *_data,int _count)
	{
		struct MFR
		{ TxBlobString *outData;
		static void _g_call_back_pop_result(void *_userdata,const unsigned char *_data,int _size)
		{ ((MFR*)_userdata)->outData->assign((const char*)_data,_size); }
		} m_MFR;
		m_MFR.outData=_outData;
		return _pack_(_num_pkg,(const unsigned char*)_data,_count,&MFR::_g_call_back_pop_result,&m_MFR);
	}
	static void pack(TxBlobString *_outData,unsigned int _num_pkg,const TxBlobString &_data)
	{
		return pack(_outData,_num_pkg,(const void*)_data.data(),(int)_data.size());
	}
};


//char buff_t[52400];
//TxBlobString g_str;
//TxSystemDependent::TxMutexWrap g_mutex;
//
//void __cb_thread_init_(void *_arg1,void *_arg2)
//{
//	srand((unsigned int)time(NULL));
//	for(;;)
//	{
//		for(int i=0;i<sizeof(buff_t);i++)
//			buff_t[i]=rand()%4==0?0:(char)rand();
//	}
//}
//
//TxBlobString _getString()
//{
//	int i1=rand()%sizeof(buff_t);
//	int i2=rand()%sizeof(buff_t);
//	if(i1>i2)
//	{
//		int a=i1;
//		i1=i2;
//		i2=a;
//	}
//	return TxBlobString(&buff_t[i1],&buff_t[i2]);
//}
//
//void __cb_thread_append_(void *_arg1,void *_arg2)
//{
//	for(;;)
//	{
//		TxBlobString str;
//		str.append(_getString());
//		str.append(_getString());
//		str.assign(_getString());
//		str.append(_getString());
//		str.append(_getString());
//		str.append(_getString());
//		str.append(_getString());
//		g_mutex.lock();
//		g_str.append(str);
//		if(g_str.size()>1024*1024)
//			g_str.clear();
//		g_mutex.unlock();
//		str.append(_getString());
//		str.append(_getString());
//	}
//}
//
//void g_test_pack(void *,void *)
//{
//	srand((unsigned int)time(NULL));
//	for(;;)
//	{
//		unsigned int nnnn;
//		TxShellPackage mPkg;
//		TxBlobString org_str=_getString();
//		org_str.append("abcdefg");
//		TxBlobString str_sri;
//		TxShellPackage::pack(&str_sri,-1,org_str);
//		int len1=rand()%(str_sri.size());
//		int len2=str_sri.size()-len1; if(len2>0) len2=rand()%len2;
//		int len3=str_sri.size()-len1-len2; if(len3>0) len3=rand()%len3;
//		int len4=str_sri.size()-len1-len2-len3; if(len4>0) len4=rand()%len4;
//		int len5=str_sri.size()-len1-len2-len3-len4;
//		int fdsfdssdffds=len1+len2+len3+len4+len5;
//		int ookk=0;
//		for(ookk=0;ookk<rand()%10+1;ookk++)
//		{
//			mPkg.pushData(str_sri.data(),len1);
//			mPkg.pushData(str_sri.data()+len1,len2);
//			mPkg.pushData(str_sri.data()+len1+len2,len3);
//			mPkg.pushData(str_sri.data()+len1+len2+len3,len4);
//			mPkg.pushData(str_sri.data()+len1+len2+len3+len4,len5);
//			std::shared_ptr<TxBlobString> sppp(new TxBlobString());
//		}
//		int jlkjsdfkl=0;
//		while(ookk--)
//		{
//			TxBlobString outOOORG;
//			DefAssertTextWrap(mPkg.popResultString(&outOOORG,&nnnn),"");
//			if(outOOORG.size()!=org_str.size()||memcmp(outOOORG.data(),org_str.data(),org_str.size())!=0)
//			{
//				int adfs=0;
//				DefAssertWrap(FALSE);
//			}
//		}
//	}
//}
//
//int main()
//{
//	std::vector<std::shared_ptr<TxSystemDependent::TxThreadWrap>> vThread;
//	{
//		vThread.push_back(std::shared_ptr<TxSystemDependent::TxThreadWrap>(new TxSystemDependent::TxThreadWrap()));
//		vThread.back()->create(__cb_thread_init_,NULL,NULL);
//	}
//	{
//		for(int i=0;i<8;i++)
//		{
//			vThread.push_back(std::shared_ptr<TxSystemDependent::TxThreadWrap>(new TxSystemDependent::TxThreadWrap()));
//			vThread.back()->create(g_test_pack,NULL,NULL);
//		}
//	}
//	{
//		for(int i=0;i<(int)vThread.size();i++)
//			vThread[i]->join();
//	}
//	return 0;
//}
