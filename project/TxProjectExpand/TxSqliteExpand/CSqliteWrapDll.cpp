#define _Tx_SqliteWrap_dll_OutPut_
#include "../include/CSqliteWrapDll.h"
#include "../common/TxAssertWrap.hpp"
#include "../common/TxSystemDependent.hpp"
#include "../common/TxBlobString.hpp"

extern "C"
{
#include "./sqlite/sqlite3.h"
}

size_t CSqliteWrapDll_sqlite3_open(const char *_filename)
{
	sqlite3 *ret=NULL;
	while(_filename[0]=='/'||_filename[0]=='\\')
		_filename++;
	TxBlobString str(_filename);
	if(_filename[0]!=0&&_filename[1]!=0
		&&(('a'<=_filename[0]&&_filename[0]<='z')||('A'<=_filename[0]&&_filename[0]<='Z'))
		&&_filename[1]==':'
		&&(_filename[2]=='\\'||_filename[2]=='/'))
	{
	}
	else
	{
		str.assign(TxSystemDependent::TxRuntimeWrap::getWorkingDirectory());
		str.append(_filename);
	}
	str.replaceSelf('\\','/');
	::sqlite3_open(str.c_str(),&ret);
	return (size_t)ret;
}

bool CSqliteWrapDll_sqlite3_exec(size_t _pDb,const char *_sql,TSqliteWrapExecCb _cb,void *_userdata,char **_errmsg)
{
	struct T_R
	{
		TSqliteWrapExecCb cb;
		void *userdata;
		static int g_call_back(void *para,int n_column,char **column_value,char **column_name)
		{
			return ((T_R*)para)->cb(((T_R*)para)->userdata,n_column,column_value,column_name);
		}
	} m_T_R;
	m_T_R.cb=_cb;
	m_T_R.userdata=_userdata;
	bool ret=false;
	if(SQLITE_OK==::sqlite3_exec((sqlite3*)_pDb,_sql,(_cb!=NULL?&T_R::g_call_back:NULL),(_cb!=NULL?&m_T_R:NULL),_errmsg))
		ret=true;
	else
	{
		ret=false;
		if(_errmsg!=NULL&&(*_errmsg)[0]==0)
		{
			*_errmsg="Î´Öª´íÎó";
			DefAssertWrap(FALSE);
		}
	}
	return ret;
}

void CSqliteWrapDll_sqlite3_close(size_t _pDb)
{
	::sqlite3_close((sqlite3*)_pDb);
}
