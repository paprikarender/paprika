#ifndef PARAMITEM_HPP
#define PARAMITEM_HPP

#include "paramtype.hpp"

namespace paprika {
namespace core {

struct ParamItem
{
	void swap(ParamItem& rhs)
	{
		bool b1 = isOne();
		bool b2 = rhs.isOne();

		std::swap(type, rhs.type);
		std::swap(lookedup, rhs.lookedup);
		std::swap(oneString, rhs.oneString);
		std::swap(ptr, rhs.ptr);

		if (b2)
			ints = &oneInt;

		if (b1)
			rhs.ints = &rhs.oneInt;
	}

	ParamItem& operator=(const ParamItem& rhs)
	{
		ParamItem(rhs).swap(*this);
		return *this;
	}

	ParamItem(const ParamItem& rhs)
	{
		type = rhs.type;
		lookedup = rhs.lookedup;
		oneString = rhs.oneString;
		ints = rhs.isOne() ? &oneInt : rhs.ints;
	}

	ParamItem() { /* Uninitialized! */ }
	ParamItem(ParamType type, int i) : type(type), oneInt(i), lookedup(false)
	{
		ints = &oneInt;
	}

	ParamItem(ParamType type, float f) : type(type), oneFloat(f), lookedup(false)
	{
		floats = &oneFloat;
	}

	ParamItem(ParamType type, const char* s) : type(type), oneString(s), lookedup(false)
	{
		strings = &oneString;
	}

	ParamItem(ParamType type, const void* ptr) : type(type), ptr(ptr), lookedup(false)
	{

	}

	ParamItem(ParamType type, const int* ints) : type(type), ints(ints), lookedup(false)
	{
        
	}

	ParamItem(ParamType type, const float* floats) : type(type), floats(floats), lookedup(false)
	{

	}
	
	ParamItem(ParamType type, const char*const* strings) : type(type), strings(strings), lookedup(false)
	{

	}

	bool isOne() const
	{
		return ints == &oneInt;
	}

public:
	ParamType type;
	mutable bool lookedup;

	union
	{
		const void* ptr;
		const char* data;
		const int* ints;
		const float* floats;
		const char*const* strings;
	};

private:
	union
	{
		int oneInt;
		float oneFloat;
		const char* oneString;
	};
};

}		/* param */
}		/* paprika */
#endif	/* PARAMITEM_HPP */