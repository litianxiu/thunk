#pragma once

#include <stdio.h>

class TxCrc32CheckCode
{
private:
	typedef unsigned int uint32;
	uint32 val_crc;
private:
	static void _g_pushData(TxCrc32CheckCode *_pThis,const unsigned char *_data,int _size)
	{
		class MyCrc32Gfunc
		{
		private:
			uint32 m_CRC32[256];
		public :
			MyCrc32Gfunc()
			{
				DefAssertTextWrap(sizeof(m_CRC32[0])==4,"");
				for(int i=0;i<sizeof(m_CRC32)/sizeof(m_CRC32[0]);i++)
				{
					uint32 crc=i;
					for(int j=0;j<8;j++)
					{
						if(crc&1) crc=(crc>>1)^0xEDB88320;
						else crc=crc>>1;
					}
					m_CRC32[i] = crc;
				}
			}
			inline const uint32* getCrc32Tabel() const
			{
				return this->m_CRC32;
			}
			static uint32 build_crc32(const uint32 *_crc_tabel,const unsigned char *_buf,int _len,uint32 _init_crc32)
			{
				while(_len>=16)
				{
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[0])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[1])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[2])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[3])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[4])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[5])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[6])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[7])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[8])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[9])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[10])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[11])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[12])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[13])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[14])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[15])]^(_init_crc32>>8);
					_buf+=16;
					_len-=16;
				}
				while(_len>=4)
				{
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[0])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[1])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[2])]^(_init_crc32>>8);
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^_buf[3])]^(_init_crc32>>8);
					_buf+=4;
					_len-=4;
				}
				while((--_len)>=0)
				{
					_init_crc32=_crc_tabel[((_init_crc32&0xFF)^(*(_buf++)))]^(_init_crc32>>8);
				}
				return _init_crc32;
			}
		};
		static const MyCrc32Gfunc __tem_;
		if(_data!=NULL&&_size>0&&_pThis!=NULL)
		{
			_pThis->val_crc=MyCrc32Gfunc::build_crc32(__tem_.getCrc32Tabel(),_data,_size,_pThis->val_crc);
		}
	}
public :
	TxCrc32CheckCode()
	{
		initialize();
		this->reset();
	}
	static void initialize()
	{
		_g_pushData(NULL,NULL,0);
	}
	inline void reset()
	{
		this->val_crc=(uint32)(-1);
	}
	bool pushFile(const char *_file)
	{
		enum { e_lc_read_unit_size=128*1024, };
		unsigned char *p_buffer=new unsigned char[e_lc_read_unit_size];
		FILE *fp=fopen(_file,"rb");
		if(fp!=NULL)
		{
			for(;;)
			{
				int lc_size=(int)fread(p_buffer,1,e_lc_read_unit_size,fp);
				if(lc_size<=0) break;
				_g_pushData(this,p_buffer,lc_size);
			}
			fclose(fp);
		}
		delete[] p_buffer;
		return fp!=NULL;
	}
	void pushData(const void *_data,int _size)
	{
		_g_pushData(this,(const unsigned char*)_data,_size);
	}
	inline uint32 getUint32Value() const
	{
		return ~this->val_crc;
	}
	inline void getCrc32Buffer(void *_out_crc32)
	{
		uint32 lc_crc=~this->val_crc;
		((unsigned char*)_out_crc32)[0]=(unsigned char)(lc_crc>>24);
		((unsigned char*)_out_crc32)[1]=(unsigned char)(lc_crc>>16);
		((unsigned char*)_out_crc32)[2]=(unsigned char)(lc_crc>>8);
		((unsigned char*)_out_crc32)[3]=(unsigned char)(lc_crc>>0);
	}
	inline void getCrc32Bytes(unsigned char (&_crc)[4])
	{
		return getCrc32Buffer(_crc);
	}
	inline void getCrc32Bytes(char (&_crc)[4])
	{
		return getCrc32Buffer(_crc);
	}
	inline void setFromUint32Value(uint32 _crc32)
	{
		this->val_crc=(~_crc32);
	}
	inline void setFromCrc32Buffer(const void *_in_crc)
	{
		this->setFromUint32Value(
			(((uint32)(((unsigned char*)_in_crc)[0]))<<24)
			|(((uint32)(((unsigned char*)_in_crc)[1]))<<16)
			|(((uint32)(((unsigned char*)_in_crc)[2]))<<8)
			|(((uint32)(((unsigned char*)_in_crc)[3]))<<0));
	}
	inline void setFromCrc32Bytes(const unsigned char (&_crc)[4])
	{
		this->setFromCrc32Buffer(_crc);
	}
	inline void setFromCrc32Bytes(const char (&_crc)[4])
	{
		this->setFromCrc32Buffer(_crc);
	}
};
