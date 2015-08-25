
#pragma once

//class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9
#define DefTxRepeatMacroclassAZ0_classAZ0(AZ) 
#define DefTxRepeatMacroclassAZ0_classAZ1(AZ) class AZ##1
#define DefTxRepeatMacroclassAZ0_classAZ2(AZ) DefTxRepeatMacroclassAZ0_classAZ1(AZ),class AZ##2
#define DefTxRepeatMacroclassAZ0_classAZ3(AZ) DefTxRepeatMacroclassAZ0_classAZ2(AZ),class AZ##3
#define DefTxRepeatMacroclassAZ0_classAZ4(AZ) DefTxRepeatMacroclassAZ0_classAZ3(AZ),class AZ##4
#define DefTxRepeatMacroclassAZ0_classAZ5(AZ) DefTxRepeatMacroclassAZ0_classAZ4(AZ),class AZ##5
#define DefTxRepeatMacroclassAZ0_classAZ6(AZ) DefTxRepeatMacroclassAZ0_classAZ5(AZ),class AZ##6
#define DefTxRepeatMacroclassAZ0_classAZ7(AZ) DefTxRepeatMacroclassAZ0_classAZ6(AZ),class AZ##7
#define DefTxRepeatMacroclassAZ0_classAZ8(AZ) DefTxRepeatMacroclassAZ0_classAZ7(AZ),class AZ##8
#define DefTxRepeatMacroclassAZ0_classAZ9(AZ) DefTxRepeatMacroclassAZ0_classAZ8(AZ),class AZ##9

//A1,A2,A3,A4,A5,A6,A7,A8,A9
#define DefTxRepeatMacroAZ0_AZ0(AZ) 
#define DefTxRepeatMacroAZ0_AZ1(AZ) AZ##1
#define DefTxRepeatMacroAZ0_AZ2(AZ) DefTxRepeatMacroAZ0_AZ1(AZ),AZ##2
#define DefTxRepeatMacroAZ0_AZ3(AZ) DefTxRepeatMacroAZ0_AZ2(AZ),AZ##3
#define DefTxRepeatMacroAZ0_AZ4(AZ) DefTxRepeatMacroAZ0_AZ3(AZ),AZ##4
#define DefTxRepeatMacroAZ0_AZ5(AZ) DefTxRepeatMacroAZ0_AZ4(AZ),AZ##5
#define DefTxRepeatMacroAZ0_AZ6(AZ) DefTxRepeatMacroAZ0_AZ5(AZ),AZ##6
#define DefTxRepeatMacroAZ0_AZ7(AZ) DefTxRepeatMacroAZ0_AZ6(AZ),AZ##7
#define DefTxRepeatMacroAZ0_AZ8(AZ) DefTxRepeatMacroAZ0_AZ7(AZ),AZ##8
#define DefTxRepeatMacroAZ0_AZ9(AZ) DefTxRepeatMacroAZ0_AZ8(AZ),AZ##9

//A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9
#define DefTxRepeatMacroAZaz0_AZaz0(AZ,az) 
#define DefTxRepeatMacroAZaz0_AZaz1(AZ,az) AZ##1 az##1
#define DefTxRepeatMacroAZaz0_AZaz2(AZ,az) DefTxRepeatMacroAZaz0_AZaz1(AZ,az),AZ##2 az##2
#define DefTxRepeatMacroAZaz0_AZaz3(AZ,az) DefTxRepeatMacroAZaz0_AZaz2(AZ,az),AZ##3 az##3
#define DefTxRepeatMacroAZaz0_AZaz4(AZ,az) DefTxRepeatMacroAZaz0_AZaz3(AZ,az),AZ##4 az##4
#define DefTxRepeatMacroAZaz0_AZaz5(AZ,az) DefTxRepeatMacroAZaz0_AZaz4(AZ,az),AZ##5 az##5
#define DefTxRepeatMacroAZaz0_AZaz6(AZ,az) DefTxRepeatMacroAZaz0_AZaz5(AZ,az),AZ##6 az##6
#define DefTxRepeatMacroAZaz0_AZaz7(AZ,az) DefTxRepeatMacroAZaz0_AZaz6(AZ,az),AZ##7 az##7
#define DefTxRepeatMacroAZaz0_AZaz8(AZ,az) DefTxRepeatMacroAZaz0_AZaz7(AZ,az),AZ##8 az##8
#define DefTxRepeatMacroAZaz0_AZaz9(AZ,az) DefTxRepeatMacroAZaz0_AZaz8(AZ,az),AZ##9 az##9

//a1,a2,a3,a4,a5,a6,a7,a8,a9
#define DefTxRepeatMacroaz0_az0(az) 
#define DefTxRepeatMacroaz0_az1(az) az##1
#define DefTxRepeatMacroaz0_az2(az) DefTxRepeatMacroaz0_az1(az),az##2
#define DefTxRepeatMacroaz0_az3(az) DefTxRepeatMacroaz0_az2(az),az##3
#define DefTxRepeatMacroaz0_az4(az) DefTxRepeatMacroaz0_az3(az),az##4
#define DefTxRepeatMacroaz0_az5(az) DefTxRepeatMacroaz0_az4(az),az##5
#define DefTxRepeatMacroaz0_az6(az) DefTxRepeatMacroaz0_az5(az),az##6
#define DefTxRepeatMacroaz0_az7(az) DefTxRepeatMacroaz0_az6(az),az##7
#define DefTxRepeatMacroaz0_az8(az) DefTxRepeatMacroaz0_az7(az),az##8
#define DefTxRepeatMacroaz0_az9(az) DefTxRepeatMacroaz0_az8(az),az##9


//
//#include <stdio.h>//12.62
//#include <vector>
//#include <string.h>
//#include <stdlib.h>
//#include <assert.h>
////#include "TxManageFunctionPool.h"
//
//
//template<class A1,class A2,class A3>
//class TxManageFunctionObject
//{
//private:
//	virtual void _execute_(A1,A2,A3)=0;
//	virtual bool _equal_(const void *p,int size) const=0;
//public:
//	virtual ~TxManageFunctionObject() { }
//	void operator()(A1 a1,A2 a2,A3 a3) { this->_execute_(a1,a2,a3); }
//public :
//	template<class ThisType,class FuncRetType>
//	static TxManageFunctionObject* newMemberFunctionObject(ThisType *pThis,FuncRetType (ThisType::*manageFunc)(A1,A2,A3))
//	{
//		typedef decltype(manageFunc) ManageFuncType;
//		class MyHandObj: public TxManageFunctionObject
//		{
//		private:
//			ThisType *const pThis;
//			const ManageFuncType func;
//		public:
//			MyHandObj(ThisType *_pThis,ManageFuncType _manageFunc)
//				: pThis(_pThis),func(_manageFunc) { }
//			void _execute_(A1 a1,A2 a2,A3 a3) { (pThis->*func)(a1,a2,a3); }
//			bool _equal_(const void *p,int size) const { return sizeof(ManageFuncType)==size&&memcmp(p,&func,sizeof(ManageFuncType))==0; }
//		};
//		return new MyHandObj(pThis,manageFunc);
//	}
//public:
//	template<class ThisType,class ValueRetType,class FuncRetType>
//	static TxManageFunctionObject* newMemberFunctionObjectRet(ValueRetType *bindRet,ThisType *pThis,FuncRetType (ThisType::*manageFunc)(A1,A2,A3))
//	{
//		typedef decltype(manageFunc) ManageFuncType;
//		class MyHandObj: public TxManageFunctionObject
//		{
//		private:
//			ThisType *const pThis;
//			ValueRetType *const bindRet;
//			const ManageFuncType func;
//		public:
//			MyHandObj(ValueRetType *_bindRet,ThisType *_pThis,ManageFuncType _manageFunc): bindRet(_bindRet),pThis(_pThis),func(_manageFunc) { }
//			void _execute_(A1 a1,A2 a2,A3 a3) { *bindRet=(pThis->*func)(a1,a2,a3); }
//			bool _equal_(const void *p,int size) const { return sizeof(ManageFuncType)==size&&memcmp(p,&func,sizeof(ManageFuncType))==0; }
//		};
//		return new MyHandObj(bindRet,pThis,manageFunc);
//	}
//public:
//	template<class ManageFuncType>
//	static TxManageFunctionObject* newLambdaFunctionObject(ManageFuncType manageFunc)
//	{
//		class MyHandObj: public TxManageFunctionObject
//		{
//		private:
//			const ManageFuncType func;
//		public:
//			MyHandObj(ManageFuncType _manageFunc): func(_manageFunc) { }
//			void _execute_(A1 a1,A2 a2,A3 a3) { func(a1,a2,a3); }
//			bool _equal_(const void *p,int size) const { return false; }
//		};
//		return new MyHandObj(manageFunc);
//	}
//public:
//	template<class ValueRetType,class ManageFuncType>
//	static TxManageFunctionObject* newLambdaFunctionObjectRet(ValueRetType *bindRet,ManageFuncType manageFunc)
//	{
//		class MyHandObj: public TxManageFunctionObject
//		{
//		private:
//			ValueRetType *const bindRet;
//			const ManageFuncType func;
//		public:
//			MyHandObj(ValueRetType *_bindRet,ManageFuncType _manageFunc): bindRet(_bindRet),func(_manageFunc) { }
//			void _execute_(A1 a1,A2 a2,A3 a3) { *bindRet=func(a1,a2,a3); }
//			bool _equal_(const void *p,int size) const { return false; }
//		};
//		return new MyHandObj(bindRet,manageFunc);
//	}
//public :
//	template<class ManageFuncType,class B1,class B2,class B3,class B4,class B5>
//	static TxManageFunctionObject* newGlobalFunctionObject(ManageFuncType manageFunc,B1 b1,B2 b2,B3 b3,B4 b4,B5 b5)
//	{
//		class MyHandObj: public TxManageFunctionObject
//		{
//		private:
//			const ManageFuncType func;
//			B1 b1; B2 b2; B3 b3; B4 b4; B5 b5;
//		public:
//			MyHandObj(ManageFuncType _manageFunc,B1 _b1,B2 _b2,B3 _b3,B4 _b4,B5 _b5)
//				: func(_manageFunc),b1(_b1),b2(_b2),b3(_b3),b4(_b4),b5(_b5)
//			{
//			}
//			void _execute_(A1 a1,A2 a2,A3 a3) { func(b1,b2,b3,b4,b5,a1,a2,a3); }
//			bool _equal_(const void *p,int size) const { return sizeof(ManageFuncType)==size&&memcmp(p,&func,sizeof(ManageFuncType))==0;; }
//		};
//		return new MyHandObj(manageFunc,b1,b2,b3,b4,b5);
//	}
//public :
//	template<class ValueRetType,class ManageFuncType,class B1,class B2,class B3,class B4,class B5>
//	static TxManageFunctionObject* newGlobalFunctionObjectRet(ValueRetType *bindRet,ManageFuncType manageFunc,B1 b1,B2 b2,B3 b3,B4 b4,B5 b5)
//	{
//		class MyHandObj: public TxManageFunctionObject
//		{
//		private:
//			ValueRetType *const bindRet;
//			const ManageFuncType func;
//			B1 b1; B2 b2; B3 b3; B4 b4; B5 b5;
//		public:
//			MyHandObj(ValueRetType *_bindRet,ManageFuncType _manageFunc,B1 _b1,B2 _b2,B3 _b3,B4 _b4,B5 _b5)
//				: bindRet(_bindRet),func(_manageFunc),b1(_b1),b2(_b2),b3(_b3),b4(_b4),b5(_b5)
//			{
//			}
//			void _execute_(A1 a1,A2 a2,A3 a3) { *bindRet=func(b1,b2,b3,b4,b5,a1,a2,a3); }
//			bool _equal_(const void *p,int size) const { return sizeof(ManageFuncType)==size&&memcmp(p,&func,sizeof(ManageFuncType))==0; }
//		};
//		return new MyHandObj(bindRet,manageFunc,b1,b2,b3,b4,b5);
//	}
//};
//
//
//template<class A1,class A2,class A3>
//class TxManageFunctionPool
//{
//private:
//	class MyHandBase
//	{
//	public:
//		virtual ~MyHandBase() { }
//		virtual void execute(A1,A2,A3)=0;
//		virtual bool equal(const void *p,int size) const=0;
//	};
//	std::vector<MyHandBase*> vManageFuncPool;
//public:
//	~TxManageFunctionPool()
//	{
//		clear();
//	}
//public:
//	template<class ThisType,class FuncRetType>
//	void addMemberFunction(ThisType *pThis,FuncRetType (ThisType::*manageFunc)(A1,A2,A3))
//	{
//		typedef decltype(manageFunc) ManageFuncType;
//		class MyHandObj: public MyHandBase
//		{
//		private:
//			ThisType *const pThis;
//			const ManageFuncType func;
//		public:
//			MyHandObj(ThisType *_pThis,ManageFuncType _manageFunc): pThis(_pThis),func(_manageFunc) { }
//			void execute(A1 a1,A2 a2,A3 a3) { (pThis->*func)(a1,a2,a3); }
//			bool equal(const void *p,int size) const { return sizeof(ManageFuncType)==size&&memcmp(p,&func,sizeof(ManageFuncType))==0; }
//		};
//		this->vManageFuncPool.push_back(new MyHandObj(pThis,manageFunc));
//	}
//public:
//	template<class ThisType,class ValueRetType,class FuncRetType>
//	void addMemberFunctionRet(ValueRetType *bindRet,ThisType *pThis,FuncRetType (ThisType::*manageFunc)(A1,A2,A3))
//	{
//		typedef decltype(manageFunc) ManageFuncType;
//		class MyHandObj: public MyHandBase
//		{
//		private:
//			ThisType *const pThis;
//			ValueRetType *const bindRet;
//			const ManageFuncType func;
//		public:
//			MyHandObj(ValueRetType *_bindRet,ThisType *_pThis,ManageFuncType _manageFunc): bindRet(_bindRet),pThis(_pThis),func(_manageFunc) { }
//			void execute(A1 a1,A2 a2,A3 a3) { *bindRet=(pThis->*func)(a1,a2,a3); }
//			bool equal(const void *p,int size) const { return sizeof(ManageFuncType)==size&&memcmp(p,&func,sizeof(ManageFuncType))==0; }
//		};
//		this->vManageFuncPool.push_back(new MyHandObj(bindRet,pThis,manageFunc));
//	}
//public:
//	template<class ManageFuncType>
//	void addLambdaFunction(ManageFuncType manageFunc)
//	{
//		class MyHandObj: public MyHandBase
//		{
//		private:
//			const ManageFuncType func;
//		public:
//			MyHandObj(ManageFuncType _manageFunc): func(_manageFunc) { }
//			void execute(A1 a1,A2 a2,A3 a3) { func(a1,a2,a3); }
//			bool equal(const void *p,int size) const { return false; }
//		};
//		this->vManageFuncPool.push_back(new MyHandObj(manageFunc));
//	}
//public:
//	template<class ValueRetType,class ManageFuncType>
//	void addLambdaFunctionRet(ValueRetType *bindRet,ManageFuncType manageFunc)
//	{
//		class MyHandObj: public MyHandBase
//		{
//		private:
//			ValueRetType *const bindRet;
//			const ManageFuncType func;
//		public:
//			MyHandObj(ValueRetType *_bindRet,ManageFuncType _manageFunc): bindRet(_bindRet),func(_manageFunc) { }
//			void execute(A1 a1,A2 a2,A3 a3) { *bindRet=func(a1,a2,a3); }
//			bool equal(const void *p,int size) const { return false; }
//		};
//		this->vManageFuncPool.push_back(new MyHandObj(bindRet,manageFunc));
//	}
//public :
//	template<class ManageFuncType,class B1,class B2,class B3,class B4,class B5>
//	void addGlobalFunction(ManageFuncType manageFunc,B1 b1,B2 b2,B3 b3,B4 b4,B5 b5)
//	{
//		class MyHandObj: public MyHandBase
//		{
//		private:
//			const ManageFuncType func;
//			B1 b1; B2 b2; B3 b3; B4 b4; B5 b5;
//		public:
//			MyHandObj(ManageFuncType _manageFunc,B1 _b1,B2 _b2,B3 _b3,B4 _b4,B5 _b5)
//				: func(_manageFunc),b1(_b1),b2(_b2),b3(_b3),b4(_b4),b5(_b5)
//			{
//			}
//			void execute(A1 a1,A2 a2,A3 a3) { func(b1,b2,b3,b4,b5,a1,a2,a3); }
//			bool equal(const void *p,int size) const { return sizeof(ManageFuncType)==size&&memcmp(p,&func,sizeof(ManageFuncType))==0;; }
//		};
//		this->vManageFuncPool.push_back(new MyHandObj(manageFunc,b1,b2,b3,b4,b5));
//	}
//public ://,class B1,class B2,class B3,class B4,class B5
//	template<class ValueRetType,class ManageFuncType,class B1,class B2,class B3,class B4,class B5>
//	void addGlobalFunctionRet(ValueRetType *bindRet,ManageFuncType manageFunc,B1 b1,B2 b2,B3 b3,B4 b4,B5 b5)
//	{
//		class MyHandObj: public MyHandBase
//		{
//		private:
//			ValueRetType *const bindRet;
//			const ManageFuncType func;
//			B1 b1; B2 b2; B3 b3; B4 b4; B5 b5;
//		public:
//			MyHandObj(ValueRetType *_bindRet,ManageFuncType _manageFunc,B1 _b1,B2 _b2,B3 _b3,B4 _b4,B5 _b5)
//				: bindRet(_bindRet),func(_manageFunc),b1(_b1),b2(_b2),b3(_b3),b4(_b4),b5(_b5)
//			{
//			}
//			void execute(A1 a1,A2 a2,A3 a3) { *bindRet=func(b1,b2,b3,b4,b5,a1,a2,a3); }
//			bool equal(const void *p,int size) const { return sizeof(ManageFuncType)==size&&memcmp(p,&func,sizeof(ManageFuncType))==0; }
//		};
//		this->vManageFuncPool.push_back(new MyHandObj(bindRet,manageFunc,b1,b2,b3,b4,b5));
//	}
//public:
//	template<class FuncType>
//	void eraseManageFunc(FuncType _recordUserFunc)
//	{
//		auto aManageFuncPool=this->vManageFuncPool.data();
//		int vManageFuncPoolSize=this->vManageFuncPool.size();
//		int pos=0; assert(sizeof(_recordUserFunc)==sizeof(void*));
//		for(int i=pos=0;i<vManageFuncPoolSize;i++)
//		{
//			if(aManageFuncPool[i]->equal(&_recordUserFunc,sizeof(_recordUserFunc)))
//				delete aManageFuncPool[i];
//			else aManageFuncPool[pos++]=aManageFuncPool[i];
//		}
//		this->vManageFuncPool.erase(this->vManageFuncPool.begin()+pos,this->vManageFuncPool.end());
//	}
//	void clear()
//	{
//		auto aManageFuncPool=this->vManageFuncPool.data();
//		int vManageFuncPoolSize=this->vManageFuncPool.size();
//		for(int i=0;i<vManageFuncPoolSize;i++)
//			delete aManageFuncPool[i];
//		this->vManageFuncPool.clear();
//	}
//public :
//	void execute(A1 a1,A2 a2,A3 a3)
//	{
//		auto aManageFuncPool=this->vManageFuncPool.data();
//		int vManageFuncPoolSize=this->vManageFuncPool.size();
//		for(int i=0;i<vManageFuncPoolSize;i++)
//			aManageFuncPool[i]->execute(a1,a2,a3);
//	}
//};
//
//
//class TTT
//{
//public:
//	int func(char x,long double y,char z)
//	{
//		return y;
//	}
//};
//
//double globalfun(char a1,char a2,double a3,char a4,long long a5,char t1,double t2,char t3)
//{
//	int sdfs=0;
//	return 1+257;
//}
//
//int funffc(int x,int y,int z,int m,int n)
//{
//	return 0;
//}
////class C1,class C2,class C3
//#define  FIF(C) \
//	template<DefTxRepeatMacroclassAZ0_classAZ5(C)> \
//	void KKK(CC1 c1,CC2 c2,CC3 c3,CC4 c4,CC5 c5)\
//{\
//	funffc(c1,c2,c3,c4,c5);\
//}
//
//FIF(CC)
//
//
//////class C1,class C2,class C3
////#define AAABBBCCC(A) \
////	template<DefTxRepeatMacroclassAZ0_classAZ5(A)> void KKK(C1 c1,C2 c2,C3 c3,C4 c4,C5 c5) { }
////AAABBBCCC(C)
//
//int main()
//{
//	KKK(1,2,3.0,4,5.0f);
//	TTT lf;
//	long double ret;
//	TxManageFunctionPool<char,long double,char> af;
//	af.addMemberFunctionRet(&ret,&lf,&TTT::func);
//	af.addGlobalFunction(&globalfun,1,2,3,4,5);
//	af.addGlobalFunctionRet(&ret,globalfun,1,2,3,4,5);
//	af.addLambdaFunctionRet(&ret,[ret](char x,long double y,char z)->int { return 1+20+30; });
//	af.execute(8,9,10);
//
//	TxManageFunctionObject<char,long double,char> *of;
//	of=TxManageFunctionObject<char,long double,char>::newMemberFunctionObjectRet(&ret,&lf,&TTT::func);
//	(*of)(8,9,10);
//	of=TxManageFunctionObject<char,long double,char>::newGlobalFunctionObjectRet(&ret,globalfun,1,2,3,4,5);
//	(*of)(8,9,10);
//	of=TxManageFunctionObject<char,long double,char>::newLambdaFunctionObjectRet(&ret,[ret](char x,long double y,char z)->int { return 1+20+30; });
//	(*of)(8,9,10);
//
//	return 0;
//}
//
