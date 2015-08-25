#pragma once

#include <atlstr.h>
#include <comdef.h>
#include <mswsock.h>
#include <process.h>
#include <Windows.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <memory>
#include <vector>
#include "TxGlobalConfig.h"
#include "TxAssertWrap.hpp"
#include "TxSystemDependent.hpp"

#pragma comment(lib,"ws2_32.lib")


#define DefIocpTcpApiGetObjectOrgOverLappedPointer_(OrgClass,MemOverLapped,OverLappedPtr) \
	((OrgClass*)(((size_t)(OVERLAPPED*)OverLappedPtr)-((size_t)(OVERLAPPED*)&((OrgClass*)(NULL))->MemOverLapped)))

namespace TxIocpTcpApi {

#pragma pack(push)//保存对齐状态
#pragma pack(1)//设定为4字节对齐
	struct tagIocpTcpApiAccpetExCbAddrBuff_
	{
		struct {
			sockaddr_in mEndPointAddr;
			unsigned char __buff__[16];
		} mLocalEndPoint, mRemoteEndPoint;
	};
#pragma pack(pop)//恢复对齐状态

	class _My_IocpGlobalFunction_
	{
	public:
		template<class T,class TTT>
		inline static T buildDynamicCast(TTT *_ptr)
		{
			DefAssertTextWrap(_ptr==NULL||(dynamic_cast<T>(_ptr))!=NULL,"");
			return _ptr!=NULL?(T)_ptr:(T)NULL;
		}
	};

	class TxIocpMemoryManager
	{
	public:
		virtual void* malloc(size_t _size)=0;
		virtual void free(void *_ptr)=0;
	};

	class MyIocpMemoryManagerR_
	{
	private:
		TxIocpMemoryManager *const pIocpMemoryManager;
	public:
		MyIocpMemoryManagerR_(TxIocpMemoryManager *_pIocpMemoryManager)
			:pIocpMemoryManager(_pIocpMemoryManager)
		{
		}
		void* malloc(size_t _size)
		{
			if(this->pIocpMemoryManager!=NULL)
				return this->pIocpMemoryManager->malloc(_size);
			return new unsigned char[_size];
		}
		void free(const void *_ptr)
		{
			if(this->pIocpMemoryManager!=NULL)
				this->pIocpMemoryManager->free(const_cast<void*>(_ptr));
			else delete[] (unsigned char*)_ptr;
		}
		inline TxIocpMemoryManager* getIocpMemoryManager()
		{
			return this->pIocpMemoryManager;
		}
	};

	class MyRecordThreadIdMgr_: private TxForbidDirectCopyObject
	{
	private:
		struct StructArry
		{
			DWORD dwThreadId;
			DWORD dwThreadIndex;
			inline bool operator<(const StructArry &_tem) { return this->dwThreadId<_tem.dwThreadId; }
		} *arry;
		int count;
		DWORD dwThreadStartIndex;
		MyIocpMemoryManagerR_ mIocpMemoryManagerR;
	public :
		MyRecordThreadIdMgr_(TxIocpMemoryManager *_pIocpMemoryManager)
			:mIocpMemoryManagerR(_pIocpMemoryManager)
		{
			this->dwThreadStartIndex=0;
			this->arry=NULL;
			this->count=0;
		}
		~MyRecordThreadIdMgr_()
		{
			this->reset(0);
		}
		void reset(int _count)
		{
			if(arry!=NULL)
			{
				if(this->arry!=NULL)
					this->mIocpMemoryManagerR.free(this->arry);
			}
			this->dwThreadStartIndex=0;
			this->arry=NULL;
			this->count=0;
			if(_count>0)
			{
				this->count=_count;
				this->arry=(StructArry*)this->mIocpMemoryManagerR.malloc(this->count*sizeof(this->arry[0]));
			}
		}
		void pushThisThread(DWORD _threadId)
		{
			this->arry[this->dwThreadStartIndex].dwThreadId=_threadId;
			this->arry[this->dwThreadStartIndex].dwThreadIndex=this->dwThreadStartIndex;
			this->dwThreadStartIndex++;
			DefAssertTextWrap((int)this->dwThreadStartIndex<=(int)this->count,"add thread is to many");
		}
		void settle()
		{
			DefAssertTextWrap(this->dwThreadStartIndex==this->count,"thread count is wrong");
			std::sort(&this->arry[0],&this->arry[this->count]);
#ifndef NDEBUG
			for(int i=this->count-1;(--i)>=0;)
			{
				DefAssertTextWrap((this->arry[i].dwThreadId!=this->arry[i+1].dwThreadId),"thread id repeat");
			}
#endif
		}
		inline int getCurrentThreadIndex(DWORD _threadId) const
		{
			StructArry *pArry=this->arry;
			if(pArry[0].dwThreadId<=_threadId&&this->count>0&&_threadId<=pArry[this->count].dwThreadId)
			{
				int l=0,r=this->count;
				while(l!=r)
				{
					int k=(l+r)>>1;
					if(pArry[k].dwThreadId<_threadId) l=k+1;
					else r=k;
				}
				if(pArry[l].dwThreadId==_threadId)
					return pArry[l].dwThreadIndex;
			}
			DefAssertTextWrap(FALSE,"getCurrentThreadIndex() no this thread id");
#ifndef NDEBUG
			{
				DWORD arry_id[1024]={0};
				int i=sizeof(arry_id)/sizeof(arry_id[0]);
				if(i>count) i=count;
				while(i--) arry_id[i]=this->arry[i].dwThreadId;
				int dfs=0;
			}
#endif
			return -1;
		}
	};

	struct MyIocpServiceAtmcVariate_
	{
		LONG *lAtmOverLappedAccretionCount;
		LONG *lAtmExistThreadCount;
		LONG *lAtmTriggerStopCount;
		LONG *lAtmThreadRunning;
		MyIocpServiceAtmcVariate_()
		{
			enum { eAlignment=16, };
			*(this->lAtmOverLappedAccretionCount=(LONG*)::_aligned_malloc(sizeof(this->lAtmOverLappedAccretionCount[0]),eAlignment))=0;
			*(this->lAtmExistThreadCount=(LONG*)::_aligned_malloc(sizeof(this->lAtmExistThreadCount[0]),eAlignment))=0;
			*(this->lAtmTriggerStopCount=(LONG*)::_aligned_malloc(sizeof(this->lAtmTriggerStopCount[0]),eAlignment))=0;
			*(this->lAtmThreadRunning=(LONG*)::_aligned_malloc(sizeof(this->lAtmThreadRunning[0]),eAlignment))=0;
		}
		~MyIocpServiceAtmcVariate_()
		{
			::_aligned_free(this->lAtmOverLappedAccretionCount);
			::_aligned_free(this->lAtmExistThreadCount);
			::_aligned_free(this->lAtmTriggerStopCount);
			::_aligned_free(this->lAtmThreadRunning);
		}
	};

	class TxIocpService: private TxForbidDirectCopyObject
	{
	public :
		typedef bool (*FuncType)(OVERLAPPED *_obj,int _iNumberOfBytes,bool _bResultStatus);
		struct tagIocpOverlappedWorkBase_
		{
			OVERLAPPED mOVERLAPPED;
			FuncType func;
			void initialize(FuncType _func) { ::memset(&this->mOVERLAPPED,0,sizeof(this->mOVERLAPPED)); this->func=_func; }
			void reset_overlapped() { ::memset(&this->mOVERLAPPED,0,sizeof(this->mOVERLAPPED)); }
			void pushQueueFailureWork() { this->func(&this->mOVERLAPPED,0,0); }
		};
	private:
		TxSystemDependent::TxMutexWrap mGlobalMutex;
		TxSystemDependent::TxMutexWrap mRecordThreadIdMgrMutex;
		MyRecordThreadIdMgr_ mRecordThreadIdMgr;
		MyIocpMemoryManagerR_ mIocpMemoryManagerR;
		HANDLE hIoCompletionPort;
		std::vector<TxCppPlatform::shared_ptr<TxSystemDependent::TxThreadWrap>> mListThread;
		void *__tem_1_;
		LONG bThreadRunSustainStatus;
		void *__tem_2_;
	public :
		MyIocpServiceAtmcVariate_ mMyIocpServiceAtmcVariate;
	public:
		void _associationSocket_(SOCKET _mSocket)
		{
			if (NULL == ::CreateIoCompletionPort((HANDLE)_mSocket, this->hIoCompletionPort, (u_long)0, 0))
				DefAssertTextWrap(FALSE,"CreateIoCompletionPort[iocp-SOCKET] failure!");
		}
		void _associationSocket_(HANDLE _hSerialPort)
		{
			if (NULL == ::CreateIoCompletionPort(_hSerialPort, this->hIoCompletionPort, (u_long)0, 0))
				DefAssertTextWrap(FALSE,"CreateIoCompletionPort[iocp-HANDLE] failure!");
		}
		template<class T>
		T* _malloc_()
		{
			void *ptr=this->mIocpMemoryManagerR.malloc(sizeof(T));
			new ((T*)ptr) T();
			return (T*)ptr;
		}
		template<class T,class A1>
		T* _malloc_(A1 _a1)
		{
			void *ptr=this->mIocpMemoryManagerR.malloc(sizeof(T));
			new ((T*)ptr) T(_a1);
			return (T*)ptr;
		}
		template<class T,class A1,class A2>
		T* _malloc_(A1 _a1,A2 _a2)
		{
			void *ptr=this->mIocpMemoryManagerR.malloc(sizeof(T));
			new ((T*)ptr) T(_a1,_a2);
			return (T*)ptr;
		}
		template<class T,class A1,class A2,class A3>
		T* _malloc_(A1 _a1,A2 _a2,A3 _a3)
		{
			void *ptr=this->mIocpMemoryManagerR.malloc(sizeof(T));
			new ((T*)ptr) T(_a1,_a2,_a3);
			return (T*)ptr;
		}
		template<class T>
		void _free_(T *_ptr)
		{
			_ptr->~T();
			return this->mIocpMemoryManagerR.free(_ptr);
		}
		static void _static_run_(void *_arg1,void *_arg2)
		{
			(void)_arg1;
			(void)_arg2;
			TxIocpService* pThis=((TxIocpService*)_arg1);
			pThis->mRecordThreadIdMgrMutex.lock();
			pThis->mRecordThreadIdMgrMutex.unlock();
			pThis->_run_();
		}
		void _run_()
		{
			this->onStartThreadEvent();
			::InterlockedIncrement(this->mMyIocpServiceAtmcVariate.lAtmTriggerStopCount);
			::InterlockedIncrement(this->mMyIocpServiceAtmcVariate.lAtmExistThreadCount);
			while(this->bThreadRunSustainStatus
				||::InterlockedExchangeAdd(this->mMyIocpServiceAtmcVariate.lAtmOverLappedAccretionCount,0)!=0
				||::InterlockedExchangeAdd(this->mMyIocpServiceAtmcVariate.lAtmTriggerStopCount,0)+1
				!=::InterlockedExchangeAdd(this->mMyIocpServiceAtmcVariate.lAtmExistThreadCount,0))
			{
				ULONG_PTR ulNumberOfBytes = 0;
				OVERLAPPED *pLcOverlapped = NULL;
				ULONG_PTR ulLcCompletionKey = 0;
				BOOL bResultStatus = ::GetQueuedCompletionStatus(this->hIoCompletionPort,&ulNumberOfBytes, &ulLcCompletionKey, &pLcOverlapped,INFINITE);
				DefAssertTextWrap(pLcOverlapped!=NULL&&DefIocpTcpApiGetObjectOrgOverLappedPointer_(tagIocpOverlappedWorkBase_,mOVERLAPPED,pLcOverlapped)->func!=NULL,
					"GetQueuedCompletionStatus() failure!");
				if (pLcOverlapped != NULL)
				{
					if(!DefIocpTcpApiGetObjectOrgOverLappedPointer_(tagIocpOverlappedWorkBase_,mOVERLAPPED,pLcOverlapped)->func(
						pLcOverlapped,(int)ulNumberOfBytes,!!bResultStatus))
					{
						::InterlockedDecrement(this->mMyIocpServiceAtmcVariate.lAtmTriggerStopCount);
					}
				}
			}
			if(::InterlockedDecrement(this->mMyIocpServiceAtmcVariate.lAtmExistThreadCount)>0)
				this->_triggerStop__();
			this->onFinishThreadEvent();
		}
		void _triggerStop__()
		{
			struct tagIocpOverlappedWork_Ext : public tagIocpOverlappedWorkBase_
			{
				TxIocpService *const pTxIocpService;
				tagIocpOverlappedWork_Ext(TxIocpService *_pTxIocpService):pTxIocpService(_pTxIocpService)
				{
					::InterlockedIncrement(this->pTxIocpService->mMyIocpServiceAtmcVariate.lAtmOverLappedAccretionCount);
				}
				~tagIocpOverlappedWork_Ext()
				{
					::InterlockedDecrement(this->pTxIocpService->mMyIocpServiceAtmcVariate.lAtmOverLappedAccretionCount);
				}
				static bool g_func(OVERLAPPED *_obj,int _iNumberOfBytes,bool _bResultStatus)
				{
					tagIocpOverlappedWork_Ext *pIocpOverlappedWork_Ext=DefIocpTcpApiGetObjectOrgOverLappedPointer_(tagIocpOverlappedWork_Ext,mOVERLAPPED,_obj);
					pIocpOverlappedWork_Ext->pTxIocpService->_free_(pIocpOverlappedWork_Ext);
					return false;
				}
			};
			tagIocpOverlappedWork_Ext *pTxIocpOverlappedWorkBase_Ext = this->_malloc_<tagIocpOverlappedWork_Ext>(this);
			pTxIocpOverlappedWorkBase_Ext->initialize(&tagIocpOverlappedWork_Ext::g_func);
			if(this->hIoCompletionPort==NULL
				||!::PostQueuedCompletionStatus(this->hIoCompletionPort,NULL,NULL,&pTxIocpOverlappedWorkBase_Ext->mOVERLAPPED))
			{
				pTxIocpOverlappedWorkBase_Ext->pushQueueFailureWork();
				DefAssertTextWrap(FALSE,"triggerStop()::PostQueuedCompletionStatus() failure");
			}
		}
		inline TxIocpMemoryManager* _getIocpMemoryManager__()
		{
			return this->mIocpMemoryManagerR.getIocpMemoryManager();
		}
	private:
		virtual void onStartThreadEvent() { }
		virtual void onFinishThreadEvent() { }
	public:
		TxIocpService(TxIocpMemoryManager *_pIocpMemoryManager=NULL)
			: hIoCompletionPort(NULL),mIocpMemoryManagerR(_pIocpMemoryManager),mRecordThreadIdMgr(_pIocpMemoryManager)
		{
			WSADATA wsaData = { 0 };
			WORD wVersionRequested = MAKEWORD(2, 2);
			if (WSAStartup(wVersionRequested, &wsaData) != 0)
				DefAssertTextWrap(FALSE,"WSAStartup failed with error");
			DefAssertTextWrap(LOBYTE(wsaData.wVersion) == 2 && HIBYTE(wsaData.wVersion) == 2,"Could not find a usable version of Winsock.dll");
			this->hIoCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
			DefAssertTextWrap(this->hIoCompletionPort!=NULL,"CreateIoCompletionPort() create iocp object failure");
		}
		virtual ~TxIocpService()
		{
			this->join();
			if (hIoCompletionPort != NULL)
			{
				::CloseHandle(hIoCompletionPort);
				hIoCompletionPort = NULL;
			}
			WSACleanup();
			DefAssertTextWrap(::InterlockedExchangeAdd(this->mMyIocpServiceAtmcVariate.lAtmOverLappedAccretionCount,0)==0,"when quit service, some OverLapped no clear!");
		}
		void start(int _iThreadCount)
		{
			this->join();
			this->mGlobalMutex.lock();
			this->mListThread.clear();
			this->mListThread.resize(_iThreadCount);
			::InterlockedExchange(this->mMyIocpServiceAtmcVariate.lAtmThreadRunning,1);
			this->bThreadRunSustainStatus=1;
			this->mRecordThreadIdMgrMutex.lock();
			this->mRecordThreadIdMgr.reset(_iThreadCount);
			for(int i=0;i<(int)this->mListThread.size();i++)
			{
				this->mListThread[i].reset(new TxSystemDependent::TxThreadWrap());
				this->mListThread[i]->create(_static_run_,(void*)this,NULL);
				this->mRecordThreadIdMgr.pushThisThread(this->mListThread[i]->getThreadId());
			}
			this->mRecordThreadIdMgr.settle();
			this->mRecordThreadIdMgrMutex.unlock();
			this->mGlobalMutex.unlock();
		}
		void join()
		{
			this->mGlobalMutex.lock();
			for(int i=0;i<(int)this->mListThread.size();i++)
			{
				this->mListThread[i]->join();
				this->mListThread[i]->release();
			}
			this->mListThread.clear();
			this->mRecordThreadIdMgrMutex.lock();
			this->mRecordThreadIdMgr.reset(0);
			this->mRecordThreadIdMgrMutex.unlock();
			this->mGlobalMutex.unlock();
		}
		void triggerStop()
		{
			if(this->bThreadRunSustainStatus)
				this->bThreadRunSustainStatus=0;
			if(::InterlockedExchange(this->mMyIocpServiceAtmcVariate.lAtmThreadRunning,0)!=0)
				this->_triggerStop__();
		}
		int getCurrentThreadIndex() const
		{
			return this->mRecordThreadIdMgr.getCurrentThreadIndex(::GetCurrentThreadId());
		}
	};

	template<int BUFFERSIZE>
	class MyIocpMemoryChunk_: private TxForbidDirectCopyObject
	{
	protected:
		struct StructDataInfo
		{
			unsigned char *pBuffer;
			struct StructDataInfo *next;
			int iStartPos;
			int iEndPos;
			int iMaxSize;
		};
		StructDataInfo *head;
		StructDataInfo *tail;
		unsigned char __tem_buffer__[BUFFERSIZE];
		BOOL b__tem_buffer_valid_;
		MyIocpMemoryManagerR_ mIocpMemoryManagerR;
		int iTotalSize;
		const int iUnitMaxSize;
	private:
		void* _malloc_(int *_outSize,size_t _size)
		{
			void *ret;
			if(this->b__tem_buffer_valid_&&_size<sizeof(this->__tem_buffer__))
			{
				this->b__tem_buffer_valid_=FALSE;
				ret=__tem_buffer__;
				*_outSize=(int)sizeof(this->__tem_buffer__);
			}
			else
			{
				*_outSize=(size_t)((_size+sizeof(long long)-1)/sizeof(long long))*sizeof(long long);
				ret=this->mIocpMemoryManagerR.malloc(*_outSize);
			}
			return ret;
		}
		void _free_(void *_ptr)
		{
			if(_ptr==(void*)this->__tem_buffer__) this->b__tem_buffer_valid_=TRUE;
			else this->mIocpMemoryManagerR.free(_ptr);
		}
	public :
		MyIocpMemoryChunk_(TxIocpMemoryManager *_pIocpMemoryManager,int _iUnitMaxSize)
			:iUnitMaxSize(_iUnitMaxSize),mIocpMemoryManagerR(_pIocpMemoryManager)
		{
			this->head=this->tail=NULL;
			this->iTotalSize=0;
			this->b__tem_buffer_valid_=TRUE;
		}
		~MyIocpMemoryChunk_()
		{
			this->clear();
		}
		inline int getTotalSize() const
		{
			return this->iTotalSize;
		}
		inline unsigned char* getUnitDataPtr()
		{
			DefAssertTextWrap(this->iTotalSize>=0,"");
			return this->head!=NULL?((unsigned char*)this->head->pBuffer+this->head->iStartPos):NULL;
		}
		inline int getUnitDataSize() const
		{
			DefAssertTextWrap(this->head==NULL||this->head->iEndPos>=this->head->iStartPos,"");
			return this->head!=NULL?(this->head->iEndPos-this->head->iStartPos):0;
		}
		void pushTail(const unsigned char *_arry,int _iCount)
		{
			DefAssertTextWrap(_iCount>=0,"");
			iTotalSize += _iCount;
			if(this->tail==NULL||(this->tail->iMaxSize-this->tail->iEndPos<_iCount))
			{
				int irsize;
				StructDataInfo *elem=(StructDataInfo*)this->_malloc_(&irsize,(this->iUnitMaxSize>_iCount?this->iUnitMaxSize:_iCount)+sizeof(StructDataInfo));
				irsize-=sizeof(StructDataInfo);
				elem->pBuffer=((unsigned char*)elem)+sizeof(StructDataInfo);
				elem->next=NULL;
				elem->iStartPos=0;
				elem->iEndPos=0;
				elem->iMaxSize=irsize;
				if(this->tail==NULL) this->head=elem;
				else this->tail->next=elem;
				this->tail=elem;
			}
			memcpy((char*)this->tail->pBuffer+this->tail->iEndPos,_arry,_iCount);
			this->tail->iEndPos+=_iCount;
		}
		void clear()
		{
			if(this->iTotalSize!=0)
			{
				StructDataInfo *elem=this->head;
				while(elem!=NULL)
				{
					StructDataInfo *ptr=elem->next;
					this->_free_((void*)elem);
					elem=ptr;
				}
				this->head=this->tail=NULL;
				this->iTotalSize=0;
			}
			this->b__tem_buffer_valid_=TRUE;
		}
		void popFront(int _iCount)
		{
			if(_iCount>=this->iTotalSize)
				this->clear();
			else if(_iCount>0)
			{
				this->iTotalSize-=_iCount;
				while(_iCount>0)
				{
					StructDataInfo* pLcElem=this->head;
					int iLcLen=pLcElem->iEndPos-pLcElem->iStartPos;
					if(iLcLen<=_iCount)
					{
						this->head=pLcElem->next;
						this->_free_((void*)pLcElem);
						if(this->head==NULL) this->tail=NULL;
					}
					else pLcElem->iStartPos+=_iCount;
					_iCount-=iLcLen;
				}
			}
		}
		void reserveStandardTail(int _iMaxSize)
		{
			DefAssertTextWrap(_iMaxSize>0,"");
			if(this->head==NULL) return ;
			while(this->iTotalSize>_iMaxSize&&this->head->next!=NULL)
			{
				DefAssertWrap(FALSE);
				StructDataInfo* pLcElem=this->head;
				this->iTotalSize-=(pLcElem->iEndPos-pLcElem->iStartPos);
				this->head=pLcElem->next;
				this->_free_((void*)pLcElem);
			}
		}
	};

	struct tagIocpOverlappedWork_: public TxIocpService::tagIocpOverlappedWorkBase_
	{
		TxIocpService *const pTxIocpService;
		tagIocpOverlappedWork_(TxIocpService *_pTxIocpService,TxIocpService::FuncType _func):pTxIocpService(_pTxIocpService)
		{
			::InterlockedIncrement(this->pTxIocpService->mMyIocpServiceAtmcVariate.lAtmOverLappedAccretionCount);
			TxIocpService::tagIocpOverlappedWorkBase_::initialize(_func);
		}
		~tagIocpOverlappedWork_()
		{
			::InterlockedDecrement(this->pTxIocpService->mMyIocpServiceAtmcVariate.lAtmOverLappedAccretionCount);
		}
		template<class L,class H>
		inline static void split_hight_low_num(L *_pl,H *_ph,unsigned long long _val)
		{
			DefAssertTextWrap((H)(1.5)==(H)(1)&&(L)(1.5)==(L)(1),"");
			*_pl=(L)(_val);
			*_ph=(H)(_val>>(8*sizeof(L)));
		}
		template<class L,class H>
		inline static unsigned long long merge_hight_low_num(L _l,H _h)
		{
			DefAssertTextWrap((H)(1.5)==(H)(1)&&(L)(1.5)==(L)(1),"");
			return ((unsigned long long)_l)|(((unsigned long long)_h)<<(8*sizeof(L)));
		}
		template<class T>
		inline static T* getObjectOrgOverLappedPointer(OVERLAPPED *_pOVERLAPPED)
		{
#ifndef NDEBUG
			if(0) { tagIocpOverlappedWork_ *_ptr=(T*)(_pOVERLAPPED); }
#endif
			return (T*)((tagIocpOverlappedWork_*)(((size_t)_pOVERLAPPED)-(size_t)&((tagIocpOverlappedWork_*)NULL)->mOVERLAPPED));
		}
		inline void setOffset(unsigned long long _offset) { this->split_hight_low_num(&this->mOVERLAPPED.Offset,&this->mOVERLAPPED.OffsetHigh,_offset); }
	};

	struct TxIocpSocketBasic: public TxCppPlatform::enable_shared_from_this<TxIocpSocketBasic>, private TxForbidDirectCopyObject
	{
	protected:
		SOCKET mSocket;
		TxSystemDependent::TxMutexWrap mutex;
	public :
		TxIocpService *const pTxIocpService;
	public :
		TxIocpSocketBasic(TxIocpService *_pTxIocpService) :pTxIocpService(_pTxIocpService)
		{
			this->mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
			DefAssertTextWrap(this->mSocket != INVALID_SOCKET,"WSASocket() failed");

			if(0)
			{
				struct linger lc_so_linger={0};
				lc_so_linger.l_onoff = TRUE;
				lc_so_linger.l_linger = 5;
				if(::setsockopt(this->mSocket,
					SOL_SOCKET,
					SO_LINGER,
					(char*)&lc_so_linger,
					sizeof(lc_so_linger)!=0))
				{
					DefAssertTextWrap(FALSE,"::setsockopt(SO_LINGER");
				}
			}

			pTxIocpService->_associationSocket_(this->mSocket);
		}
		virtual ~TxIocpSocketBasic() { this->close(); }
		SOCKET getRiskSocket() { return this->mSocket; }
		void lockObject() { this->mutex.lock(); }
		void unlockObject() { this->mutex.unlock(); }
		inline TxIocpService* getIocpService() const { return const_cast<TxIocpSocketBasic*>(this)->pTxIocpService; }
		void close()
		{
			this->lockObject();
			SOCKET s=this->mSocket;
			this->mSocket=INVALID_SOCKET;
			this->unlockObject();
			if(s!=INVALID_SOCKET)
			{
				::shutdown(s, SD_BOTH);
				if (0 != ::closesocket(s))
					DefAssertTextWrap(FALSE,"closesocket() failure");
			}
		}
	};

	class TxIocpTransmitSocket : public TxIocpSocketBasic
	{
	public:
		virtual void _vfReadExceptionEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp)=0;
		virtual void _vfReadBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,const unsigned char *_data,int _size)=0;
		virtual void _vfWriteBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,int _iSurplusSize)=0;
	private:
		unsigned char pReadBuff[10*1024];
		MyIocpMemoryChunk_<5*1024> mWriteBuff;
		LONG __dsfdsdf__0;
		LONG lStandardReserve;
		LONG __dsfdsdf__1;
		LONG bWrittingSign;
		LONG __dsfdsdf__2;
	public:
		TxIocpTransmitSocket(TxIocpService *_pTxIocpService,int _mtu,int _iWriteBufferSize)
			: TxIocpSocketBasic(_pTxIocpService),mWriteBuff(_pTxIocpService->_getIocpMemoryManager__(),_mtu),lStandardReserve((LONG)_iWriteBufferSize)
		{
			this->bWrittingSign=0;
		}
		virtual ~TxIocpTransmitSocket() {}
		void setWriteBufferSize(int _iWriteBufferSize)
		{
			this->lockObject();
			this->lStandardReserve=(LONG)_iWriteBufferSize;
			this->unlockObject();
		}
		void asyncWrite(const void *_data, int _size)
		{
			if(_data==NULL||_size<=0) return ;
			_staticAsyncWrite((const unsigned char*)_data,_size,0,this->shared_from_this(),NULL);
		}
		static void asyncWrite(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,const void *_data, int _size)
		{
			DefAssertTextWrap(dynamic_cast<TxIocpTransmitSocket*>(_sp.get()),"");
			if(_data==NULL||_size<=0) return ;
			_staticAsyncWrite((const unsigned char*)_data,_size,0,_sp,NULL);
		}
		void asyncWrite(const std::string &_data)
		{
			if(_data.length()<=0) return ;
			_staticAsyncWrite((const unsigned char*)_data.c_str(),(int)_data.size(),0,this->shared_from_this(),NULL);
		}
		static void asyncWrite(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,const std::string &_data)
		{
			if(_data.length()<=0) return ;
			DefAssertTextWrap(dynamic_cast<TxIocpTransmitSocket*>(_sp.get()),"");
			_staticAsyncWrite((const unsigned char*)_data.c_str(),(int)_data.size(),0,_sp,NULL);
		}
		static void _staticAsyncWrite(const unsigned char *_data, int _size,const int _iWriteCount,TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,void *_orgOvrLpdPtr)
		{
			struct tagIocpOverlappedWork_Ext : public tagIocpOverlappedWork_
			{
				DWORD dwNumberOfBytesRecvd;
				DWORD dwOrgTotalSize;
				TxCppPlatform::weak_ptr<TxIocpSocketBasic> wp;
				tagIocpOverlappedWork_Ext(TxCppPlatform::shared_ptr<TxIocpSocketBasic> *_sp,TxIocpService *_pTxIocpService,DWORD _dwOrgTotalSize)
					:wp(*_sp),tagIocpOverlappedWork_(_pTxIocpService,g_func),dwOrgTotalSize(_dwOrgTotalSize)
				{
					this->dwNumberOfBytesRecvd=0;
				}
				static bool g_func(OVERLAPPED *_obj,int _iNumberOfBytes,bool _bResultStatus)
				{
					tagIocpOverlappedWork_Ext *pIocpOverlappedWork_Ext=tagIocpOverlappedWork_::getObjectOrgOverLappedPointer<tagIocpOverlappedWork_Ext>(_obj);
					TxCppPlatform::shared_ptr<TxIocpSocketBasic> lc_sp(pIocpOverlappedWork_Ext->wp.lock());
					TxIocpTransmitSocket *pThis=_My_IocpGlobalFunction_::buildDynamicCast<TxIocpTransmitSocket*>(lc_sp.get());
					BOOL bLcDoGood=FALSE;
					if(pThis!=NULL)
					{
						if(_bResultStatus&&_iNumberOfBytes>0)
						{
							((TxIocpTransmitSocket*)lc_sp.get())->_vfWriteBufferEvent(lc_sp,(int)(pIocpOverlappedWork_Ext->dwOrgTotalSize-_iNumberOfBytes));
							_staticAsyncWrite(NULL,0,_iNumberOfBytes,lc_sp,pIocpOverlappedWork_Ext);
							bLcDoGood=TRUE;
						}
						//else pThis->close();
					}
					if(!bLcDoGood)
						pIocpOverlappedWork_Ext->pTxIocpService->_free_(pIocpOverlappedWork_Ext);
					return true;
				}
			};
			tagIocpOverlappedWork_Ext *pIocpOverlappedWork_Ext=(tagIocpOverlappedWork_Ext*)_orgOvrLpdPtr;
			BOOL bLcPushMsgSuc=FALSE;
			if(_sp)
			{
				TxIocpTransmitSocket *pThis=_My_IocpGlobalFunction_::buildDynamicCast<TxIocpTransmitSocket*>(_sp.get());
				WSABUF mLcBuf={0};
				pThis->lockObject();
				if(_iWriteCount>0)
				{
					pThis->mWriteBuff.popFront(_iWriteCount);
					if(pThis->lStandardReserve>0)
						pThis->mWriteBuff.reserveStandardTail(pThis->lStandardReserve);
				}
				if(_size>0)
					pThis->mWriteBuff.pushTail((const unsigned char*)_data,_size);
				if(!pThis->bWrittingSign||_iWriteCount>0)//写操作空闲
				{
					mLcBuf.buf = (char*)pThis->mWriteBuff.getUnitDataPtr();
					mLcBuf.len = pThis->mWriteBuff.getUnitDataSize();
				}
				if(mLcBuf.len>0)
				{
					if(pIocpOverlappedWork_Ext==NULL)
					{
						pIocpOverlappedWork_Ext = pThis->pTxIocpService->_malloc_<tagIocpOverlappedWork_Ext>(
							&_sp,pThis->pTxIocpService,(DWORD)pThis->mWriteBuff.getTotalSize());
					}
					DWORD dwFlags = 0;
					bLcPushMsgSuc=(pThis->getRiskSocket()!=INVALID_SOCKET
						&&(SOCKET_ERROR != ::WSASend(pThis->getRiskSocket(), &mLcBuf,1,
						&pIocpOverlappedWork_Ext->dwNumberOfBytesRecvd, dwFlags,&pIocpOverlappedWork_Ext->mOVERLAPPED, NULL)
						||::WSAGetLastError() == ERROR_IO_PENDING));
				}
				pThis->bWrittingSign=!!bLcPushMsgSuc;
				pThis->unlockObject();
			}
			if(!bLcPushMsgSuc&&pIocpOverlappedWork_Ext!=NULL)
				pIocpOverlappedWork_Ext->pushQueueFailureWork();
		}
		static void staticStartAsyncRead(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,void *_orgOvrLpdPtr)
		{
			struct tagIocpOverlappedWork_Ext : public tagIocpOverlappedWork_
			{
				DWORD dwNumberOfBytesRecvd;
				DWORD dwFlags;
				TxCppPlatform::shared_ptr<TxIocpSocketBasic> sp;
				tagIocpOverlappedWork_Ext(TxCppPlatform::shared_ptr<TxIocpSocketBasic> *_sp,
					TxIocpService *_pTxIocpService)
					:sp(*_sp),tagIocpOverlappedWork_(_pTxIocpService,g_func)
				{
					this->dwNumberOfBytesRecvd=0;
					this->dwFlags=0;
				}
				static bool g_func(OVERLAPPED *_obj,int _iNumberOfBytes,bool _bResultStatus)
				{
					tagIocpOverlappedWork_Ext *pIocpOverlappedWork_Ext=tagIocpOverlappedWork_::getObjectOrgOverLappedPointer<tagIocpOverlappedWork_Ext>(_obj);
					TxIocpTransmitSocket *pThis=_My_IocpGlobalFunction_::buildDynamicCast<TxIocpTransmitSocket*>(pIocpOverlappedWork_Ext->sp.get());
					BOOL bSec=FALSE;
					if(pThis!=NULL)
					{
						if(_iNumberOfBytes>0&&_bResultStatus)
						{
							pThis->_vfReadBufferEvent(pIocpOverlappedWork_Ext->sp,pThis->pReadBuff,_iNumberOfBytes);
							staticStartAsyncRead(pIocpOverlappedWork_Ext->sp,pIocpOverlappedWork_Ext);
							bSec=TRUE;
						}
					}
					if(!bSec)
					{
						if(pThis!=NULL)
						{
							pThis->close();
							pThis->_vfReadExceptionEvent(pIocpOverlappedWork_Ext->sp);
						}
						pIocpOverlappedWork_Ext->pTxIocpService->_free_(pIocpOverlappedWork_Ext);
					}
					return true;
				}
			};
			tagIocpOverlappedWork_Ext *pIocpOverlappedWork_Ext=(tagIocpOverlappedWork_Ext*)_orgOvrLpdPtr;
			TxIocpTransmitSocket *pThis=_My_IocpGlobalFunction_::buildDynamicCast<TxIocpTransmitSocket*>(_sp.get());
			BOOL bLcPushMsgSuc=FALSE;
			if(pThis!=NULL)
			{
				if(pIocpOverlappedWork_Ext==NULL)
				{
					pIocpOverlappedWork_Ext = pThis->pTxIocpService->_malloc_<tagIocpOverlappedWork_Ext>(
						&_sp,pThis->pTxIocpService);
				}
				WSABUF mLcBuf;
				mLcBuf.buf = (char*)pThis->pReadBuff;
				mLcBuf.len = sizeof(pThis->pReadBuff);
				pThis->lockObject();
				bLcPushMsgSuc=(pThis->getRiskSocket()!=INVALID_SOCKET
					&&(SOCKET_ERROR != ::WSARecv(pThis->getRiskSocket(), &mLcBuf, 1, &pIocpOverlappedWork_Ext->dwNumberOfBytesRecvd,
					&pIocpOverlappedWork_Ext->dwFlags,&pIocpOverlappedWork_Ext->mOVERLAPPED, NULL)
					|| ::WSAGetLastError() == ERROR_IO_PENDING));
				pThis->unlockObject();
			}
			if(!bLcPushMsgSuc&&pIocpOverlappedWork_Ext!=NULL)
			{
				pIocpOverlappedWork_Ext->pushQueueFailureWork();
			}
		}
	};

	class MyIocpTcpServerSocketBase_: public TxIocpSocketBasic
	{
	public :
		MyIocpTcpServerSocketBase_(TxIocpService *_pTxIocpService): TxIocpSocketBasic(_pTxIocpService) { }
	public :
		virtual bool onPeerSocketReadExceptionEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp) { (void)&_sp; return false; }
		virtual bool onPeerSocketReadBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,const unsigned char *_data,int _size) { (void)&_sp; (void)_data;(void)_size; return false; }
		virtual bool onPeerSocketWriteBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,int _iSurplusSize) { (void)&_sp; (void)_iSurplusSize; return false; }
	};

	class TxIocpTcpPeerSocket: public TxIocpTransmitSocket
	{
	private:
		TxCppPlatform::shared_ptr<TxIocpSocketBasic> spServerSocket;
	public:
		TxIocpTcpPeerSocket(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_spParentSocket,int _mtu,int _iWriteBufferSize=-1)
			:TxIocpTransmitSocket(_spParentSocket->pTxIocpService,_mtu,_iWriteBufferSize),spServerSocket(_spParentSocket)
		{
		}
		inline TxCppPlatform::shared_ptr<TxIocpSocketBasic>& getServerSocket() { return this->spServerSocket; }
		virtual void _vfReadExceptionEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp)
		{
			MyIocpTcpServerSocketBase_ *pServerSocket=_My_IocpGlobalFunction_::buildDynamicCast<MyIocpTcpServerSocketBase_*>(this->spServerSocket.get());
			if(pServerSocket!=NULL)
				pServerSocket->onPeerSocketReadExceptionEvent(_sp);
		}
		virtual void _vfReadBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,const unsigned char *_data,int _size)
		{
			if(_sp)
			{
				MyIocpTcpServerSocketBase_* pServerSocket=_My_IocpGlobalFunction_::buildDynamicCast<MyIocpTcpServerSocketBase_*>(this->spServerSocket.get());
				if(pServerSocket!=NULL)
					pServerSocket->onPeerSocketReadBufferEvent(_sp,_data,_size);
			}
		}
		virtual void _vfWriteBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,int _iSurplusSize)
		{
			if(_sp)
			{
				MyIocpTcpServerSocketBase_* pServerSocket=_My_IocpGlobalFunction_::buildDynamicCast<MyIocpTcpServerSocketBase_*>(this->spServerSocket.get());
				if(pServerSocket!=NULL)
					pServerSocket->onPeerSocketWriteBufferEvent(_sp,_iSurplusSize);
			}
		}
	};

	class TxIocpTcpServerSocket : public MyIocpTcpServerSocketBase_
	{
	private:
		LPFN_ACCEPTEX lpfnAcceptEx;
	private:
		virtual bool onAcceptPeerSocketEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,const SOCKADDR_IN *_addr) { (void)&_sp; (void)_addr; return false; }
		virtual TxCppPlatform::shared_ptr<TxIocpSocketBasic> onNewSocketEvent()
		{
			return TxCppPlatform::shared_ptr<TxIocpSocketBasic>(new TxIocpTcpPeerSocket(this->shared_from_this(),1440));
		}
	public:
		TxIocpTcpServerSocket(TxIocpService *_pTxIocpService) :MyIocpTcpServerSocketBase_(_pTxIocpService)
		{
			GUID mLcGuidAcceptEx = WSAID_ACCEPTEX;
			DWORD dwBytes = 0;
			this->lpfnAcceptEx = NULL;
			if (::WSAIoctl(this->getRiskSocket(), SIO_GET_EXTENSION_FUNCTION_POINTER,
				&mLcGuidAcceptEx, sizeof (mLcGuidAcceptEx),
				&this->lpfnAcceptEx, sizeof (this->lpfnAcceptEx),
				&dwBytes, NULL, NULL) == SOCKET_ERROR)
			{
				DefAssertTextWrap(FALSE,"WSAIoctl AcceptEx() failure");
			}
		}
		virtual ~TxIocpTcpServerSocket()  { }
		void asyncAccept__(void *_orgOvrLpdPtr=NULL)
		{
			struct tagIocpOverlappedWork_Ext : public tagIocpOverlappedWork_
			{
				tagIocpTcpApiAccpetExCbAddrBuff_ mBothEndPointAddrs;
				TxCppPlatform::shared_ptr<TxIocpSocketBasic> sp;
				TxIocpTcpServerSocket *const pIocpTcpServerSocket;
				DWORD dwBytesReceived;
				tagIocpOverlappedWork_Ext(TxIocpTcpServerSocket *_pIocpTcpServerSocket)
					:tagIocpOverlappedWork_(_pIocpTcpServerSocket->pTxIocpService,g_func),pIocpTcpServerSocket(_pIocpTcpServerSocket)
				{
					memset(&this->mBothEndPointAddrs,0,sizeof(this->mBothEndPointAddrs));
					this->dwBytesReceived=0;
				}
				static bool g_func(OVERLAPPED *_obj,int _iNumberOfBytes,bool _bResultStatus)
				{
					(void)_iNumberOfBytes;
					tagIocpOverlappedWork_Ext *pIocpOverlappedWork_Ext=tagIocpOverlappedWork_::getObjectOrgOverLappedPointer<tagIocpOverlappedWork_Ext>(_obj);
					TxIocpTcpServerSocket *pThis=pIocpOverlappedWork_Ext->pIocpTcpServerSocket;
					BOOL bSec=FALSE;
					if(pThis!=NULL)
					{
						bSec=TRUE;
						if(_bResultStatus)
						{
							pThis->onAcceptPeerSocketEvent(pIocpOverlappedWork_Ext->sp,
								(const SOCKADDR_IN*)&pIocpOverlappedWork_Ext->mBothEndPointAddrs.mRemoteEndPoint.mEndPointAddr);
							if(pIocpOverlappedWork_Ext->sp)
								TxIocpTransmitSocket::staticStartAsyncRead((TxCppPlatform::shared_ptr<TxIocpSocketBasic>)pIocpOverlappedWork_Ext->sp,NULL);
						}
						if(!_bResultStatus)
						{
							pThis->lockObject();
							SOCKET s=pThis->getRiskSocket();
							pThis->unlockObject();
							if(s==INVALID_SOCKET)
								bSec=FALSE;
						}
						if(bSec)
							pThis->asyncAccept__(pIocpOverlappedWork_Ext);
					}
					if(!bSec&&pIocpOverlappedWork_Ext)
					{
						pIocpOverlappedWork_Ext->pTxIocpService->_free_(pIocpOverlappedWork_Ext);
					}
					return true;
				}
			};

			tagIocpOverlappedWork_Ext *pIocpOverlappedWork_Ext=(tagIocpOverlappedWork_Ext*)_orgOvrLpdPtr;
			if(pIocpOverlappedWork_Ext==NULL)
				pIocpOverlappedWork_Ext=this->pTxIocpService->_malloc_<tagIocpOverlappedWork_Ext>(this);
			pIocpOverlappedWork_Ext->sp=this->onNewSocketEvent();
			DefAssertTextWrap(TxCppPlatform::dynamic_pointer_cast<TxIocpTcpPeerSocket>(pIocpOverlappedWork_Ext->sp),
				"onNewSocketEvent() must return object inherit TxIocpTcpPeerSocket!");
			BOOL bLcPushMsgSuc=FALSE;

			this->lockObject();
			bLcPushMsgSuc=(this->getRiskSocket()!=INVALID_SOCKET
				&&(lpfnAcceptEx(
				this->getRiskSocket(),
				pIocpOverlappedWork_Ext->sp->getRiskSocket(),
				&pIocpOverlappedWork_Ext->mBothEndPointAddrs,
				0,
				sizeof(pIocpOverlappedWork_Ext->mBothEndPointAddrs.mLocalEndPoint),
				sizeof(pIocpOverlappedWork_Ext->mBothEndPointAddrs.mRemoteEndPoint),
				&pIocpOverlappedWork_Ext->dwBytesReceived,
				&pIocpOverlappedWork_Ext->mOVERLAPPED)
				|| ::WSAGetLastError() == ERROR_IO_PENDING));
			this->unlockObject();

			if(!bLcPushMsgSuc&&pIocpOverlappedWork_Ext!=NULL)
			{
				DefAssertTextWrap(this->getRiskSocket()==INVALID_SOCKET,"lpfnAcceptEx() failure");
				pIocpOverlappedWork_Ext->pushQueueFailureWork();
			}
		}
		bool listen(int _iPort)
		{
			//初始化Socket
			struct sockaddr_in mLcServerAddress = { 0 };
			mLcServerAddress.sin_family = AF_INET;
			// 这里可以选择绑定任何一个可用的地址，或者是自己指定的一个IP地址	
			mLcServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
			mLcServerAddress.sin_port = htons(_iPort);
			SOCKET s=this->getRiskSocket();
			// 绑定端口
			if (s!=INVALID_SOCKET
				&&SOCKET_ERROR != ::bind(s, (struct sockaddr *) &mLcServerAddress, sizeof(mLcServerAddress))
				&& SOCKET_ERROR != ::listen(s, SOMAXCONN))
			{
				//return true;
				this->asyncAccept__(NULL);
				return true;
			}
			return false;
		}
	};

	class TxIocpTcpClientSocket : public TxIocpTransmitSocket
	{
	private:
		LPFN_CONNECTEX lpfnConnectEx;
	private:
		virtual bool onClientConnectEvent(bool _bResult) { (void)_bResult; return false; }
		virtual bool onClientReadExceptionEvent() { return false; }
		virtual bool onClientReadBufferEvent(const unsigned char *_data,int _size) { (void)_data;(void)_size; return false; }
		virtual bool onClientWriteBufferEvent(int _iSurplusSize) { (void)_iSurplusSize; return false; }
	public:
		TxIocpTcpClientSocket(TxIocpService *_pTxIocpService,int _mtu,int _iWriteBufferSize=-1) :TxIocpTransmitSocket(_pTxIocpService,_mtu,_iWriteBufferSize)
		{
			GUID mLcGuidConnectEx = WSAID_CONNECTEX;
			DWORD dwBytes = 0;
			this->lpfnConnectEx = NULL;
			sockaddr_in local_addr = { 0 };
			local_addr.sin_family = AF_INET;
			SOCKET s = this->getRiskSocket();
			if (SOCKET_ERROR == ::WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER,
				&mLcGuidConnectEx, sizeof(mLcGuidConnectEx),
				&this->lpfnConnectEx, sizeof(this->lpfnConnectEx), &dwBytes, 0, 0))
			{
				DefAssertTextWrap(FALSE, "WSAIoctl failed ConnectEx()");
			}
			while (SOCKET_ERROR == ::bind(s, (sockaddr *)(&local_addr), sizeof(sockaddr_in)))
			{
				int err=(int)::WSAGetLastError();
				if (err == (int)(ERROR_IO_PENDING))
				{
					break;
				}
				else if (err == (int)(WSAENOBUFS))
				{
					::Sleep(1000);
				}
				else
				{
					DefAssertTextWrap(FALSE, "client bind(local) failed");
				}
			}
		}
		virtual ~TxIocpTcpClientSocket() { }
		void asyncConnect(const char _strIp[], int _iPort)
		{
			staticAsyncConnect(_strIp,_iPort,this->shared_from_this());
		}
		static void staticAsyncConnect(const char _strIp[], int _iPort,TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp)
		{
			struct tagIocpOverlappedWork_Ext : public tagIocpOverlappedWork_
			{
				DWORD dwNumberOfBytesRecvd;
				TxCppPlatform::shared_ptr<TxIocpSocketBasic> sp;
				tagIocpOverlappedWork_Ext(TxCppPlatform::shared_ptr<TxIocpSocketBasic> *_sp)
					:tagIocpOverlappedWork_((*_sp)->pTxIocpService,g_func),sp(*_sp)
				{
					this->dwNumberOfBytesRecvd=0;
				}
				static bool g_func(OVERLAPPED *_obj,int _iNumberOfBytes,bool _bResultStatus)
				{
					(void)_iNumberOfBytes;
					tagIocpOverlappedWork_Ext *pIocpOverlappedWork_Ext=tagIocpOverlappedWork_::getObjectOrgOverLappedPointer<tagIocpOverlappedWork_Ext>(_obj);
					TxIocpTcpClientSocket *pThis=(TxIocpTcpClientSocket*)(pIocpOverlappedWork_Ext->sp.get());
					pThis->onClientConnectEvent(_bResultStatus);
					if(_bResultStatus)
						TxIocpTransmitSocket::staticStartAsyncRead(pIocpOverlappedWork_Ext->sp,NULL);
					pIocpOverlappedWork_Ext->pTxIocpService->_free_(pIocpOverlappedWork_Ext);
					return true;
				}
			};
			TxIocpTcpClientSocket *pThis=(TxIocpTcpClientSocket*)_sp.get();
			tagIocpOverlappedWork_Ext *pIocpOverlappedWork_Ext=pThis->pTxIocpService->_malloc_<tagIocpOverlappedWork_Ext>(&_sp);
			sockaddr_in mLcPeerAddr = { 0 };
			mLcPeerAddr.sin_family = AF_INET;
			mLcPeerAddr.sin_addr.s_addr = inet_addr(_strIp);
			mLcPeerAddr.sin_port = htons(_iPort);
			pIocpOverlappedWork_Ext->dwNumberOfBytesRecvd = 0;
			BOOL bLcPushMsgSuc=FALSE;
			pThis->lockObject();
			bLcPushMsgSuc=(pThis->getRiskSocket()!=INVALID_SOCKET
				&&(pThis->lpfnConnectEx(
				pThis->getRiskSocket(),
				(sockaddr*)&mLcPeerAddr,
				sizeof (mLcPeerAddr),
				NULL,
				0,
				&pIocpOverlappedWork_Ext->dwNumberOfBytesRecvd,
				&pIocpOverlappedWork_Ext->mOVERLAPPED)
				|| ::WSAGetLastError() == ERROR_IO_PENDING));
			pThis->unlockObject();
			if(!bLcPushMsgSuc)
			{
				DefAssertTextWrap(pThis->getRiskSocket()==INVALID_SOCKET,"lpfnConnectEx() failure");
				pIocpOverlappedWork_Ext->pushQueueFailureWork();
			}
		}
		virtual void _vfReadExceptionEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp)
		{
			TxIocpTcpClientSocket *pThis=_My_IocpGlobalFunction_::buildDynamicCast<TxIocpTcpClientSocket*>(_sp.get());
			if(pThis!=NULL)
				pThis->onClientReadExceptionEvent();
		}
		virtual void _vfReadBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,const unsigned char *_data,int _size)
		{
			TxIocpTcpClientSocket *pThis=_My_IocpGlobalFunction_::buildDynamicCast<TxIocpTcpClientSocket*>(_sp.get());
			if(pThis!=NULL)
				pThis->onClientReadBufferEvent(_data,_size);
		}
		virtual void _vfWriteBufferEvent(TxCppPlatform::shared_ptr<TxIocpSocketBasic> &_sp,int _iSurplusSize)
		{
			TxIocpTcpClientSocket *pThis=_My_IocpGlobalFunction_::buildDynamicCast<TxIocpTcpClientSocket*>(_sp.get());
			if(pThis!=NULL)
				pThis->onClientWriteBufferEvent(_iSurplusSize);
		}
	};

}

