#pragma once

#include <windows.h>
#include <stdio.h>



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