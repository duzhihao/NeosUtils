/*
	Author:		ZhiHao DU
	Date:		2016/12/16
	Function:	Standard C++ object memory pool
	Usage:		The class T must have two pointer members, one is "T* m_pPre", the other is "T* m_pNext"
	Lisence:	GPL
	Note:		This class is NOT thread-safe before C++ 11, you should give every thread a pool. 
				If you wanna a thread-safe version, please use C++ 11 compiler and add user define 'USE_CPP_11', then recompile it.

	Enjoy it!(^.^)
*/

#ifndef __OBJECT_POOL_H__
#define __OBJECT_POOL_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define USE_CPP_11

#ifdef USE_CPP_11
#include <mutex>
#endif

namespace NeosUtils
{
	
	template<class T>
	class ObjectPool
	{
	private:
		T* m_pPool;
		T* m_pUsedListHeader;
		T* m_pFreeListHeader;
		volatile uint32_t m_uPoolSize;
		volatile uint32_t m_uUsedNumber;
		volatile uint32_t m_uFreeNumber;
#ifdef USE_CPP_11
		std::mutex m_mtxMutex;
#endif

	public:
		typedef int(*VisitFunc)(T*);
		T* AllocObject();
		int FreeObject(T*& obj);
		int InitPool(uint32_t size);
		int ViewPool(VisitFunc _func, int flag);
		ObjectPool();
		~ObjectPool();

	private:
		bool IsEmpty()
		{
			if (m_uFreeNumber > 0)
				return false;
			else
				return true;
		}
		inline uint32_t GetUsedNumber()
		{ 
			return m_uUsedNumber; 
		}
		inline uint32_t GetFreeNumber()
		{
			return m_uFreeNumber; 
		}
		inline uint32_t GetPoolSize()
		{ 
			return m_uPoolSize; 
		}
	};

	template<class T>
	int ObjectPool<T>::ViewPool(VisitFunc _func, int flag)
	{
		if (_func == NULL)
			return -1;
		T* tmp = NULL;
		if (flag & 1){
			printf("Visit Free Object List...length = %u\n", m_uFreeNumber);
			tmp = m_pFreeListHeader->m_pNext;
			while (tmp != NULL){
				_func(tmp);
				tmp = tmp->m_pNext;
			}
		}
		if (flag & 2){
			printf("Visit Used Object List...length = %u\n", m_uUsedNumber);
			tmp = m_pUsedListHeader->m_pNext;
			while (tmp != NULL){
				_func(tmp);
				tmp = tmp->m_pNext;
			}
		}
		return 0;
	}

	template<class T>
	T* ObjectPool<T>::AllocObject()
	{
		T* tmp = NULL;
		if (IsEmpty())
			return tmp;
#ifdef USE_CPP_11
		if (m_mtxMutex.try_lock()){
#endif
			tmp = m_pFreeListHeader->m_pNext;
			m_pFreeListHeader->m_pNext = tmp->m_pNext;
			if (tmp->m_pNext != NULL)
				tmp->m_pNext->m_pPre = tmp->m_pPre;

			tmp->m_pNext = m_pUsedListHeader->m_pNext;
			m_pUsedListHeader->m_pNext = tmp;
			tmp->m_pPre = m_pUsedListHeader;
			if (tmp->m_pNext != NULL)
				tmp->m_pNext->m_pPre = tmp;

			m_uFreeNumber--;
			m_uUsedNumber++;
#ifdef USE_CPP_11
			m_mtxMutex.unlock();
		}
#endif
		return tmp;
	}

	template<class T>
	int ObjectPool<T>::FreeObject(T*& obj)
	{
		if (obj == NULL)
			return -1;
#ifdef USE_CPP_11
		if (m_mtxMutex.try_lock()){
#endif
			obj->m_pPre->m_pNext = obj->m_pNext;
			if (obj->m_pNext != NULL)
				obj->m_pNext->m_pPre = obj->m_pPre;

			obj->m_pNext = m_pFreeListHeader->m_pNext;
			m_pFreeListHeader->m_pNext = obj;
			obj->m_pPre = m_pFreeListHeader;
			if (obj->m_pNext != NULL)
				obj->m_pNext->m_pPre = obj;

			obj = NULL;
			m_uFreeNumber++;
			m_uUsedNumber--;
#ifdef USE_CPP_11
			m_mtxMutex.unlock();
		}
#endif
		return 0;
	}

	template<class T>
	int ObjectPool<T>::InitPool(uint32_t size)
	{
		m_uPoolSize = size;
		
		m_pPool = new T[size];
		if (m_pPool == NULL)
			return -1;

		m_pUsedListHeader = new T();
		m_pUsedListHeader->m_pNext = NULL;
		m_pUsedListHeader->m_pPre = NULL;

		m_pFreeListHeader = new T();
		m_pFreeListHeader->m_pPre = NULL;
		m_pFreeListHeader->m_pNext = m_pPool;
		m_pPool->m_pPre = m_pFreeListHeader;
		for (uint32_t i = 1; i < m_uPoolSize; i++){
			m_pPool[i].m_pPre = m_pPool + (i - 1);
			m_pPool[i - 1].m_pNext = m_pPool + i;
		}
		m_pPool[m_uPoolSize - 1].m_pNext = NULL;

		m_uUsedNumber = 0;
		m_uFreeNumber = m_uPoolSize;
		
		return 0;
	}

	template<class T>
	ObjectPool<T>::ObjectPool()
	{
		m_pPool = NULL;
		m_pUsedListHeader = NULL;
		m_pFreeListHeader = NULL;
		m_uUsedNumber = 0;
		m_uFreeNumber = 0;
		m_uPoolSize = 0;
	}

	template<class T>
	ObjectPool<T>::~ObjectPool()
	{
		if (m_pUsedListHeader != NULL)
			delete m_pUsedListHeader;
		if (m_pFreeListHeader != NULL)
			delete m_pFreeListHeader;
		if (m_pPool != NULL)
			delete m_pPool;

		m_uUsedNumber = 0;
		m_uFreeNumber = 0;
		m_uPoolSize = 0;
	}
}

#endif