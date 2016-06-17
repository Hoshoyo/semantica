#include "memory.h"

static int total = 0;
static void* mem[1024 * 1024 * 20] = {};

void* operator new(size_t size)
{
	return malloc(size);
}

int GetTotalMemoryUsed()
{
	return total;
}

void ClearMemory()
{
	for (void* ptr : mem)
	{
		delete ptr;
	}
}

void* operator new(size_t size, const char* file, uint line)
{
	static int index = 0;

	void* block = malloc(size);

	mem[index] = block;
	index++;
	total += size;
	return block;
}

void* operator new[](size_t size)
{
	return malloc(size);
}

void* operator new[](size_t size, const char* file, uint line)
{
	return malloc(size);
}

void operator delete(void* block)
{
	free(block);
}

void operator delete(void* block, const char* file, uint line)
{
	free(block);
}

void operator delete[](void* block)
{
	free(block);
}

void operator delete[](void* block, const char* file, uint line)
{
	free(block);
}


