#pragma once

#include <list>
#include <string>
#include "../common/TxBlobString.hpp"

class CWindowsWrapExt
{
public:
	static void getLastErrorText(TxBlobString *_retErrText);
	static void getLastErrorText(std::string *_retErrText) ;
};

class CAfxFileFindExt
{
public :
	static bool gFindAllFile(std::list<std::string> *_ret,std::string *_error,const char *_pstr,const char *_strWildcard);
	static bool gDeleteAllFile(std::string *_errText,const char *_filename,int _iDir1File2=0);
};

