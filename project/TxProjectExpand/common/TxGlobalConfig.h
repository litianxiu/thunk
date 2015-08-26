#pragma once

#include <assert.h>
#include <stdio.h>

#pragma warning(disable:4996)

#define TxCppPlatform std::tr1

#define DefUseMemoryPool

#if defined(UNICODE)||defined(__________UNICODE)
#define DefBoolCodeEnableUnicode 1
#define DefEncodeText(xxxxx123456789)      L ## xxxxx123456789
#else
#define DefBoolCodeEnableUnicode 0
#define DefEncodeText(xxxxx123456789)      xxxxx123456789
#endif


class TxForbidDirectCopyObject
{
private:
	TxForbidDirectCopyObject(const TxForbidDirectCopyObject&)
	{
		assert(NULL=="copy a object,but it is forbided copyable![a]");
	}
	TxForbidDirectCopyObject& operator=(const TxForbidDirectCopyObject&)
	{
		assert(NULL=="copy a object,but it is forbided copyable![b]"); return *(TxForbidDirectCopyObject*)NULL;
	}
public:
	TxForbidDirectCopyObject() { }
};

