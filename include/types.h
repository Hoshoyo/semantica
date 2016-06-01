#pragma once
#include <vector>

enum Types
{
	INT,
	BOOL,
	UNIT
};

struct PrimitiveType
{
	Types t;
	bool isReference;
	PrimitiveType(Types t, bool isRef)
		: t(t), isReference(isRef)
	{
	}
};

struct Type
{
	std::vector<PrimitiveType> type;
};