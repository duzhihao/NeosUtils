#include "NObjectPool.h"
#include "NQueue.h"
#include <stdio.h>
#include <stdlib.h>

struct SampleObject
{
	SampleObject* m_pPre;
	SampleObject* m_pNext;
	int m_iData;
};

int PrintObject(SampleObject* obj)
{
	printf("Pre:%p, This:%p, Next:%p, data: %d\n", obj->m_pPre, obj, obj->m_pNext, obj->m_iData);
	return 0;
}

int SetObject(SampleObject* obj)
{
	obj->m_iData = 0;
	return 0;
}

int main()
{
	printf("Hello ObjectPool!\n");
	NeosUtils::ObjectPool<SampleObject> pool;
	NeosUtils::NQueue<SampleObject> queue(10);
	pool.InitPool(10);
	pool.ViewPool(SetObject, 3);
	pool.ViewPool(PrintObject, 3);
	SampleObject* tmp = NULL;

	int cnt = 0;
	int rst = 0;
	while (true)
	{
		rst = rand() % 2;
		if (rst == 0){
			printf("Alloc a object!\n");
			if ((tmp = pool.AllocObject()) == NULL)
				break;
			pool.ViewPool(PrintObject, 3);
			queue.Push(tmp);
			queue.ViewQueue(PrintObject);
			system("pause");
		}
		if (rst == 1){
			printf("Free a object!\n");
			tmp = queue.Front();
			queue.Pop();
			pool.FreeObject(tmp);
			pool.ViewPool(PrintObject, 3);
			queue.ViewQueue(PrintObject);
			system("pause");
		}

	}

	return 0;
}