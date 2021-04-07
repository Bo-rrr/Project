#include "CentralCache.h"
#include "PageCache.h"

Span* CentralCache::GetOneSpan(size_t size)
{
	size_t index = SizeClass::ListIndex(size);
	SpanList& spanlist = _spanLists[index];
	Span* it = spanlist.Begin();
	while (it != spanlist.End())
	{
		if (!it->_freeList.Empty())
		{
			return it;
		}
		else
		{
			it = it->_next;
		}
	}

	// page cache 获取一个span
	size_t numpage = SizeClass::NumMovePage(size);
	Span* span = pageCacheInst.NewSpan(numpage);
	// 把span对象切成对应大小挂到span的freelist中
	char* start = (char*)(span->_pageid<<12);
	char* end = start + (span->_pagesize << 12);
	while (start < end)
	{
		char* obj = start;
		start += size;

		span->_freeList.Push(obj);
	}
	span->_objSize = size;
	spanlist.PushFront(span);

	return span;
}

size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t size)
{
	size_t index = SizeClass::ListIndex(size);
	SpanList& spanlist = _spanLists[index];
	spanlist.Lock();

	Span* span = GetOneSpan(size);
	FreeList& freelist = span->_freeList;
	size_t actualNum = freelist.PopRange(start, end, num);
	span->_usecount += actualNum;

	spanlist.Unlock();

	return actualNum;
}

void CentralCache::ReleaseListToSpans(void* start, size_t size)
{
	size_t index = SizeClass::ListIndex(size);
	SpanList& spanlist = _spanLists[index];
	spanlist.Lock();

	while (start)
	{
		void* next = NextObj(start);
		PAGE_ID id = (PAGE_ID)start >> PAGE_SHIFT;
		Span* span = pageCacheInst.GetIdToSpan(id);
		span->_freeList.Push(start);
		span->_usecount--;

		// 表示当前span切出去的对象全部返回，可以将span还给page cache,进行合并，减少内存碎片。
		if (span->_usecount == 0)
		{
			size_t index = SizeClass::ListIndex(span->_objSize);
			_spanLists[index].Erase(span);
			span->_freeList.Clear();

			pageCacheInst.ReleaseSpanToPageCache(span);
		}

		start = next;
	}

	spanlist.Unlock();
}