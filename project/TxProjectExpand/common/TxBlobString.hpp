#pragma once

#ifdef WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <string>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <cctype>
#include <assert.h>
#include <algorithm>
#include <type_traits>
#include "TxGlobalConfig.h"

namespace __nsBlobTmplString_Template_
{
	template<class T> struct TxIsTypeFloat { const static bool value=false; };
	template<> struct TxIsTypeFloat<float> { const static bool value=true; };
	template<> struct TxIsTypeFloat<float&> { const static bool value=true; };
	template<> struct TxIsTypeFloat<const float> { const static bool value=true; };
	template<> struct TxIsTypeFloat<const float&> { const static bool value=true; };

	template<class T> struct TxIsTypeDouble { const static bool value=false; };
	template<> struct TxIsTypeDouble<double> { const static bool value=true; };
	template<> struct TxIsTypeDouble<double&> { const static bool value=true; };
	template<> struct TxIsTypeDouble<const double> { const static bool value=true; };
	template<> struct TxIsTypeDouble<const double&> { const static bool value=true; };

	template<class T> struct TxIsTypeLongDouble { const static bool value=false; };
	template<> struct TxIsTypeLongDouble<long double> { const static bool value=true; };
	template<> struct TxIsTypeLongDouble<long double&> { const static bool value=true; };
	template<> struct TxIsTypeLongDouble<const long double> { const static bool value=true; };
	template<> struct TxIsTypeLongDouble<const long double&> { const static bool value=true; };

	template<int NSIZE> struct TxSizeSignedInteger{ };
	template<> struct TxSizeSignedInteger<2> { typedef signed short type; };
	template<> struct TxSizeSignedInteger<4> { typedef signed int type; };
}

template<class TypeChar,class TypeOpsChar,class StdString,class TPeerTypeChar,class TPeerTypeOpsChar,class StdPeerString>
class TxBlobTmplString
{
private:
	typedef TxBlobTmplString<TPeerTypeChar,TPeerTypeOpsChar,StdPeerString,TypeChar,TypeOpsChar,StdString> PeerStrongTmplStringType;
	enum { e_tss_base_size=40/(sizeof(TypeChar)), };
	TypeChar *dataPtr;
	int iLength;
	int iBuffMaxSize;
	void *const __forbid_assign__;
	TypeChar _mem_buff_temp_[e_tss_base_size];
public :
	inline static int __min_int_(int val1,int val2) { return (val1)<=(val2)?(val1):(val2); }
	inline static int __max_int_(int val1,int val2) { return (val1)>=(val2)?(val1):(val2); }
	bool _DoSaveTssAppendWriteFile_(const char *_fileName,const char *_fileMode) const
	{
		FILE *fp=fopen(_fileName,_fileMode);
		if(fp==NULL) return false;
		int wCount=fwrite(this->dataPtr,1,this->iLength*sizeof(TypeChar),fp);
		fclose(fp);
		return wCount==this->iLength*sizeof(TypeChar);
	}
	inline static int __my_str_len(const TypeChar _str[])
	{
		if(_str==NULL||_str[0]==0) return 0;
		return sizeof(TypeChar)==sizeof(char)?((int)::strlen((const char*)_str)):((int)::wcslen((const wchar_t*)_str));
	}
	inline static TypeChar* __my_mem_chr(const TypeChar _str[],size_t _count,TypeChar _val)
	{
		return sizeof(TypeChar)==sizeof(char)?((TypeChar)::memchr((const char*)_str,(char)_val,_count))
			:((TypeChar)::wmemchr((const wchar_t*)_str,(wchar_t)_val,_count));
	}
	inline static void __my_mem_set(const TypeChar _str[],size_t _count,TypeChar _val)
	{
		if(sizeof(TypeChar)==sizeof(char))
			::memset((char*)_str,(char)_val,_count);
		else
			::wmemset((wchar_t*)_str,(wchar_t)_val,_count);
	}
	inline static void __my_mem_cpy(TypeChar* _dest, const TypeChar* _src, size_t _count)
	{
		::memcpy(_dest,_src,_count*sizeof(TypeChar));
	}
	inline static int __my_vsn_printf(TypeChar *_dest,int _buf_size, const TypeChar* _format, va_list _vlist)
	{
		return sizeof(TypeChar)==sizeof(char)?
			((int)::vsnprintf((char*)_dest,_buf_size,(const char*)_format,_vlist))
			:((int)::vswprintf((wchar_t*)_dest,_buf_size,(const wchar_t*)_format,_vlist));
	}
	inline static const TypeChar* __my_mem_chr(const TypeChar *_str,TypeChar ch,int _iLength)
	{
		return sizeof(TypeChar)==sizeof(char)?
			(const TypeChar*)::memchr(_str,(unsigned char)ch,_iLength)
			:(const TypeChar*)::wmemchr((const wchar_t*)_str,(wchar_t)ch,_iLength);
	}
	inline void __constructor_init__()
	{
		char _buf_tem_[((sizeof(TypeChar)==sizeof(char)||sizeof(TypeChar)==sizeof(wchar_t))&&sizeof(TypeChar)==sizeof(TypeOpsChar)
			&&(sizeof(TPeerTypeChar)==sizeof(char)||sizeof(TPeerTypeOpsChar)==sizeof(wchar_t))&&sizeof(TPeerTypeChar)==sizeof(TPeerTypeOpsChar)
			&&sizeof(TypeChar)!=sizeof(TPeerTypeChar))?1:-1]; (void)_buf_tem_[0];
		dataPtr=_mem_buff_temp_;
		iLength=0;
		iBuffMaxSize=e_tss_base_size;
		_mem_buff_temp_[0]=0;
	}
	void __clear__()
	{
		if(dataPtr!=_mem_buff_temp_)
		{
			delete[] this->dataPtr;
			this->dataPtr=_mem_buff_temp_;
		}
		this->iLength=0;
		this->iBuffMaxSize=e_tss_base_size;
		this->_mem_buff_temp_[0]=0;
	}
	void __resize__(const int _iNewSize,const bool _bKeepOrgn)
	{
		if(_iNewSize<=1) this->__clear__();
		else if(_iNewSize<=e_tss_base_size)
		{
			if(dataPtr!=_mem_buff_temp_)
			{
				if(_bKeepOrgn) __my_mem_cpy(_mem_buff_temp_,dataPtr,__min_int_(_iNewSize,e_tss_base_size));
				delete[] this->dataPtr;
				this->dataPtr=_mem_buff_temp_;
			}
			this->iLength=_iNewSize-1;
			this->iBuffMaxSize=e_tss_base_size;
		}
		else
		{
			if(_iNewSize>iBuffMaxSize||_iNewSize<(iBuffMaxSize>>2))//重新分配内存
			{
				int iLcBuffMaxTemSize=(iBuffMaxSize<_iNewSize?iBuffMaxSize:e_tss_base_size);
				while((iLcBuffMaxTemSize<<=1)<_iNewSize) ;
				this->iBuffMaxSize=iLcBuffMaxTemSize;
				if(_bKeepOrgn)
				{
					TypeChar *pOld=this->dataPtr;
					this->dataPtr=new TypeChar[iLcBuffMaxTemSize];
					__my_mem_cpy(this->dataPtr,pOld,__min_int_(this->iLength+1,_iNewSize));
					if(pOld!=_mem_buff_temp_) delete[] pOld;
				}
				else
				{
					if(this->dataPtr!=_mem_buff_temp_) delete[] this->dataPtr;
					this->dataPtr=new TypeChar[iLcBuffMaxTemSize];
				}
			}
			this->iLength=_iNewSize-1;
		}
	}
	inline bool _data_pointer_inside_(const void *_str) const
	{
		return (uintptr_t)this->dataPtr<=(uintptr_t)_str&&((uintptr_t)(this->dataPtr+this->iBuffMaxSize))>(uintptr_t)_str;
		//return ((uintptr_t)((intptr_t)_str-(intptr_t)this->dataPtr))<(uintptr_t)(this->iBuffMaxSize*sizeof(TypeChar));
	}
	void __assign__(const TypeChar *_str,int _iLen=-1)
	{
		if(_iLen<0) _iLen=__my_str_len(_str);
		if(this->_data_pointer_inside_(_str))
			this->assign(TxBlobTmplString(_str,_iLen));
		else if(_str!=NULL)
		{
			this->__resize__(_iLen+1,false);
			__my_mem_cpy(this->dataPtr,_str,_iLen);
			this->dataPtr[_iLen]=0;
		}
		else
		{
			assert(0);
			this->__clear__();
		}
	}
	void __append__(const TypeChar *_str,int _iLen=-1)
	{
		if(_iLen<0) _iLen=__my_str_len(_str);
		if(this->_data_pointer_inside_(_str))
			this->append(TxBlobTmplString(_str,_iLen));
		else if(_str!=NULL)
		{
			int iStartPos=this->iLength;
			this->__resize__(_iLen+this->iLength+1,true);
			__my_mem_cpy(this->dataPtr+iStartPos,_str,_iLen);
			this->dataPtr[this->iLength]=0;
		}
		else
		{
			assert(0);
		}
	}
	void __pop_back__(int _iDiscardCount)
	{
		if(_iDiscardCount>=this->iLength)
			this->__clear__();
		else if(_iDiscardCount>0)
		{
			this->__resize__(this->iLength+1-_iDiscardCount,true);
			this->dataPtr[this->iLength]=0;
		}
	}
	bool __compare_equal_(const TypeChar *_str,int _iLen=-1) const
	{
		if(_iLen<0) _iLen=__my_str_len(_str);
		return _iLen==this->iLength&&::memcmp(this->dataPtr,_str,_iLen*sizeof(TypeChar))==0;
	}
	bool __no_case_compare_equal_(const TypeChar *_str,int _iLen=-1) const
	{
		if(_iLen<0) _iLen=__my_str_len(_str);
		if(_iLen!=this->iLength) return false;
		for(int pos=_iLen;pos--;)
		{
			int ch1=(int)this->dataPtr[pos];
			int ch2=(int)_str[pos];
			if('A'<=ch1&&ch1<='Z') ch1+='a'-'A';
			if('A'<=ch2&&ch2<='Z') ch2+='a'-'A';
			if(ch1!=ch2)
				return false;
		}
		return true;
	}
	int __compare_value_(const TypeChar *_str,int _iLen=-1) const
	{
		if(_iLen<0) _iLen=__my_str_len(_str);
		if(this->iLength<_iLen) return -1;
		else if(this->iLength>_iLen) return 1;
		int _ret=::memcmp(this->dataPtr,_str,_iLen*sizeof(TypeChar));
		if(_ret<0) return -1;
		else if(_ret>0) return 1;
		return 0;
	}
public :
	TxBlobTmplString():__forbid_assign__(&dataPtr) { __constructor_init__(); }
	TxBlobTmplString(const TypeChar *_str,int _iLen=-1):__forbid_assign__(&dataPtr) { __constructor_init__(); if(_str!=NULL) this->assign(_str,_iLen); }
	TxBlobTmplString(const TypeOpsChar *_str,int _iLen=-1):__forbid_assign__(&dataPtr) { __constructor_init__(); if(_str!=NULL) this->assign(_str,_iLen); }
	TxBlobTmplString(const StdString &_temp):__forbid_assign__(&dataPtr) { __constructor_init__(); this->assign(_temp.c_str(),_temp.length()); }
	TxBlobTmplString(const TxBlobTmplString &_temp):__forbid_assign__(&dataPtr) { __constructor_init__(); this->assign(_temp); }
	virtual ~TxBlobTmplString() { assert((void*)__forbid_assign__==(void*)&dataPtr); this->__clear__(); }
	TxBlobTmplString clone() const { return *this; }
	void assign(const TypeChar *_str,int _iLen=-1) { this->__assign__(_str,_iLen); }
	void assign(const TypeOpsChar *_str,int _iLen=-1) { this->__assign__((const TypeChar*)_str,_iLen); }
	void assign(const TxBlobTmplString &_tem) { this->__assign__(_tem.dataPtr,_tem.iLength); }
	void assign(const StdString &_tem) { this->__assign__(_tem.c_str(),(int)_tem.length()); }
	void assign(const PeerStrongTmplStringType &_tem) { this->clear(); this->append(_tem); }
	void assign(const TPeerTypeChar *_str,int _iLen=-1) { this->clear(); this->append(_str,_iLen); }
	void append(const TypeChar *_str,int _iLen=-1) { this->__append__(_str,_iLen); }
	void append(const TypeOpsChar *_str,int _iLen=-1) { this->__append__((const TypeChar*)_str,_iLen); }
	void append(const TxBlobTmplString &_tem) { this->__append__(_tem.dataPtr,_tem.iLength); }
	void append(const StdString &_tem) { this->__append__(_tem.c_str(),(int)_tem.length()); }
	void append(const PeerStrongTmplStringType &_tem) { this->append(_tem.c_str(),_tem.length()); }
	void append(const TPeerTypeChar *_str,int _iLen=-1)
	{
		if(_iLen<0) _iLen=PeerStrongTmplStringType::__my_str_len(_str);
		int iLcOldLen=this->iLength;
		this->__resize__(iLcOldLen+_iLen+1,true);
		TypeChar *pLcDataPtr=this->dataPtr+iLcOldLen;
		for(int i=_iLen;i--;)
			pLcDataPtr[i]=(TypeChar)_str[i];
		pLcDataPtr[_iLen]=0;
	}
	void makeChunkBuffer(int _size,const TypeChar &_ch=*(const TypeChar*)NULL)
	{
		this->__resize__(_size+1,false);
		if(NULL!=&_ch)
			__my_mem_set(this->dataPtr,this->iLength,_ch);
		this->dataPtr[this->iLength]=0;
	}
	void resize(const int _iSize,const bool _bKeepOrgn)
	{
		this->__resize__(_iSize+1,_bKeepOrgn);
		this->dataPtr[this->iLength]=0;
	}
	int compare(const TypeChar *_str,int _iLen=-1) const
	{
		return this->__compare_value_(_str,_iLen);
	}
	int compare(const TxBlobTmplString &_tem) const
	{
		return this->__compare_value_(_tem,_tem.iLength);
	}
	bool equalNoCase(const TypeChar *_str,int _iLen=-1) const
	{
		return this->__no_case_compare_equal_(_str,_iLen);
	}
	bool equalNoCase(const TxBlobTmplString &_tem) const
	{
		return this->__no_case_compare_equal_(_tem,_tem.iLength);
	}
	void pop_back(int _iDiscardCount) { this->__pop_back__(_iDiscardCount); }
	void clear() { this->__clear__(); }
	inline const TypeChar* data() const { return this->dataPtr; }
	inline const TypeChar* c_str() const { return this->dataPtr; }
	inline int length() const { return this->iLength; }
	inline int size() const { return this->iLength; }
	TxBlobTmplString& operator=(const TxBlobTmplString &_tem) { this->assign(_tem); return *this; }
	TxBlobTmplString& operator=(const StdString &_tem) { this->assign(_tem); return *this; }
	TxBlobTmplString& operator=(const TypeChar *_str) { this->assign(_str); return *this; }
	TxBlobTmplString& operator=(const TypeOpsChar *_str) { this->assign(_str); return *this; }
	TxBlobTmplString& operator=(const PeerStrongTmplStringType &_tem) { this->assign(_tem); return *this; }
	TxBlobTmplString& operator=(const TPeerTypeChar *_str) { this->assign(_str); return *this; }
	TxBlobTmplString& operator+=(const StdString &_tem) { this->append(_tem); return *this; }
	TxBlobTmplString& operator+=(const TxBlobTmplString &_tem) { this->append(_tem); return *this; }
	TxBlobTmplString& operator+=(const TypeChar *_str) { this->append(_str); return *this; }
	TxBlobTmplString& operator+=(const TypeOpsChar *_str) { this->append(_str); return *this; }
	TxBlobTmplString& operator+=(const PeerStrongTmplStringType &_tem) { this->append(_tem); return *this; }
	TxBlobTmplString& operator+=(const TPeerTypeChar *_str) { this->append(_str); return *this; }
	TxBlobTmplString operator+(const TxBlobTmplString &_tem) const { TxBlobTmplString ret(*this); ret.append(_tem); return ret; }
	TxBlobTmplString operator+(const StdString &_tem) const { TxBlobTmplString ret(*this); ret.append(_tem); return ret; }
	TxBlobTmplString operator+(const TypeChar *_str) const { TxBlobTmplString ret(*this); ret.append(_str); return ret; }
	TxBlobTmplString operator+(const TypeOpsChar *_str) const { TxBlobTmplString ret(*this); ret.append(_str); return ret; }
	TxBlobTmplString operator+(const PeerStrongTmplStringType &_tem) const { TxBlobTmplString ret(*this); ret.append(_tem); return ret; }
	TxBlobTmplString operator+(const TPeerTypeChar *_str) const { TxBlobTmplString ret(*this); ret.append(_str); return ret; }
	bool operator==(const TxBlobTmplString &_tem) const { return this->__compare_equal_(_tem.dataPtr,_tem.iLength); }
	bool operator==(const TypeChar _str[]) const { return this->__compare_equal_(_str); }
	bool operator!=(const TxBlobTmplString &_tem) const { return !this->__compare_equal_(_tem.dataPtr,_tem.iLength); }
	bool operator!=(const TypeChar _str[]) const { return !this->__compare_equal_(_str); }
	bool operator<(const TxBlobTmplString &_tem) const { return this->__compare_value_(_tem.dataPtr,_tem.iLength)<0; }
	bool operator<(const TypeChar _str[]) const { return this->__compare_value_(_str)<0; }
	TxBlobTmplString left(int _nCount) const
	{
		return TxBlobTmplString(this->dataPtr,__min_int_(_nCount,this->iLength));
	}
	TxBlobTmplString right(int _nCount) const
	{
		if(_nCount<=0) return TxBlobTmplString();
		else if(_nCount>=this->iLength) return *this;
		return TxBlobTmplString(this->dataPtr+this->iLength-_nCount,_nCount);
	}
	TxBlobTmplString middle(int _nStart,int _nCount) const
	{
		if(_nStart>=this->iLength||_nCount<=0) return TxBlobTmplString();
		return TxBlobTmplString(this->dataPtr+_nStart,__min_int_(_nCount,this->iLength-_nStart));
	}
	TxBlobTmplString& lowerCaseSelf()
	{
		for(int pos=this->iLength;pos--;)
		{
			int ch=this->dataPtr[pos];
			if(ch>='A'&&ch<='Z')
				this->dataPtr[pos]+='a'-'A';
		}
		return *this;
	}
	TxBlobTmplString& upperCaseSelf()
	{
		for(int pos=this->iLength;pos--;)
		{
			int ch=this->dataPtr[pos];
			if(ch>='a'&&ch<='z')
				this->dataPtr[pos]+='A'-'z';
		}
		return *this;
	}
	void replaceSelf(TypeChar target,TypeChar sample1)
	{
		for(int pos=this->iLength;pos--;)
		{
			if(this->dataPtr[pos]==sample1)
				this->dataPtr[pos]=target;
		}
	}
	void replaceSelf(TypeChar target,TypeChar sample1,TypeChar sample2)
	{
		for(int pos=this->iLength;pos--;)
		{
			int ch=this->dataPtr[pos];
			if(ch==(int)sample1||ch==(int)sample2)
				this->dataPtr[pos]=target;
		}
	}
	void replaceSelf(TypeChar target,TypeChar sample1,TypeChar sample2,TypeChar sample3)
	{
		for(int pos=this->iLength;pos--;)
		{
			int ch=this->dataPtr[pos];
			if(ch==(int)sample1||ch==(int)sample2||ch==(int)sample3)
				this->dataPtr[pos]=target;
		}
	}
	int format(const TypeChar fmt[],...)
	{
		TypeChar buffer[8*1024/(sizeof(TypeChar))],*pBuf=buffer;
		int bufLen=sizeof(buffer)/sizeof(buffer[0]);
		int res=0;
		for(;;)
		{
			va_list args;
			va_start (args, fmt);
			res=__my_vsn_printf(pBuf,bufLen-1,fmt, args);
			va_end(args);
			if(res<0)
			{
				if(pBuf!=(TypeChar*)buffer) delete[] pBuf;
				pBuf=new TypeChar[(bufLen*=2)];
			}
			else break;
		}
		this->assign(pBuf,res);
		if((void*)pBuf!=(void*)buffer) delete[] pBuf;
		return res;
	}
	int appendFormat(const TypeChar fmt[],...)
	{
		TypeChar buffer[8*1024/(sizeof(TypeChar))],*pBuf=buffer;
		int bufLen=sizeof(buffer)/sizeof(buffer[0]);
		int res=0;
		for(;;)
		{
			va_list args;
			va_start (args, fmt);
			res=__my_vsn_printf(pBuf,bufLen,fmt,args);
			//res=vsnprintf(pBuf,bufLen,fmt, args);
			va_end(args);
			if(res<0)
			{
				if(pBuf!=(TypeChar*)buffer) delete[] pBuf;
				pBuf=new TypeChar[(bufLen*=2)];
			}
			else break;
		}
		this->append(pBuf,res);
		if((void*)pBuf!=(void*)buffer) delete[] pBuf;
		return res;
	}
	TxBlobTmplString trimLeft(const TypeChar _str[]) const
	{
		int _strLen=__my_str_len(_str);
		const TypeChar *p=this->dataPtr-1;
		while(__my_mem_chr(_str,*(++p),_strLen)!=NULL) ;
		return TxBlobTmplString(p);
	}
	TxBlobTmplString trimRight(const TypeChar _str[]) const
	{
		int _strLen=__my_str_len(_str);
		const TypeChar *pStart=this->dataPtr;
		const TypeChar *p=this->dataPtr+this->iLength;
		while((--p)>=pStart&&__my_mem_chr(_str,*p,_strLen)!=NULL) ;
		return TxBlobTmplString(pStart,(int)(p-pStart+1));
	}
	TxBlobTmplString trimLR(const TypeChar _str[]) const
	{
		const TypeChar *p;
		int _strLen=__my_str_len(_str);
		for(p=this->dataPtr-1;__my_mem_chr(_str,*(++p),_strLen)!=NULL; ) ;
		const TypeChar *pStart=p;
		for(p=this->dataPtr+this->iLength;(--p)>=pStart&&__my_mem_chr(_str,*p,_strLen)!=NULL;) ;
		return TxBlobTmplString(pStart,(int)(p-pStart+1));
	}
	TxBlobTmplString& trimSelfRight(const TypeChar _str[])
	{
		int _strLen=__my_str_len(_str);
		TypeChar *pStart=this->dataPtr;
		TypeChar *p=this->dataPtr+this->iLength;
		while((--p)>=pStart&&__my_mem_chr(_str,*p,_strLen)!=NULL) ;
		this->pop_back(this->iLength-(int)(p-pStart+1));
		return *this;
	}
	inline TypeChar frontChar() const
	{
		return this->iLength>0?this->dataPtr[0]:0;
	}
	inline TypeChar backChar() const
	{
		return this->iLength>0?this->dataPtr[this->iLength-1]:0;
	}
public :
	std::list<TxBlobTmplString> splitSegment(const TypeChar *_sample0,const TypeChar *_sample1=NULL,const TypeChar *_sample2=NULL,const TypeChar *_sample3=NULL,
		const TypeChar *_sample4=NULL,const TypeChar *_sample5=NULL,const TypeChar *_sample6=NULL,const TypeChar *_sample7=NULL) const
	{
		std::list<TxBlobTmplString> ret;
		struct tagSpSeg{
			const TypeChar *sample;
			int len;
			tagSpSeg() { this->len=0; this->sample=NULL; }
			void set(const TypeChar *_sample) { this->len=((this->sample=_sample)==NULL?0:(int)::strlen(_sample)); }
			bool operator<(const tagSpSeg &_tem) const { return this->len>_tem.len; }
		} arg[8];
		arg[0].set(_sample0);
		arg[1].set(_sample1);
		arg[2].set(_sample2);
		arg[3].set(_sample3);
		arg[4].set(_sample4);
		arg[5].set(_sample5);
		arg[6].set(_sample6);
		arg[7].set(_sample7);
		std::sort(&arg[0],&arg[sizeof(arg)/sizeof(arg[0])]);

		int lcStartPos=0;
		for(int pos=0;pos<this->iLength;pos++)
		{
			for(int i=0;i<sizeof(arg)/sizeof(arg[0]);i++)
			{
				if(arg[i].len<=0) break;
				if(arg[i].len<=this->iLength-pos&&::memcmp(&this->dataPtr[pos],arg[i].sample,arg[i].len)==0)
				{
					if(pos>lcStartPos)
					{
						ret.push_back(TxBlobTmplString());
						ret.back().assign(&this->dataPtr[lcStartPos],pos-lcStartPos);
					}
					lcStartPos=(pos+=arg[i].len);
					break;
				}
			}
		}
		if(this->iLength>lcStartPos)
		{
			ret.push_back(TxBlobTmplString());
			ret.back().assign(&this->dataPtr[lcStartPos],this->iLength-lcStartPos);
		}
		return ret;
	}
public :
	bool all_is_digit() const
	{
		for(int lc_pos=this->iLength;lc_pos--;)
		{
			int ch=(int)this->dataPtr[lc_pos];
			if(ch<'0'||ch>'9')
				return false;
		}
		return true;
	}
	bool all_is_alpha() const
	{
		for(int lc_pos=this->iLength;lc_pos--;)
		{
			int ch=(int)this->dataPtr[lc_pos];
			if((ch<'a'||ch>'z')&&(ch<'A'||ch>'Z'))
				return false;
		}
		return true;
	}
	bool all_is_alpha_or_digit() const
	{
		for(int lc_pos=this->iLength;lc_pos--;)
		{
			int ch=(int)this->dataPtr[lc_pos];
			if((ch<'a'||ch>'z')&&(ch<'A'||ch>'Z')&&(ch<'0'||ch>'9'))
				return false;
		}
		return true;
	}
public:
	bool saveFile(const char *fileName) const
	{
		return this->_DoSaveTssAppendWriteFile_(fileName,"wb");
	}
	bool appendToWriteFile(const char *_fileName) const
	{
		return this->_DoSaveTssAppendWriteFile_(_fileName,"ab+");
	}
	bool loadFile(const char *fileName)
	{
		FILE *fp=NULL;
		long pos=0;
		bool res=false;
		if((fp=fopen(fileName,"rb"))!=NULL&&fseek(fp,0,SEEK_END)==0&&(pos=ftell(fp))>=0&&(pos%sizeof(TypeChar)==0)&&fseek(fp,0,SEEK_SET)==0)
		{
			pos/=sizeof(TypeChar);
			__resize__(pos+1,false);
			if(pos==((long)fread(this->dataPtr,1,pos*sizeof(TypeChar)+1,fp)))
			{
				this->dataPtr[this->iLength]=0;
				res=true;
			}
			else this->__clear__();
		}
		if(fp!=NULL) fclose(fp);
		return res;
	}
public:
	template<int N> inline static bool txStrNCopy(TypeChar (&_dest)[N],const TypeChar _src[])
	{
		bool ret=true; assert(N>0);
		int len=__my_str_len((TypeChar*)_src)+1;
		if(len>N) ret=false,len=N-1,_dest[N-1]=0;
		__my_mem_cpy(_dest,_src,len); return ret;
	}
	template<int N> inline static bool txStrNCopy(TypeChar (&_dest)[N],const TypeOpsChar _src[])
	{
		bool ret=true; assert(N>0);
		int len=__my_str_len((TypeChar*)_src)+1;
		if(len>N) ret=false,len=N-1,_dest[N-1]=0;
		__my_mem_cpy(_dest,_src,len); return ret;
	}
	template<int N> inline static bool txStrNCopy(TypeOpsChar (&_dest)[N],const TypeChar _src[])
	{
		bool ret=true; assert(N>0);
		int len=__my_str_len((TypeChar*)_src)+1;
		if(len>N) ret=false,len=N-1,_dest[N-1]=0;
		__my_mem_cpy(_dest,_src,len); return ret;
	}
	template<int N> inline static bool txStrNCopy(TypeOpsChar (&_dest)[N],const TypeOpsChar _src[])
	{
		bool ret=true; assert(N>0);
		int len=__my_str_len((TypeChar*)_src)+1;
		if(len>N) ret=false,len=N-1,_dest[N-1]=0;
		__my_mem_cpy(_dest,_src,len); return ret;
	}

	template<int N> inline static bool txSnprintf(TypeChar (&_dest)[N], const TypeChar *format, ...)
	{
		assert(N>0);
		va_list args;
		va_start(args,format);
		int ret=__my_vsn_printf((TypeChar*)_dest,N-1,format,args);
		va_end(args);
		_dest[N-1]=0;
		return ret>=0;
	}
	template<int N> inline static bool txSnprintf(TypeOpsChar (&_dest)[N], const TypeChar *format, ...)
	{
		assert(N>0);
		va_list args;
		va_start(args,format);
		int ret=__my_vsn_printf((TypeChar*)_dest,N-1,format,args);
		va_end(args);
		_dest[N-1]=0;
		return ret>=0;
	}
public :
	template<class RET>
	RET toDatumRet(const RET &_defaultVal=*(RET*)NULL) const
	{
		RET ret;
		if(!this->toDatum(ret)&&NULL==&_defaultVal)
			ret=_defaultVal;
		return ret;
	}
	template<class NumType>
	bool toDatum(NumType &out_num,int offset=0) const
	{
		bool res=false;
		if(this==NULL||(unsigned int)offset>(unsigned int)this->iLength) return false;
		if(TxCppPlatform::is_integral<NumType>::value||TxCppPlatform::is_enum<NumType>::value)//整数
		{
			if(TxCppPlatform::is_signed<NumType>::value)//有符号
			{
				if(sizeof(NumType)<=sizeof(int))
				{
					if(sizeof(TypeChar)==sizeof(char))
						res=(::sscanf((const char*)(this->dataPtr+offset),"%d",(int*)&out_num)==1);
					else
						res=(::swscanf((const wchar_t*)(this->dataPtr+offset),L"%d",(int*)&out_num)==1);
				}
				else if(sizeof(NumType)<=sizeof(long long int))
				{
					if(sizeof(TypeChar)==sizeof(char))
						res=(::sscanf((const char*)(this->dataPtr+offset),"%lld",(long long int*)&out_num)==1);
					else
						res=(::swscanf((const wchar_t*)(this->dataPtr+offset),L"%lld",(long long int*)&out_num)==1);
				}
			}
			else//没有符号
			{
				if(sizeof(NumType)<=sizeof(unsigned int))
				{
					if(sizeof(TypeChar)==sizeof(char))
						res=(::sscanf((const char*)(this->dataPtr+offset),"%u",(unsigned int*)&out_num)==1);
					else
						res=(::swscanf((const wchar_t*)(this->dataPtr+offset),L"%u",(unsigned int*)&out_num)==1);
				}
				else if(sizeof(NumType)<=sizeof(unsigned long long int))
				{
					if(sizeof(TypeChar)==sizeof(char))
						res=(::sscanf((const char*)(this->dataPtr+offset),"%u",(unsigned long long int*)&out_num)==1);
					else
						res=(::swscanf((const wchar_t*)(this->dataPtr+offset),L"%u",(unsigned long long int*)&out_num)==1);
				}
			}
		}
		else if(TxCppPlatform::is_floating_point<NumType>::value)//浮点数
		{
			if(__nsBlobTmplString_Template_::TxIsTypeFloat<NumType>::value)
			{
				if(sizeof(TypeChar)==sizeof(char)) res=(::sscanf((const char*)(this->dataPtr+offset),"%f",(float*)&out_num)==1);
				else res=(::swscanf((const wchar_t*)(this->dataPtr+offset),L"%f",&out_num)==1);
			}
			else if(__nsBlobTmplString_Template_::TxIsTypeDouble<NumType>::value)
			{
				if(sizeof(TypeChar)==sizeof(char)) res=(::sscanf((const char*)(this->dataPtr+offset),"%lg",(double*)&out_num)==1);
				else res=(::swscanf((const wchar_t*)(this->dataPtr+offset),L"%lg",&out_num)==1);
			}
			else if(__nsBlobTmplString_Template_::TxIsTypeLongDouble<NumType>::value)
			{
				if(sizeof(TypeChar)==sizeof(char)) res=(::sscanf((const char*)(this->dataPtr+offset),"%Lg",(long double*)&out_num)==1);
				else res=(::swscanf((const wchar_t*)(this->dataPtr+offset),L"%Lg",&out_num)==1);
			}
			else assert(0);
		}
		else
		{
			assert(TxCppPlatform::is_floating_point<NumType>::value||TxCppPlatform::is_integral<NumType>::value);
			assert(0);
		}
		return res;
	}
	bool toDatum(bool &out_num,int offset=0) const
	{
		bool res=false;
		if(offset<=this->iLength)
		{
			int n=(!!out_num);
			if(sizeof(TypeChar)==sizeof(char)) res=(::sscanf((const char*)(this->dataPtr+offset),"%d",&n)==1);
			else res=(::swscanf((const wchar_t*)(this->dataPtr+offset),L"%d",&n)==1);
			out_num=!!n;
		}
		return res;
	}
	template<int N>
	bool toDatum(TypeChar (&out_num)[N],int offset=0) const
	{
		bool res=false;
		if(offset<=this->iLength)
			res=txStrNCopy(out_num,this->dataPtr+offset);
		else out_num[0]=0;
		return res;
	}
	template<int N>
	bool toDatum(TypeOpsChar (&out_num)[N],int offset=0) const
	{
		bool res=false;
		if(offset<=this->iLength)
			res=txStrNCopy(out_num,this->dataPtr+offset);
		else out_num[0]=0;
		return res;
	}
	bool toDatum(TxBlobTmplString& out_str,int offset=0) const
	{
		bool res=false;
		if(offset<=this->iLength)
		{
			out_str=(this->dataPtr+offset);
			res=true;
		}
		else
		{
			out_str.clear();
		}
		return res;
	}
	bool toDatum(StdPeerString& out_str,int offset=0) const
	{
		bool res=false;
		if(offset<=this->iLength)
		{
			PeerStrongTmplStringType mLcTem;
			mLcTem.assign(this->dataPtr+offset);
			out_str.assign(mLcTem.c_str(),mLcTem.iLength);
			res=true;
		}
		else
		{
			out_str.clear();
		}
		return res;
	}
	bool toDatum(PeerStrongTmplStringType& out_str,int offset=0) const
	{
		bool res=false;
		if(offset<=this->iLength)
		{
			out_str.assign(this->dataPtr+offset);
			res=true;
		}
		else
		{
			out_str.clear();
		}
		return res;
	}
	bool toDatum(StdString& out_str,int offset=0) const
	{
		bool res=false;
		if(offset<=this->iLength)
		{
			out_str=(this->dataPtr+offset);
			res=true;
		}
		else
		{
			out_str.clear();
		}
		return res;
	}
public :
	template<class NumType>
	static TxBlobTmplString fromDatum(const NumType &num)
	{
		TypeChar outBuffStr[96]; outBuffStr[0]=0;
		if(TxCppPlatform::is_integral<NumType>::value||TxCppPlatform::is_enum<NumType>::value)//整数
		{
			if(TxCppPlatform::is_signed<NumType>::value)//有符号
			{
				assert(sizeof(long long int)>=sizeof(num));
				if(sizeof(NumType)<=sizeof(int))
					txSnprintf(outBuffStr,sizeof(TypeChar)==sizeof(char)?(TypeChar*)"%d":(TypeChar*)L"%d",(int)num);
				else
					txSnprintf(outBuffStr,sizeof(TypeChar)==sizeof(char)?(TypeChar*)"%lld":(TypeChar*)L"%lld",(long long int)num);
			}
			else//没有符号
			{
				assert(sizeof(unsigned long long int)>=sizeof(num));
				if(sizeof(NumType)<=sizeof(unsigned int))
					txSnprintf(outBuffStr,sizeof(TypeChar)==sizeof(char)?(TypeChar*)"%u":(TypeChar*)L"%u",(unsigned int)num);
				else
					txSnprintf(outBuffStr,sizeof(TypeChar)==sizeof(char)?(TypeChar*)"%llu":(TypeChar*)L"%llu",(unsigned long long int)num);
			}
		}
		else if(TxCppPlatform::is_floating_point<NumType>::value)//浮点数
		{
			assert(sizeof(long double)>=sizeof(num));
			if(__nsBlobTmplString_Template_::TxIsTypeLongDouble<NumType>::value)
				txSnprintf(outBuffStr,sizeof(TypeChar)==sizeof(char)?(TypeChar*)"%.64Lg":(TypeChar*)L"%.64Lg",(long double)num);
			else if(__nsBlobTmplString_Template_::TxIsTypeDouble<NumType>::value||__nsBlobTmplString_Template_::TxIsTypeFloat<NumType>::value)
				txSnprintf(outBuffStr,sizeof(TypeChar)==sizeof(char)?(TypeChar*)"%.64g":(TypeChar*)L"%.64g",(double)num);
			else assert(0);
		}
		else
		{
			assert(TxCppPlatform::is_floating_point<NumType>::value||TxCppPlatform::is_integral<NumType>::value||TxCppPlatform::is_enum<NumType>::value);
			assert(0);
		}
		return TxBlobTmplString(outBuffStr);
	}

	static TxBlobTmplString fromDatum(const TypeOpsChar *num) { return TxBlobTmplString(num); }
	static TxBlobTmplString fromDatum(const TypeChar *num) { return TxBlobTmplString(num); }
	static TxBlobTmplString fromDatum(TypeOpsChar *num) { return TxBlobTmplString(num); }
	static TxBlobTmplString fromDatum(TypeChar *num) { return TxBlobTmplString(num); }
	static TxBlobTmplString fromDatum(const StdString &num) { return TxBlobTmplString(num); }
	static TxBlobTmplString fromDatum(StdString &num) { return TxBlobTmplString(num); }
	static TxBlobTmplString fromDatum(const TxBlobTmplString &num) { return num; }
	static TxBlobTmplString fromDatum(TxBlobTmplString &num) { return num; }

	static TxBlobTmplString fromDatum(const TPeerTypeChar *num) { TxBlobTmplString ret; ret.assign(num); return ret; }
	static TxBlobTmplString fromDatum(const TPeerTypeOpsChar *num) { TxBlobTmplString ret; ret.assign(num); return ret; }
	static TxBlobTmplString fromDatum(TPeerTypeChar *num) { TxBlobTmplString ret; ret.assign(num); return ret; }
	static TxBlobTmplString fromDatum(TPeerTypeOpsChar *num) { TxBlobTmplString ret; ret.assign(num); return ret; }
	static TxBlobTmplString fromDatum(const PeerStrongTmplStringType &num) { TxBlobTmplString ret; ret.assign(num); return ret; }
	static TxBlobTmplString fromDatum(PeerStrongTmplStringType &num) { TxBlobTmplString ret; ret.assign(num); return ret; }
	static TxBlobTmplString fromDatum(const StdPeerString &num) { TxBlobTmplString ret; ret.assign(num); return ret; }
	static TxBlobTmplString fromDatum(StdPeerString &num) { TxBlobTmplString ret; ret.assign(num); return ret; }
};

typedef TxBlobTmplString<char,unsigned char,std::string,wchar_t,__nsBlobTmplString_Template_::TxSizeSignedInteger<sizeof(wchar_t)>::type,std::wstring> TxBlobString;
typedef TxBlobTmplString<wchar_t,__nsBlobTmplString_Template_::TxSizeSignedInteger<sizeof(wchar_t)>::type,std::wstring,char,unsigned char,std::string> TxBlobWString;

#if(DefBoolCodeEnableUnicode==0)
typedef std::string CEncodeString;
typedef TxBlobString TxEncodeString;
#else
typedef std::wstring CEncodeString;
typedef TxBlobWString TxEncodeString;
#endif

/*
int main()
{
	TxString sss;
	sss.format(" 512.3 sfd%d,%s,",123456789,"abc");
	sss.appendFormat("sfd%d,%s,		",123456789,"abc");
	TxString sss1=sss.trimLR("	 ");
	double val=sss.toDatumRet<double>();
	int64_t val64=sss.toDatumRet<int>();
	return 0;
}
*/
