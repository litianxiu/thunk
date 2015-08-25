
#ifndef NDEBUG

#ifdef WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <map>
#include <string>
#include <time.h>
#include <assert.h>
#include <memory>
#include <utility>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <stddef.h>
#include <stdlib.h>
#include <memory>

#define _Def_MyAllocatorMemoryDebugInfo_Assert__(_arg) (void)(((!!(_arg))||(__my_assert_(__FILE__,__LINE__),true)))
#define _My_Def_MyAllocatorMemoryDebugInfo_Log_File_Name "TxMemoryPool.log"

class MyAllocatorMemoryDebugInfoBase
{
private:
	long long llAllocatorCount;
protected:
	inline static void __my_assert_(const char *_file,int _line)
	{
		static long bLcJumpOut=1;
		if(bLcJumpOut)
		{
			bLcJumpOut=0;
			printf("assert(%s,%d)\n",_file,_line);
			assert(0);
			bLcJumpOut=1;
		}
	}
	static void _init_revamp_log_file_size_()
	{
		FILE *my_lc_fp=fopen(_My_Def_MyAllocatorMemoryDebugInfo_Log_File_Name,"rb");
		if(my_lc_fp!=NULL)
		{
			unsigned char aLcBuff[2*1024];
			fseek(my_lc_fp,(-1)*(int)sizeof(aLcBuff),SEEK_END); //定位到文件末 
			int aLcBuffLen=fread(aLcBuff,1,sizeof(aLcBuff),my_lc_fp);
			fclose(my_lc_fp);
			::remove(_My_Def_MyAllocatorMemoryDebugInfo_Log_File_Name);
			my_lc_fp=fopen(_My_Def_MyAllocatorMemoryDebugInfo_Log_File_Name,"wb+");
			if(my_lc_fp!=NULL)
			{
				fwrite(aLcBuff,1,aLcBuffLen,my_lc_fp);
				fclose(my_lc_fp);
			}
		}
	}
public :
	MyAllocatorMemoryDebugInfoBase()
	{
		this->llAllocatorCount=0;
		_init_revamp_log_file_size_();
	}
	~MyAllocatorMemoryDebugInfoBase()
	{
		_Def_MyAllocatorMemoryDebugInfo_Assert__(this->llAllocatorCount==0);
	}
	void mem_count_plusplus()
	{
		this->llAllocatorCount++;
	}
	void mem_count_minusminus()
	{
		this->llAllocatorCount--;
	}
	virtual void vf_malloc(void *ptr,long long _llSize)=0;
	virtual void vf_free(void *ptr)=0;
} ;

MyAllocatorMemoryDebugInfoBase *get_global_MyAllocatorMemoryDebugInfo();

template<class T>
class MyMemoryDebugAllocator : public std::allocator<T>
{
public: 
	template<class Other> struct rebind { typedef MyMemoryDebugAllocator<Other> other; };
	typedef	T value_type;
	typedef T* pointer;
	typedef size_t size_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef const T* const_pointer;
	typedef	ptrdiff_t difference_type;
public:
	MyMemoryDebugAllocator() { }
	MyMemoryDebugAllocator(const MyMemoryDebugAllocator& _tem):std::allocator<T>(_tem) {}
	template<class U> MyMemoryDebugAllocator(const MyMemoryDebugAllocator<U>& _tem):std::allocator<T>(_tem) {}
	~MyMemoryDebugAllocator() { }
	MyMemoryDebugAllocator& operator=(const std::allocator<T>& _tem) { std::allocator<T>::operator=(_tem); return (*this); }
	template<class U> MyMemoryDebugAllocator& operator=(const MyMemoryDebugAllocator<U>& _tem) { std::allocator<T>::operator=(_tem); return (*this); }
	pointer allocate( size_type n, std::allocator<void>::const_pointer hint = NULL )
	{
		assert(hint==NULL);
		if(hint!=NULL)
		{
			::free(const_cast<void*>(hint));
			::get_global_MyAllocatorMemoryDebugInfo()->mem_count_minusminus();
		}
		::get_global_MyAllocatorMemoryDebugInfo()->mem_count_plusplus();
		return (pointer)::malloc(n*sizeof(T));
	}
	void deallocate (pointer p, size_type n)
	{
		(void)n;
		::get_global_MyAllocatorMemoryDebugInfo()->mem_count_minusminus();
		::free((void*)p);
	}
};

template<class TYPE_MUTEX,bool BPRINTDEBUF>
class MyAllocatorMemoryDebugInfo: public MyAllocatorMemoryDebugInfoBase
{
	long long __PtrMap_index;
	long long llTotalUserSize;
	long long llTotalUserCount;
	TYPE_MUTEX mMutex;
	struct _StructRecordValueInfo_ { long long llIndex,llSize; };
	std::map<void*,_StructRecordValueInfo_,std::less<void*>,MyMemoryDebugAllocator<std::pair<void const*,_StructRecordValueInfo_>>> __PtrMap;
public:
	MyAllocatorMemoryDebugInfo():__PtrMap_index(0),llTotalUserSize(0),llTotalUserCount(0)//,logFile("TxMemoryPool.log")
	{
	}
	~MyAllocatorMemoryDebugInfo()
	{
		this->mMutex.lock();
		FILE *fp=fopen(_My_Def_MyAllocatorMemoryDebugInfo_Log_File_Name,"a+");
		time_t result_cur_time = ::time(NULL);
		fprintf(fp,"\n\n-------------------------------------%s\n",::ctime(&result_cur_time));
		struct DbgThreeRecordInfo
		{
			long long llSize;
			long long llIndex;
			void *ptr;
			DbgThreeRecordInfo(std::pair<void*,_StructRecordValueInfo_> &_tem)
			{
				this->llSize=_tem.second.llSize;
				this->llIndex=_tem.second.llIndex;
				this->ptr=_tem.first;
			}
		};
		std::vector<DbgThreeRecordInfo,MyMemoryDebugAllocator<DbgThreeRecordInfo>> vLcPtr;
		for(std::map<void*,_StructRecordValueInfo_,std::less<void*>,MyMemoryDebugAllocator<std::pair<void const*,_StructRecordValueInfo_>>>::iterator iter=__PtrMap.begin();
			iter!=__PtrMap.end();iter++)
		{
			vLcPtr.push_back(DbgThreeRecordInfo((std::pair<void*,_StructRecordValueInfo_>&)(*iter)));
		}
		if(BPRINTDEBUF)
			printf("this is MP debug!	");
		fprintf(fp,"this is MP debug!	");
		if(vLcPtr.size())
		{
			if(BPRINTDEBUF)
				printf(" memory pool failure !\nredundancy=%d\n------------------------------>>\n",(int)vLcPtr.size());
			fprintf(fp," memory pool failure !\nredundancy=%d\n------------------------------>>\n",(int)vLcPtr.size());
			//DbgThreeRecordInfo *pd=vLcPtr.data();
			//for(int i=0;i<(int)vLcPtr.size();i++)
			//{
			//	if(BPRINTDEBUF)
			//		printf("%lld	%p	%lld\n",(long long)pd[i].llIndex,(void*)pd[i].ptr,(long long)pd[i].llSize);
			//	fprintf(fp,"%lld	%p	%lld\n",(long long)pd[i].llIndex,(void*)pd[i].ptr,(long long)pd[i].llSize);
			//}
			for(std::vector<DbgThreeRecordInfo,MyMemoryDebugAllocator<DbgThreeRecordInfo>>::iterator iter=vLcPtr.begin();
				iter!=vLcPtr.end();iter++)
			{
				//if(BPRINTDEBUF)
				//	printf("%lld	%p	%lld\n",(long long)pd[i].llIndex,(void*)pd[i].ptr,(long long)pd[i].llSize);
				//fprintf(fp,"%lld	%p	%lld\n",(long long)pd[i].llIndex,(void*)pd[i].ptr,(long long)pd[i].llSize);
				if(BPRINTDEBUF)
					printf("%lld	%p	%lld\n",(long long)iter->llIndex,(void*)iter->ptr,(long long)iter->llSize);
				fprintf(fp,"%lld	%p	%lld\n",(long long)iter->llIndex,(void*)iter->ptr,(long long)iter->llSize);
			}
			if(BPRINTDEBUF)
				printf("++++++++++++++++++++++++++++++>>\n");
			fprintf(fp,"++++++++++++++++++++++++++++++>>\n");
		}
		else
		{
			if(BPRINTDEBUF)
				printf(" memory pool success !");
			fprintf(fp," memory pool success !");
		}
		if(BPRINTDEBUF)
			printf("\n");
		fprintf(fp,"\n");
		fclose(fp);
		_Def_MyAllocatorMemoryDebugInfo_Assert__(llTotalUserSize==0&&llTotalUserCount==0);
		this->mMutex.unlock();
	}
	void vf_malloc(void *ptr,long long _llSize)
	{
		this->mMutex.lock();
		void *ret_ptr=(void*)(ptr);
		_Def_MyAllocatorMemoryDebugInfo_Assert__(__PtrMap.find(ret_ptr)==__PtrMap.end());
		_StructRecordValueInfo_ temValue;
		temValue.llIndex=(++__PtrMap_index);
		temValue.llSize=_llSize;
		llTotalUserSize+=_llSize;
		llTotalUserCount++;
		__PtrMap[ret_ptr]=temValue;
		if(__PtrMap_index==53)
		{
			//printf("%d",__PtrMap_index);
		}
		this->mMutex.unlock();
	}
	void vf_free(void *ptr)
	{
		this->mMutex.lock();
		std::map<void*,_StructRecordValueInfo_,std::less<void*>,MyMemoryDebugAllocator<std::pair<void const*,_StructRecordValueInfo_>>>::iterator __iter=__PtrMap.find(ptr);
		if(__iter!=__PtrMap.end())
		{
			this->llTotalUserCount--;
			this->llTotalUserSize-=__iter->second.llSize;
			if(__iter->second.llIndex==53)
			{
				int ijiji=0; ijiji++;
				//printf("%d",ijiji);
			}
			__PtrMap.erase(__iter);
		}
		else _Def_MyAllocatorMemoryDebugInfo_Assert__(0);
		this->mMutex.unlock();
	}
};

#ifdef WIN32 //windows
#include <Windows.h>
class _My_Tx_CriticalSection_Lock_
{
private:
	CRITICAL_SECTION  m_cs_test;
public:
	_My_Tx_CriticalSection_Lock_() { ::InitializeCriticalSection(&m_cs_test); }
	~_My_Tx_CriticalSection_Lock_() { ::DeleteCriticalSection(&m_cs_test); }
	void lock() { ::EnterCriticalSection(&m_cs_test); }
	void unlock() { ::LeaveCriticalSection(&m_cs_test); }
};
#elif __linux__ //linux
#include <pthread.h>
#include <unistd.h>
class _My_Tx_CriticalSection_Lock_
{
private:
	pthread_mutex_t mutex_x;
public:
	_My_Tx_CriticalSection_Lock_() { this->mutex_x=PTHREAD_MUTEX_INITIALIZER;}
	~_My_Tx_CriticalSection_Lock_() {}
	void lock() { pthread_mutex_lock(&mutex_x); }
	void unlock() { pthread_mutex_unlock(&mutex_x); }
};
#else			//C++11
#include <boost/thread/mutex.hpp>
class _My_Tx_CriticalSection_Lock_
{
private:
	boost::mutex g_mutex;;
public:
	_My_Tx_CriticalSection_Lock_() {}
	~_My_Tx_CriticalSection_Lock_() {}
	void lock() { g_mutex.lock();; }
	void unlock() { g_mutex.unlock(); }
};
#endif

class MyAllocatorMemoryDebugInfo_Syst
{
public:
	MyAllocatorMemoryDebugInfo_Syst()
	{
		get_global_MyAllocatorMemoryDebugInfo();
	}
} m_MyAllocatorMemoryDebugInfo_Syst_sfdjlksdfjkfjksdlfjklsdjklfjklsd;

MyAllocatorMemoryDebugInfoBase *get_global_MyAllocatorMemoryDebugInfo()
{
	static MyAllocatorMemoryDebugInfo<_My_Tx_CriticalSection_Lock_,true> __m_MyAllocatorMemoryDebugInfo_;
	return &__m_MyAllocatorMemoryDebugInfo_;
}

void* initializeDebugMemoryAllocator()
{
	return get_global_MyAllocatorMemoryDebugInfo();
}

void* operator new(std::size_t _size)
{
	if(_size>2*1024*1024)
	{
		int iji=0;
	}
	void *ret=NULL;
	if(_size>0)
	{
		ret=::malloc((int)((_size+7)/8)*8);
		get_global_MyAllocatorMemoryDebugInfo()->vf_malloc(ret,_size);
	}
	return ret;
}
void* operator new[](std::size_t _size)
{
	return ::operator new(_size);
}

void operator delete(void* _rawMemory)
{
	if(_rawMemory!=NULL)
	{
		get_global_MyAllocatorMemoryDebugInfo()->vf_free(_rawMemory);
		::free(_rawMemory);
	}
}
void operator delete[](void* _rawMemory)
{
	return ::operator delete((void*)_rawMemory);
}

#endif

