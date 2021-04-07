#pragma once

#include "Common.h"

class ThreadCache
{
public:
	// �����ڴ���ͷ��ڴ�
	void* Allocte(size_t size);
	void Deallocte(void* ptr, size_t size);

	// �����Ļ����ȡ����
	void* FetchFromCentralCache(size_t index);

	// ������������ж��󳬹�һ�����Ⱦ�Ҫ�ͷŸ����Ļ���
	void ListTooLong(FreeList& freeList, size_t num, size_t size);
private:
	FreeList _freeLists[NFREE_LIST];

	//ThreadCache* _next;
	//int threadid;
};

// ThreadCache* tclist = nullptr;
// �߳�TLS Thread Local Storage

_declspec (thread) static ThreadCache* pThreaCache = nullptr;