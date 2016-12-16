/*
	Author:		ZhiHao DU
	Date:		2016/12/16
	Function:	Standard C++ object FIFO Queue
	Lisence:	GPL
	Note:		This class is NOT thread-safe before C++ 11, you can give every thread a queue for thread-safe.
				If you wanna a thread-safe version, please use C++ 11 compiler and add user define 'USE_CPP_11', then recompile it.

	Enjoy it!(^.^)
*/

#ifndef __N_QUEUE_H__
#define __N_QUEUE_H__

#include <stdint.h>
#include <stdio.h>
#ifdef USE_CPP_11
#include <mutex>
#endif

namespace NeosUtils
{
	template<class T>
	class NQueue
	{
	public:
		typedef int(*VisitFunc)(T*);
		inline bool IsEmpty(){ return m_uNumberInQueue == 0; }
		inline bool IsFull(){ return m_uNumberInQueue == m_uQueueSize; }
		inline uint32_t GetNumberInQueue(){ return m_uNumberInQueue; }
		int Push(T* obj)
		{
			if (!IsFull()){
#ifdef USE_CPP_11
				if (m_mtxMutex.try_lock()){
#endif
					m_ppQueue[m_uEndIndex] = obj;
					m_uEndIndex = (m_uEndIndex + 1) % m_uQueueSize;
					m_uNumberInQueue++;
#ifdef USE_CPP_11
					m_mtxMutex.unlock();
				}
#endif
				return 0;
			}
			return -1;
		}
		int Pop()
		{
			if (!IsEmpty()){
#ifdef USE_CPP_11
				if (m_mtxMutex.try_lock()){
#endif
					m_uStartIndex = (m_uStartIndex + 1) % m_uQueueSize;
					m_uNumberInQueue--;
#ifdef USE_CPP_11
				m_mtxMutex.unlock();
			}
#endif
				return 0;
			}
			return -1;
		}
		inline T* Front(){ 
			if (!IsEmpty())
				return m_ppQueue[m_uStartIndex]; 
			return NULL;
		}
		inline T* Back(){
			if (!IsEmpty())
				return m_ppQueue[m_uEndIndex];
			return NULL;
		}
		int ViewQueue(VisitFunc _fpFunc);
		NQueue(uint32_t size);
		~NQueue();
	private:
		T** m_ppQueue;
		uint32_t m_uStartIndex;
		uint32_t m_uEndIndex;
		uint32_t m_uQueueSize;
		uint32_t m_uNumberInQueue;
#ifdef USE_CPP_11
		std::mutex m_mtxMutex;
#endif
	};

	template<class T>
	NQueue<T>::NQueue(uint32_t size)
	{
		m_uStartIndex = 0;
		m_uEndIndex = 0;
		m_uNumberInQueue = 0;
		m_uQueueSize = size;
		m_ppQueue = new T*[m_uQueueSize];
	}

	template<class T>
	int NQueue<T>::ViewQueue(VisitFunc _fpFunc)
	{
		if (IsEmpty())
			return 0;
		printf("View Queue...Length = %u\n", m_uNumberInQueue);
		uint32_t i = m_uStartIndex;
		do
		{
			_fpFunc(m_ppQueue[i]);
			i = (i + 1) % m_uQueueSize;
		} while (i != m_uEndIndex);
		return 0;
	}

	template<class T>
	NQueue<T>::~NQueue()
	{
		m_uStartIndex = 0;
		m_uEndIndex = 0;
		m_uQueueSize = 0;
		m_uNumberInQueue = 0;
		delete m_ppQueue;
	}
}

#endif