#include "PageCache.h"

Span* PageCache::NewSpan(size_t numpage)
{
	//_spanLists[numpage].Lock();
	if (!_spanLists[numpage].Empty())
	{
		Span* span = _spanLists[numpage].Begin();
		_spanLists[numpage].PopFront();
		return span;
	}

	for (size_t i = numpage+1; i < MAX_PAGES; ++i)
	{
		if (!_spanLists[i].Empty())
		{
			// 分裂
			Span* span = _spanLists[i].Begin();
			_spanLists[i].PopFront();

			Span* splitspan = new Span;
			splitspan->_pageid = span->_pageid + span->_pagesize - numpage;
			splitspan->_pagesize = numpage;
			for (PAGE_ID i = 0; i < numpage; ++i)
			{
				_idSpanMap[splitspan->_pageid + i] = splitspan;
			}

			span->_pagesize -= numpage;

			_spanLists[span->_pagesize].PushFront(span);

			return splitspan;
		}
	}

	void* ptr = SystemAlloc(MAX_PAGES - 1);

	Span* bigspan = new Span;
	bigspan->_pageid = (PAGE_ID)ptr >> PAGE_SHIFT;
	bigspan->_pagesize = MAX_PAGES - 1;

	for (PAGE_ID i = 0; i < bigspan->_pagesize; ++i)
	{
		//_idSpanMap.insert(std::make_pair(bigspan->_pageid + i, bigspan));
		_idSpanMap[bigspan->_pageid + i] = bigspan;
	}

	_spanLists[bigspan->_pagesize].PushFront(bigspan);
	
	return NewSpan(numpage);
}

void PageCache::ReleaseSpanToPageCache(Span* span)
{
	// 向前合并
	while (1)
	{
		PAGE_ID prevPageId = span->_pageid - 1;
		auto pit = _idSpanMap.find(prevPageId);
		// 前面的页不存在
		if (pit == _idSpanMap.end())
		{
			break;
		}

		// 说明前一个也还在使用中，不能合并
		Span* prevSpan = pit->second;
		if (prevSpan->_usecount != 0)
		{
			break;
		}

		// 合并
		span->_pageid = prevSpan->_pageid;
		span->_pagesize += prevSpan->_pagesize;
		for (PAGE_ID i = 0; i < prevSpan->_pagesize; ++i)
		{
			_idSpanMap[prevSpan->_pageid + i] = span;
		}


		_spanLists[prevSpan->_pagesize].Erase(prevSpan);
		delete prevSpan;
	}


	// 向后合并
	while (1)
	{
		PAGE_ID nextPageId = span->_pageid + span->_pagesize;
		auto nextIt = _idSpanMap.find(nextPageId);
		if (nextIt == _idSpanMap.end())
		{
			break;
		}

		Span* nextSpan = nextIt->second;
		if (nextSpan->_usecount != 0)
		{
			break;
		}

		span->_pagesize += nextSpan->_pagesize;
		for (PAGE_ID i = 0; i < nextSpan->_pagesize; ++i)
		{
			_idSpanMap[nextSpan->_pageid + i] = span;
		}

		_spanLists[nextSpan->_pagesize].Erase(nextSpan);
		delete nextSpan;
	}

	_spanLists[span->_pagesize].PushFront(span);
}

Span* PageCache::GetIdToSpan(PAGE_ID id)
{
	//std::map<PAGE_ID, Span*>::iterator it = _idSpanMap.find(id);
	auto it = _idSpanMap.find(id);
	if (it != _idSpanMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

