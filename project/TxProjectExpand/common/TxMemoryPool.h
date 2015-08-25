#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <memory>
#include <vector>
#include <set>
#include <utility>
#include <map>
#include <type_traits>
#include "TxRepeatMacro.h"

//8
//16
//24
//32
//48
//64
//96
//128
//192
//256
//384
//512
//768
//1024
//1536
//2048
//3072
//4096
//6144
//8192
//12288
//16384
//24576
//32768
//49152
//65536
//98304
//131072
//196608
//262144
//393216
//524288
//786432
//1048576
//1572864
//2097152
//3145728
//4194304
//6291456
//8388608
//12582912
//16777216

class TxMemoryPoolBase
{
private:
	enum { EnumHeightSize=41, };
	unsigned long index_size[EnumHeightSize];
	unsigned char *pFree[EnumHeightSize+1];
	void *pHeadList;//sizeof(void*)+sizeof(void*)+data
	int iTxMallocFreeBalance,iSysMallocFreeBalance;
#ifndef NDEBUG
	std::map<void*,int> __PtrMap;
	std::map<void*,int> __FreePtrMap;
	int __PtrMap_index;
#endif
	///////////////const变量是为了防止直接赋值
	const size_t _iCriticalSection;
private:
	virtual void _vfLock_() { };
	virtual void _vfUnlock_() { };
public :
	TxMemoryPoolBase():_iCriticalSection(0)
	{
#ifndef NDEBUG
		this->__PtrMap_index=0;
#endif
		pHeadList=NULL;
		this->iTxMallocFreeBalance=this->iSysMallocFreeBalance=0;
		memset(pFree,0,sizeof(pFree));
		index_size[0]=8; index_size[1]=16; index_size[2]=24;
		for(int i=3;i<EnumHeightSize;i++)
			index_size[i]=(index_size[i-2]<<1);
	}
	~TxMemoryPoolBase()
	{
#ifndef NDEBUG
		this->_vfLock_();
		std::vector<std::pair<void*,int>> vPtr;
		for(auto iter=__PtrMap.begin();iter!=__PtrMap.end();iter++)
			vPtr.push_back(*iter);
		printf("this is MP debug!	");
		if(vPtr.size())
		{
			printf(" memory pool failure ！\nredundancy=%d\n------------------------------>>\n",(int)vPtr.size());
			std::pair<void*,int> *pd=vPtr.data();
			for(int i=0;i<(int)vPtr.size();i++)
				printf("%d	%p\n",(void*)pd[i].second,(void*)pd[i].first);
			printf("++++++++++++++++++++++++++++++>>\n");
		}
		else printf(" memory pool success ！");
		printf("\n");
		this->_vfUnlock_();
#endif
		while(pHeadList!=NULL)
		{
			void *p=*(void**)pHeadList;
			::free(pHeadList);
			this->iSysMallocFreeBalance--;
			pHeadList=p;
		}
		assert(this->iTxMallocFreeBalance==0&&this->iSysMallocFreeBalance==0);
	}
	void* txMalloc(unsigned long _size,long _userData=0)
	{
		int l=0,pos=EnumHeightSize;
		while(l<pos)
		{
			int mid=(l+pos)>>1;
			if(this->index_size[mid]<_size) l=mid+1;
			else pos=mid;
		}
		this->_vfLock_();
		if(this->pFree[pos]==nullptr)
		{
			const int chunkMinBaseSize=(128*1024<=this->index_size[EnumHeightSize-1]?128*1024:this->index_size[EnumHeightSize-1]);
			int chunkSize=(pos==EnumHeightSize?(_size+7)/8*8:this->index_size[pos])+sizeof(long)+sizeof(long);
			int totalSize=((chunkSize+chunkMinBaseSize)/chunkSize)*chunkSize;
			unsigned char *newHead=(unsigned char*)::malloc(totalSize+sizeof(void*)+sizeof(void*));
			((void**)newHead)[0]=this->pHeadList;
			((void**)newHead)[1]=nullptr;
			if(this->pHeadList!=nullptr)
				((void**)this->pHeadList)[1]=newHead;
			this->pHeadList=newHead;
			newHead+=sizeof(void*)+sizeof(void*)+totalSize-chunkSize;
			pFree[pos]=newHead;
			for(int count=totalSize/chunkSize;--count;newHead-=chunkSize)
				*(void**)newHead=newHead-chunkSize;
			*(void**)newHead=nullptr;
			this->iSysMallocFreeBalance++;
		}
		unsigned char *res=pFree[pos];
		pFree[pos]=*(unsigned char**)pFree[pos];
		((long*)(long*)res)[0]=pos;
		((long*)(long*)res)[1]=_userData;
		this->iTxMallocFreeBalance++;
		this->_vfUnlock_();
#ifndef NDEBUG
		this->_vfLock_();
		void *ret_ptr=(void*)(res+sizeof(long)+sizeof(long));
		assert(__PtrMap.find(ret_ptr)==__PtrMap.end());
		__PtrMap.insert(std::pair<void*,int>(ret_ptr,++__PtrMap_index));
		if(__PtrMap_index==161||__PtrMap_index==148)
		{
			int jkj=0;jkj++;
		}
		this->_vfUnlock_();
#endif
		return (void*)(res+sizeof(long)+sizeof(long));
	}
	void txFree(const void *__ptr,long *_userData=nullptr)
	{
		void *ptr=const_cast<void*>(__ptr);
#ifndef NDEBUG
		this->_vfLock_();
		auto __iter=__PtrMap.find(ptr);
		if(__iter->second==18)
		{
			int ijiji=0; ijiji++;
		}
		assert(__iter!=__PtrMap.end());
		__FreePtrMap.insert(std::pair<void*,int>(ptr,__iter->second));
		__PtrMap.erase(__iter);
		this->_vfUnlock_();
#endif
		ptr=((long*)ptr)-2;
		long pos=((long*)ptr)[0];
		if(_userData!=nullptr)
			*_userData=((long*)ptr)[1];
		this->_vfLock_();
		if((unsigned long)pos<EnumHeightSize)
		{
			*(void**)ptr=this->pFree[pos];
			this->pFree[pos]=(unsigned char*)ptr;
		}
		else
		{
			ptr=((void**)ptr)-2;
			void *newxPtr=((void**)ptr)[0];
			if(newxPtr!=nullptr) ((void**)newxPtr)[1]=((void**)ptr)[1];
			void *prcvPtr=((void**)ptr)[1];
			if(prcvPtr!=nullptr) ((void**)prcvPtr)[0]=((void**)ptr)[0];
			else this->pHeadList=newxPtr;
			::free(ptr);
			this->iSysMallocFreeBalance--;
		}
		this->iTxMallocFreeBalance--;
		this->_vfUnlock_();
	}
public:
	template<class T>
	void txDestructor(T *ptr)
	{
		int iArryNum=*(((long*)ptr)-1);
		if(!std::is_trivial<T>::value&&!TxTmpltType::TxIsBasicType<T>::value)
		{
			for(int i=iArryNum;i--;) ptr[i].~T();
		}
		long lArryNum=0;
		this->txFree(ptr,&lArryNum);
		assert(iArryNum==(int)lArryNum);
	}

#define Def_TxMemoryPool_txConstructor_func_Null() \
	template<class T> \
	T* txConstructor(int iArryNum) \
	{ \
	T *ret=(T*)txMalloc(iArryNum*sizeof(T),(int)iArryNum); \
	if(!std::is_trivial<T>::value&&!TxTmpltType::TxIsBasicType<T>::value) \
	{ \
	while(iArryNum--) \
	new(&ret[iArryNum]) T(); \
	} \
	return ret; \
	}
#define Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ0_A,DefTxRepeatMacroAZaz0_AZaz0_Aa,DefTxRepeatMacroaz0_az0_a) \
	template<class T,DefTxRepeatMacroclassAZ0_classAZ0_A> \
	T* txConstructor(int iArryNum,DefTxRepeatMacroAZaz0_AZaz0_Aa) \
	{ \
	T *ret=(T*)txMalloc(iArryNum*sizeof(T),(int)iArryNum); \
	while(iArryNum--) \
	new(&ret[iArryNum]) T(DefTxRepeatMacroaz0_az0_a); \
	return ret; \
	}

	Def_TxMemoryPool_txConstructor_func_Null()
		Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ1(A),DefTxRepeatMacroAZaz0_AZaz1(A,a),DefTxRepeatMacroaz0_az1(a))
		Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ2(A),DefTxRepeatMacroAZaz0_AZaz2(A,a),DefTxRepeatMacroaz0_az2(a))
		Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ3(A),DefTxRepeatMacroAZaz0_AZaz3(A,a),DefTxRepeatMacroaz0_az3(a))
		Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ4(A),DefTxRepeatMacroAZaz0_AZaz4(A,a),DefTxRepeatMacroaz0_az4(a))
		Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ5(A),DefTxRepeatMacroAZaz0_AZaz5(A,a),DefTxRepeatMacroaz0_az5(a))
		Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ6(A),DefTxRepeatMacroAZaz0_AZaz6(A,a),DefTxRepeatMacroaz0_az6(a))
		Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ7(A),DefTxRepeatMacroAZaz0_AZaz7(A,a),DefTxRepeatMacroaz0_az7(a))
		Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ8(A),DefTxRepeatMacroAZaz0_AZaz8(A,a),DefTxRepeatMacroaz0_az8(a))
		Def_TxMemoryPool_txConstructor_func(DefTxRepeatMacroclassAZ0_classAZ9(A),DefTxRepeatMacroAZaz0_AZaz9(A,a),DefTxRepeatMacroaz0_az9(a))
};

struct _TxMemoryPoolFakeVainMutexType_
{
	inline void lock() { }
	inline void unlock() { }
};

template<typename MUTEX_TYPE=_TxMemoryPoolFakeVainMutexType_>
class TxMemoryPool : public TxMemoryPoolBase
{
private:
	MUTEX_TYPE mMutex;
private:
	virtual void _vfLock_() { mMutex.lock(); };
	virtual void _vfUnlock_() { mMutex.unlock(); };
};

//template<bool _bCriticalSection>
//class TxMemoryPool_B :public TxMemoryPool
//{
//public:
//	TxMemoryPool_B():TxMemoryPool(_bCriticalSection) { }
//};

//class TestBase
//{
//	int in[5];
//public:
//	TestBase()
//	{
//		int i=0;
//	}
//	virtual ~TestBase()
//	{
//		int i=0;
//	}
//};
//class TestBasefd
//{
//public:
//	TestBasefd()
//	{
//		int i=0;
//	}
//	virtual ~TestBasefd()
//	{
//		int i=0;
//	}
//};
//class test :public TestBase,public TestBasefd
//{
//	int i;
//public:
//	test(int a1,int a2,int a3,int a4,int a5)
//	{
//		i=a1;
//	}
//	virtual ~test()
//	{
//		int i=0;
//	}
//};
//
//int ijij0=0;
//int ijij1=0;
//class test9 :public TestBase,public TestBasefd
//{
//	int i;
//public:
//	test9(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,int a9)
//	{
//		i=a1;
//		ijij0++;
//	}
//	virtual ~test9()
//	{
//		int i=0;
//		ijij1++;
//	}
//};
//
//int main()
//{
//	if(0){
//		TxMemoryPool gg;
//		test *p=gg.txConstructor<test>(1,1,2,3,4,5);
//		TestBase *pb=p;
//		gg.txDestructor(pb);
//
//		test9 *pp9=gg.txConstructor<test9>(1010000,1,2,3,4,5,6,7,8,9);
//		gg.txDestructor(pp9);
//
//
//		test9 *pp12=gg.txConstructor<test9>(101000,1,2,3,4,5,6,7,8,9);
//
//		test9 *pfp12=gg.txConstructor<test9>(1331000,1,2,3,4,5,6,7,8,9);
//		gg.txDestructor(pfp12);
//
//		gg.txDestructor(pp12);
//
//		unsigned char *psfd=gg.txConstructor<unsigned char>(1);
//		gg.txDestructor(psfd);
//	}
//	{
//		TxMemoryPool gg;
//		test9 *pp9=gg.txConstructor<test9>(1010000,1,2,3,4,5,6,7,8,9);
//		gg.txDestructor(pp9);
//
//		test9 *pp12=gg.txConstructor<test9>(1010000,1,2,3,4,5,6,7,8,9);
//		gg.txDestructor(pp12);
//
//		struct OO
//		{
//			char bb[1010000*sizeof(test9)];
//		};
//		OO *psfd=gg.txConstructor<OO>(1);
//		gg.txDestructor(psfd);
//		int jkljkl=0;
//	}
// 	if(0)
// 	{
// 		const int iMax=10*1024*1024;
// 		for (int io=0;io<iMax;io++)
// 			new char[1];
// 		for (int io=0;io<iMax;io++)
// 			new int[1];
// 		for (int io=0;io<iMax;io++)
// 			gg.txConstructor<unsigned char>(1);
// 		for (int io=0;io<iMax;io++)
// 			gg.txConstructor<unsigned int>(1);
// 	}
//	return 0;
//}


//const int COUNT=1024;
//unsigned char *a[COUNT];
//int main()
//{
//	clock_t t1=clock();
//	{
//		TxMemoryPool PPP;
//		for(int kk=0;kk<1000;kk++)
//		{
//			for(int i=0;i<COUNT;i++)
//			{
//				a[i]=PPP.txConstructor<unsigned char>(i*8+1);
//				//a[i]=new unsigned char[i*8+1];
//			}
//			for(int i=0;i<COUNT;i++)
//			{
//				PPP.txDestructor(a[i]);
//				//delete a[i];
//			}
//		}
//		clock_t t2=clock();
//		double t3=((double)(t2-t1))/1000.0;
//		//t3=0;
//		printf("%g",t3);
//		gets((char*)a);
//	}
//	return 0;
//}
