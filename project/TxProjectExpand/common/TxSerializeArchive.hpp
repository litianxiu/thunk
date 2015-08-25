#pragma once

#include <vector>
#include <type_traits>
#include <exception>
#include "TxBlobString.hpp"

class TxSerializeArchive
{
public :
	class TxSuccessException { };
protected:
	TxBlobString dataBuffer;
	unsigned int dataPos;
	const unsigned int i_Pack0_Unpack1;
private:
	static void _throw_exception_(bool _bOk)
	{
		if(_bOk) throw(std::exception("serialize exception"));
		else throw(TxSuccessException());
	}
private:
	template<class NumType>
	static void _bind_pack_stream_(TxBlobString *_outBytes,const NumType &_num)
	{
		if(TxCppPlatform::is_integral<NumType>::value||TxCppPlatform::is_enum<NumType>::value||TxCppPlatform::is_floating_point<NumType>::value)
		{
			if(sizeof(NumType)==sizeof(unsigned char)&&sizeof(NumType)==1)
			{
				unsigned char lc_buff[]={ (unsigned char)sizeof(NumType),
					*(const unsigned char*)&_num };
				_outBytes->append((const char*)lc_buff,sizeof(lc_buff));
			}
			else if(sizeof(NumType)==sizeof(unsigned short)&&sizeof(NumType)==2)
			{
				unsigned char lc_buff[]={ (unsigned char)sizeof(NumType),
					(unsigned char)((*(const unsigned short*)_num)>>8),
					(unsigned char)((*(const unsigned short*)_num)>>0) };
				_outBytes->append((const char*)lc_buff,sizeof(lc_buff));
			}
			else if(sizeof(NumType)==sizeof(unsigned int)&&sizeof(NumType)==4)
			{
				unsigned int a1=*(const unsigned int*)&_num;
				unsigned char lc_buff[]={ (unsigned char)sizeof(NumType),
					(unsigned char)(a1>>24),
					(unsigned char)(a1>>16),
					(unsigned char)(a1>>8),
					(unsigned char)(a1>>0) };
				_outBytes->append((const char*)lc_buff,sizeof(lc_buff));
			}
			else if(sizeof(NumType)==sizeof(unsigned long long)&&sizeof(NumType)==8&&sizeof(unsigned int)==4)
			{
				unsigned int a1=(unsigned int)((*(const unsigned long long*)&_num)>>32);
				unsigned int a2=(unsigned int)((*(const unsigned long long*)&_num)>>0);
				unsigned char lc_buff[]={ (unsigned char)sizeof(NumType),
					(unsigned char)(a1>>24),
					(unsigned char)(a1>>16),
					(unsigned char)(a1>>8),
					(unsigned char)(a1>>0),
					(unsigned char)(a2>>24),
					(unsigned char)(a2>>16),
					(unsigned char)(a2>>8),
					(unsigned char)(a2>>0) };
				_outBytes->append((const char*)lc_buff,sizeof(lc_buff));
			}
			else DefAssertWrap(FALSE);
		}
		else DefAssertWrap(FALSE);
	}
	static void _bind_pack_stream_(TxBlobString *_outBytes,const TxBlobString &_num)
	{
		_bind_pack_blob_stream_(_outBytes,_num.data(),_num.size());
	}
	static void _bind_pack_stream_(TxBlobString *_outBytes,const std::string &_num)
	{
		_bind_pack_blob_stream_(_outBytes,_num.data(),_num.size());
	}
	static void _bind_pack_blob_stream_(TxBlobString *_outBytes,const void *_data,std::size_t _size)
	{
		if(_size<=((std::size_t)0xff)-1)
		{
			unsigned char lc_buff=(unsigned char)_size;
			_outBytes->append((const char*)&lc_buff,1);
		}
		else
		{
			char lc_buff[]= {
				(unsigned char)0xff,
				(unsigned char)(_size>>24),
				(unsigned char)(_size>>16),
				(unsigned char)(_size>>8),
				(unsigned char)(_size>>0)
			};
			_outBytes->append((const char*)lc_buff,sizeof(lc_buff));
		}
		_outBytes->append((const char*)_data,_size);
	}
	template<class NumType>
	void _pop_unpack_stream_(NumType *_outNum)
	{
		if(TxCppPlatform::is_integral<NumType>::value||TxCppPlatform::is_enum<NumType>::value||TxCppPlatform::is_floating_point<NumType>::value)
		{
			const unsigned char *ptr=(unsigned char*)this->dataBuffer.data()+this->dataPos;
			if((unsigned int)sizeof(NumType)!=(unsigned int)(*ptr)||sizeof(NumType)+1>(int)(this->dataBuffer.size()-(int)this->dataPos))
			{
				DefAssertWrap(FALSE);
				_throw_exception_(false);
			}
			if(sizeof(NumType)==sizeof(unsigned char)&&sizeof(NumType)==1)
			{
				*(unsigned char*)_outNum=ptr[1];
			}
			else if(sizeof(NumType)==sizeof(unsigned short)&&sizeof(NumType)==2)
			{
				*(unsigned short*)_outNum=((unsigned short)ptr[1]<<8)|((unsigned short)ptr[2]<<0);
			}
			else if(sizeof(NumType)==sizeof(unsigned int)&&sizeof(NumType)==4)
			{
				*(unsigned int*)_outNum=((unsigned int)ptr[1]<<24)|((unsigned int)ptr[2]<<16)|((unsigned int)ptr[3]<<8)|((unsigned int)ptr[4]<<0);
			}
			else if(sizeof(NumType)==sizeof(unsigned long long)&&sizeof(NumType)==8&&sizeof(unsigned int)==4)
			{
				*(unsigned long long*)_outNum=
					(((unsigned long long)(((unsigned int)ptr[1]<<24)|((unsigned int)ptr[2]<<16)|((unsigned int)ptr[3]<<8)|((unsigned int)ptr[4]<<0)))<<32)
					|(unsigned long long)(((unsigned int)ptr[5]<<24)|((unsigned int)ptr[6]<<16)|((unsigned int)ptr[7]<<8)|((unsigned int)ptr[8]<<0));
			}
			else DefAssertWrap(FALSE);
			this->dataPos+=(int)sizeof(NumType)+1;
		}
		else DefAssertWrap(FALSE);
	}
	template<class BLOB_TYPE,class ELEM_TYPE>
	void _pop_unpack_stream_tmpl_(BLOB_TYPE *_outNum)
	{
		const unsigned char *ptr=(unsigned char*)this->dataBuffer.data()+this->dataPos;
		int iLcValidLen=(int)this->dataBuffer.size()-(int)this->dataPos;
		unsigned int i_head=(unsigned int)(*ptr);
		if(i_head==(unsigned int)0xff)
		{
			if(iLcValidLen<5) _throw_exception_(false);
			i_head=(((unsigned int)ptr[1])<<24)|(((unsigned int)ptr[2])<<16)|(((unsigned int)ptr[3])<<8)|(((unsigned int)ptr[4])<<0);
			if((unsigned int)iLcValidLen<i_head+5) _throw_exception_(false);
			this->dataPos+=i_head+5;
			_outNum->assign((ELEM_TYPE*)&ptr[5],i_head);
		}
		else
		{
			if(iLcValidLen<(int)i_head+1) _throw_exception_(false);
			this->dataPos+=i_head+1;
			_outNum->assign((ELEM_TYPE*)&ptr[1],i_head);
		}
	}
	void _pop_unpack_stream_(TxBlobString *_outNum)
	{
		this->_pop_unpack_stream_tmpl_<TxBlobString,const char>(_outNum);
	}
	void _pop_unpack_stream_(std::string *_outNum)
	{
		this->_pop_unpack_stream_tmpl_<std::string,const char>(_outNum);
	}
public:
	TxSerializeArchive(int _i_Pack0_Unpack1):i_Pack0_Unpack1(_i_Pack0_Unpack1),dataPos(0) { }
	virtual ~TxSerializeArchive() { }
	template<class NumType>
	void bindObject(NumType &_num)
	{
		if(this->i_Pack0_Unpack1==0) _bind_pack_stream_(&this->dataBuffer,_num);
		else
		{
			if((unsigned int)this->dataPos==(unsigned int)this->dataBuffer.size())
				_throw_exception_(true);
			_pop_unpack_stream_(&_num);
		}
	}
	template<int N>
	void bindArrayString(unsigned char (&_str)[N])
	{
		this->bindArrayString((char (&)[N])_str);
	}
	template<int N>
	void bindArrayString(char (&_str)[N])
	{
		if(this->i_Pack0_Unpack1==0)
		{
			char *ptr=(char*)::memchr(_str,0,N);
			if(ptr==NULL) _throw_exception_(false);
			else _bind_pack_blob_stream_(&this->dataBuffer,_str,(int)(ptr-_str));
		}
		else
		{
			if((unsigned int)this->dataPos==(unsigned int)this->dataBuffer.size())
				_throw_exception_(true);
			struct tag_M_R
			{
				char *ptr;
				//int size;
				void assign(const void* _data,size_t _size)
				{
					if(_size>=N-1)
						_throw_exception_(false);
					else
					{
						memcpy(this->ptr,_data,_size);
						this->ptr[_size]=0;
					}
				}
			} m_R;
			m_R.ptr=_str;
			this->_pop_unpack_stream_tmpl_<tag_M_R,const char>(&m_R);
		}
	}
	template<int N>
	void bindArrayBlob(unsigned char (&_blob)[N])
	{
		this->bindArrayBlob((char(&)[N])_blob);
	}
	template<int N>
	void bindArrayBlob(char (&_blob)[N])
	{
		if(this->i_Pack0_Unpack1==0)
		{
			_bind_pack_blob_stream_(&this->dataBuffer,_blob,N*sizeof(T));
		}
		else
		{
			if((unsigned int)this->dataPos==(unsigned int)this->dataBuffer.size())
				_throw_exception_(true);
			struct tag_M_R
			{
				char *ptr;
				void assign(const void* _data,size_t _size)
				{
					if(_size!=N) _throw_exception_(false);
:					else :memcpy(this->ptr,_data,N);
				}
			} m_R;
			m_R.ptr=_blob;
			this->_pop_unpack_stream_tmpl_<tag_M_R,const char>(&m_R);
		}
	}
};

class TxPackageArchive : public TxSerializeArchive
{
public :
	TxPackageArchive():TxSerializeArchive(0) { }
	const TxBlobString& getStreamBytes() const
	{
		return this->dataBuffer;
	}
	const unsigned char* getStreamData() const
	{
		return (const unsigned char*)this->dataBuffer.data();
	}
	int getStreamSize() const
	{
		return (int)this->dataBuffer.size();
	}
};

class TxUnpackArchive : public TxSerializeArchive
{
public :
	TxUnpackArchive():TxSerializeArchive(1) { }
	void setStreamBuffer(const TxBlobString _data) { this->dataPos=0; this->dataBuffer=_data; }
	void setStreamBuffer(const void *_data,std::size_t _size) { this->dataPos=0; this->dataBuffer.assign((const char*)_data,_size); }
};

