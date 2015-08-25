#pragma once

#include <Windows.h>
#include "CWindowsWrapExt.h"

class CWindowsFileHandle
{
public :
	static void g_build_crc32(unsigned char (&_crc32)[4],long long *_filesize,const char *_filename)
	{
		_crc32[0]=0;
		_crc32[1]=0;
		_crc32[2]=0;
		_crc32[3]=0;
		*_filesize=0;
		(void)_filename;
		return ;
	}
	static void g_read_file(void *_buff,int *_readsize,std::string *_err,long long *_llTotalSize,const char *_filename,long long _llpos,int _buffsize)
	{
		HANDLE pfile = ::CreateFileA(
			_filename,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);

		if(pfile==INVALID_HANDLE_VALUE)
		{
			_err->assign("无法打开文件");
		}
		else
		{
			LARGE_INTEGER lcFileSize={0};
			if(!::GetFileSizeEx(pfile,&lcFileSize))
			{
				_err->assign("无法获取文件大小");
			}
			else
			{
				*_llTotalSize=lcFileSize.QuadPart;
				DWORD dwCount=0;
				OVERLAPPED lv_ovlpd={0};
				lv_ovlpd.Offset=(DWORD)_llpos;
				lv_ovlpd.OffsetHigh=(DWORD)(((unsigned long long)_llpos)>>(8*sizeof(DWORD)));
				if(!::ReadFile(pfile,_buff,(DWORD)_buffsize,&dwCount,&lv_ovlpd)||dwCount<0)
				{
					CWindowsWrapExt::getLastErrorText(_err);
					if(_err->length()<=0) _err->assign("错误");
				}
				else
				{
					*_readsize=(int)dwCount;
				}
			}
			::CloseHandle(pfile);
		}
	}
	static void g_write_file(std::string *_err,const char *_filename,long long _llpos,const void *_buff,int _buffsize)
	{
		HANDLE pfile = ::CreateFileA(
			_filename,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if(pfile!=INVALID_HANDLE_VALUE)
		{
			_err->assign("无法打开文件");
		}
		else
		{
			DWORD dwCount=0;
			OVERLAPPED lv_ovlpd={0};
			lv_ovlpd.Offset=(DWORD)_llpos;
			lv_ovlpd.OffsetHigh=(DWORD)(((unsigned long long)_llpos)>>(8*sizeof(DWORD)));
			if(!::WriteFile(pfile,_buff,(DWORD)_buffsize,&dwCount,&lv_ovlpd)||dwCount<0
				||dwCount!=(DWORD)_buffsize)
			{
				CWindowsWrapExt::getLastErrorText(_err);
				if(_err->length()<=0) _err->assign("错误");
			}
			else
			{
			}
			::CloseHandle(pfile);
		}
	}
};
