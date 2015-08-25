#pragma once

#include <stdlib.h>
#include <assert.h>

#ifndef _Tx_SqliteWrap_dll_OutPut_
#pragma comment(lib,"../bin/TxSqliteWrap.lib")
#define TX_DLL_TYPE extern "C" __declspec(dllimport)
#else
#define TX_DLL_TYPE extern "C" __declspec(dllexport)
#endif


typedef int (__stdcall *TSqliteWrapExecCb)(void *para,int n_column,char **column_value,char **column_name);


TX_DLL_TYPE size_t CSqliteWrapDll_sqlite3_open(const char *_filename);
TX_DLL_TYPE bool CSqliteWrapDll_sqlite3_exec(size_t _pDb,const char *_sql,TSqliteWrapExecCb _cb,void *_userdata,char **_errmsg);
TX_DLL_TYPE void CSqliteWrapDll_sqlite3_close(size_t _pDb);

#undef TX_DLL_TYPE

class CSqliteWrapDll
{
private:
	size_t pDb;
public :
	CSqliteWrapDll()
	{
		this->pDb=NULL;
	}
	~CSqliteWrapDll()
	{
		this->close();
	}
	void close()
	{
		if(this->pDb!=NULL)
		{
			::CSqliteWrapDll_sqlite3_close(this->pDb);
			this->pDb=NULL;
		}
	}
	bool open(const char *_filename)
	{
		this->close();
		this->pDb=::CSqliteWrapDll_sqlite3_open(_filename);
		return this->pDb!=NULL;
	}
	bool execute(const char *_sql,TSqliteWrapExecCb _cb,void *_userdata,char **_errmsg)
	{
		if(this->pDb!=NULL)
			return ::CSqliteWrapDll_sqlite3_exec(this->pDb,_sql,_cb,_userdata,_errmsg);
		assert(0);
		if(_errmsg!=NULL)
			*_errmsg="数据库没打开";
		return false;
	}
	inline bool isValid() const
	{
		return this->pDb!=NULL;
	}
};

