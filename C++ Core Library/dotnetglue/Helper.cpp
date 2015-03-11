#include <Objbase.h>

//Use COM memory allocator. Allocated memory is used by managed clients, 
//which are responsible for releasing it (via FreeCoTaskMem()).
// Objbase.h cannot be included directly in SecDbAPI_Impl.cpp due to type conflicts.
void* co_task_mem_alloc(int size)
{
	return ::CoTaskMemAlloc(size);
}

void co_task_mem_free(void* ptr)
{
	::CoTaskMemFree(ptr);
}