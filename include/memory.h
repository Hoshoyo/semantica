#pragma once
#include <typeinfo>
#include <vector>
#include "utils.h"

typedef unsigned int uint;

#define honew		new(__FILE__, __LINE__)

void* operator new(size_t size);
void* operator new(size_t size, const char* file, uint line);
void* operator new[](size_t size);
void* operator new[](size_t size, const char* file, uint line);
	
void operator delete(void* block);
void operator delete(void* block, const char* file, uint line);
void operator delete[](void* block);
void operator delete[](void* block, const char* file, uint line);

int GetTotalMemoryUsed();
void ClearMemory();