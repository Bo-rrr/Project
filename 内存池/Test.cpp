#include "ThreadCache.h"

#include <vector>

//void UnitThreadCache()
//{
//	ThreadCache tc;
//	vector<void*> v;
//	for (size_t i = 0; i < 21; ++i)
//	{
//		v.push_back(tc.Allocte(7));
//	}
//
//	for (size_t i = 0; i < v.size(); ++i)
//	{
//		printf("[%d]->%p\n", i, v[i]);
//	}
//
//	for (auto ptr : v)
//	{
//		tc.Deallocte(ptr, 7);
//	}
//}

void UnitTestSizeClass()
{
	// 控制在[1%，10%]左右的内碎片浪费
	// [1,128] 8byte对齐 freelist[0,16)
	// [129,1024] 16byte对齐 freelist[16,72)
	// [1025,8*1024] 128byte对齐 freelist[72,128)
	// [8*1024+1,64*1024] 1024byte对齐 freelist[128,184)
	cout << SizeClass::RoundUp(1) << endl;
	cout << SizeClass::RoundUp(127) << endl;
	cout << endl;

	cout << SizeClass::RoundUp(129) << endl;
	cout << SizeClass::RoundUp(1023) << endl;
	cout << endl;

	cout << SizeClass::RoundUp(1025) << endl;
	cout << SizeClass::RoundUp(8*1024-1) << endl;
	cout << endl;

	cout << SizeClass::RoundUp(8 * 1024+1) << endl;
	cout << SizeClass::RoundUp(64 * 1024-1) << endl;
	cout << endl << endl;

	cout << SizeClass::ListIndex(1) << endl;
	cout << SizeClass::ListIndex(128) << endl;
	cout << endl;

	cout << SizeClass::ListIndex(129) << endl;
	cout << SizeClass::ListIndex(1023) << endl;
	cout << endl;

	cout << SizeClass::ListIndex(1025) << endl;
	cout << SizeClass::ListIndex(8 * 1024 - 1) << endl;
	cout << endl;

	cout << SizeClass::ListIndex(8 * 1024 + 1) << endl;
	cout << SizeClass::ListIndex(64 * 1024 - 1) << endl;
	cout << endl;
}

void UnitTestSystemAlloc()
{
	void* ptr = SystemAlloc(1);
	PAGE_ID id = (PAGE_ID)ptr >> PAGE_SHIFT;
	void* ptrshift = (void*)(id << PAGE_SHIFT);

	char* obj1 = (char*)ptr;
	char* obj2 = (char*)ptr + 8;
	char* obj3 = (char*)ptr + 16;
	PAGE_ID id1 = (PAGE_ID)obj1 >> PAGE_SHIFT;
	PAGE_ID id2 = (PAGE_ID)obj2 >> PAGE_SHIFT;
	PAGE_ID id3 = (PAGE_ID)obj3 >> PAGE_SHIFT;
}

void UnitThreadCache()
{
	ThreadCache tc;
	std::vector<void*> v;
	size_t size = 7;
	for (size_t i = 0; i < SizeClass::NumMoveSize(size); ++i)
	{
		v.push_back(tc.Allocte(size));
	}

	v.push_back(tc.Allocte(size));

	for (size_t i = 0; i < v.size(); ++i)
	{
		printf("[%d]->%p\n", i, v[i]);
	}

	for (auto ptr : v)
	{
		tc.Deallocte(ptr, 7);
	}

	v.clear();

	v.push_back(tc.Allocte(size));
}

#include "ConcurrentMalloc.h"

void func1(size_t n)
{
	std::vector<void*> v;
	size_t size = 7;
	for (size_t i = 0; i < SizeClass::NumMoveSize(size)+1; ++i)
	{
		v.push_back(ConcurrentMalloc(size));
	}

	for (size_t i = 0; i < v.size(); ++i)
	{
		//printf("[%d]->%p\n", i, v[i]);
	}

	for (auto ptr : v)
	{
		ConcurrentFree(ptr);
	}

	v.clear();
}

void func2(size_t n)
{
	std::vector<void*> v;
	size_t size = 7;
	for (size_t i = 0; i < SizeClass::NumMoveSize(size) + 1; ++i)
	{
		v.push_back(ConcurrentMalloc(size));
	}

	for (size_t i = 0; i < v.size(); ++i)
	{
	//	printf("[%d]->%p\n", i, v[i]);
	}

	for (auto ptr : v)
	{
		ConcurrentFree(ptr);
	}

	v.clear();
}

class Singleton
{
public:
	static Singleton& GetInstance()
	{
		static Singleton inst;
		return inst;
	}
private:
	Singleton()
	{}

	//Singleton(const Singleton&) = delete;
	Singleton(const Singleton&);
};

int main()
{
	Singleton::GetInstance();

	//Singleton s2;
	//Singleton s3;

	//UnitThreadCache();
	//UnitTestSizeClass();
	//UnitTestSystemAlloc();
	//UnitThreadCache();
	std::thread t1(func1, 100);
	std::thread t2(func2, 100);

	cout << "主线程等待" << endl;
	t1.join();
	t2.join();

	/*void* ptr1 = ConcurrentMalloc(1<<PAGE_SHIFT);
	void* ptr2 = ConcurrentMalloc(65 << PAGE_SHIFT);
	void* ptr3 = ConcurrentMalloc(129 << PAGE_SHIFT);

	ConcurrentFree(ptr1);
	ConcurrentFree(ptr2);
	ConcurrentFree(ptr3);*/

	return 0;
}