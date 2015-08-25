#pragma once
#include <Windows.h>

template<class T>
class TxAtomicInteger
{
public :
	TxAtomicInteger(T _data=0)
	{
		if(sizeof(T)<=sizeof(LONG))
			*(LONG*)&this->mBuffer=(LONG)_data;
		*(LONGLONG*)&this->mBuffer=(LONGLONG)_data;
	}
	TxAtomicInteger(const TxAtomicInteger &_obj)
	{
		if(sizeof(T)<=sizeof(LONG))
			*(LONG*)&this->mBuffer=*(LONG*)&_obj.mBuffer;
		*(LONGLONG*)&this->mBuffer=*(LONGLONG*)&_obj.mBuffer;
	}
	TxAtomicInteger& operator=(TxAtomicInteger& _tem)
	{
		this->setValue(_tem.getValue());
		return *this;
	}
	T exchange(T _data)
	{
		if(sizeof(T)<=sizeof(LONG))
			return (T)::InterlockedExchange((LONG*)&this->mBuffer,(LONG)_data);
		return (T)::InterlockedExchange64((LONGLONG*)&this->mBuffer,(LONGLONG)_data);
	}
	T plus(T _data)
	{
		if(sizeof(T)<=sizeof(LONG))
		{
			if(_data==-1) return (T)::InterlockedDecrement((LONG*)&this->mBuffer);
			if(_data==1) return (T)::InterlockedIncrement((LONG*)&this->mBuffer);
			return (T)::InterlockedExchangeAdd((LONG*)&this->mBuffer,(LONG)_data)+_data;
		}
		if(_data==-1) return (T)::InterlockedDecrement64((LONGLONG*)&this->mBuffer);
		if(_data==1) return (T)::InterlockedIncrement64((LONGLONG*)&this->mBuffer);
		return (T)::InterlockedExchangeAdd64((LONGLONG*)&this->mBuffer,(LONGLONG)_data)+_data;
	}
	T minus(T _data)
	{
		return this->plus(-_data);
	}
	void setValue(T _data)
	{
		this->exchange(_data);
	}
	T getValue()
	{
		return this->plus((T)0);
	}
private:
	__declspec(align(8)) struct { char _buf__[sizeof(T)<=sizeof(LONG)?sizeof(LONG):sizeof(LONGLONG)]; } mBuffer;
};

//#include <stdio.h>
//int main()
//{
//	{
//		TxAtomicInteger<int> a;
//		a.setValue(12);
//		a.plus(100);
//		int jlk=a.getValue();
//		TxAtomicInteger<char> b;
//		b.plus(100);
//		TxAtomicInteger<long long> c;
//		c.plus(100);
//		c.setValue(10000);
//	}
//	char buff[64]={0};
//	const int n=(int)(1e6)/5;
//	DWORD t1=::GetTickCount();
//	for(int i=0;i<n;i++)
//	{
//		InterlockedIncrement((LONG*)&buff[0]);
//		InterlockedIncrement((LONG*)&buff[0]);
//		InterlockedIncrement((LONG*)&buff[0]);
//		InterlockedIncrement((LONG*)&buff[0]);
//		InterlockedIncrement((LONG*)&buff[0]);
//	}
//	DWORD t2=::GetTickCount();
//	for(int i=0;i<n;i++)
//	{
//		InterlockedExchangeAdd((LONG*)&buff[0],(LONG)100000);
//		InterlockedExchangeAdd((LONG*)&buff[0],(LONG)100000);
//		InterlockedExchangeAdd((LONG*)&buff[0],(LONG)100000);
//		InterlockedExchangeAdd((LONG*)&buff[0],(LONG)100000);
//		InterlockedExchangeAdd((LONG*)&buff[0],(LONG)100000);
//	}
//	DWORD t3=::GetTickCount();
//	for(int i=0;i<n;i++)
//	{
//		InterlockedAnd64((LONGLONG*)&buff[0],-1);
//		InterlockedAnd64((LONGLONG*)&buff[0],-1);
//		InterlockedAnd64((LONGLONG*)&buff[0],-1);
//		InterlockedAnd64((LONGLONG*)&buff[0],-1);
//		InterlockedAnd64((LONGLONG*)&buff[0],-1);
//	}
//	DWORD t4=::GetTickCount();
//	for(int i=0;i<n;i++)
//	{
//		InterlockedExchangeAdd64((LONGLONG*)&buff[0],0);
//		InterlockedExchangeAdd64((LONGLONG*)&buff[0],0);
//		InterlockedExchangeAdd64((LONGLONG*)&buff[0],0);
//		InterlockedExchangeAdd64((LONGLONG*)&buff[0],0);
//		InterlockedExchangeAdd64((LONGLONG*)&buff[0],0);
//	}
//	DWORD t5=::GetTickCount();
//	for(int i=0;i<n;i++)
//	{
//		InterlockedIncrement64((LONGLONG*)&buff[0]);
//		InterlockedIncrement64((LONGLONG*)&buff[0]);
//		InterlockedIncrement64((LONGLONG*)&buff[0]);
//		InterlockedIncrement64((LONGLONG*)&buff[0]);
//		InterlockedIncrement64((LONGLONG*)&buff[0]);
//	}
//	DWORD t6=::GetTickCount();
//	TxAtomicInteger<char> atCh8;
//	for(int i=0;i<n;i++)
//	{
//		atCh8.plus(1);
//		atCh8.plus(1);
//		atCh8.plus(1);
//		atCh8.plus(1);
//		atCh8.plus(1);
//	}
//	DWORD t7=::GetTickCount();
//	TxAtomicInteger<int> atCh32;
//	for(int i=0;i<n;i++)
//	{
//		atCh32.plus(1);
//		atCh32.plus(1);
//		atCh32.plus(1);
//		atCh32.plus(1);
//		atCh32.plus(1);
//	}
//	DWORD t8=::GetTickCount();
//	TxAtomicInteger<long long> atCh64;
//	TxAtomicInteger<long double> atCh64d;
//	for(int i=0;i<n;i++)
//	{
//		atCh64.plus(1);
//		atCh64.plus(1);
//		atCh64.plus(1);
//		atCh64.plus(1);
//		atCh64.plus(1);
//	}
//	DWORD t9=::GetTickCount();
//	printf("time:\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n",
//		(double)((t2-t1)/1000.0),
//		(double)((t3-t2)/1000.0),
//		(double)((t4-t3)/1000.0),
//		(double)((t5-t4)/1000.0),
//		(double)((t6-t5)/1000.0),
//		(double)((t7-t6)/1000.0),
//		(double)((t8-t7)/1000.0),
//		(double)((t9-t8)/1000.0));
//	return 0;
//}
