#pragma once

#include <windows.h>
#include <stdio.h>
#define bpaware() register stack *bp asm("ebp")
/**

added some stuff from NH3.0

**/
struct stack
{
	stack* next;
	char* ret;

	template<typename T> inline T& arg(unsigned int i)
	{
		return *(T*)((void**)this + i - 1 + 2);
	}
};

template<typename T>
inline T Min(T x, T y)
{
	return x < y ? x : y;
}

template<typename T>
inline T Max(T x, T y)
{
	return x > y ? x : y;
}

template<typename T>
constexpr T minof()
{
	return (T)1 << (sizeof(T) * 8 - 1);
}

template<typename T>
constexpr T maxof()
{
	return ~((T)1 << (sizeof(T) * 8 - 1));
}

inline float clamp(float v, float mmin, float mmax)
{
	if (v > mmax) return mmax;
	if (v < mmin) return mmin;

	return v;
}

inline int clamp(int v, int mmin, int mmax)
{
	if (v > mmax) return mmax;
	if (v < mmin) return mmin;

	return v;
}

template<typename T> inline T get_BP()
{
	__asm mov eax, ebp // asm("mov eax, ebp");
}

template<typename T> inline T get_SI()
{
	__asm mov eax, esi // asm("mov eax, esi");
}

const char* format(const char* mask, ...);