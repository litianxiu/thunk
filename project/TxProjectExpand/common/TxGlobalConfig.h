#pragma once

#include <assert.h>
#include <stdio.h>

#pragma warning(disable:4996)

#define TxCppPlatform std::tr1

#define DefUseMemoryPool


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

