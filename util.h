#pragma once
#include "main.h"

namespace util
{
	void	Message(const char* f, ...);

	char*	SafeFormat(char* string);
	char*	MakeFancy(const char* string);

	bool	IsKeyDown(int keycode);
	
	unsigned long	FindRefString(const char* dll, const char* string);
	unsigned long	FindPattern(const char* dll, const char* pattern);
	unsigned long	FindPattern(unsigned long address, unsigned long size, const char* pattern);
	bool			DataCompare(const char* base, const char* data);

	unsigned long	FindPatternComplex(unsigned long base, unsigned long size, int n, ...);

	inline unsigned long FunctionStart(unsigned long p)
	{
		for (; *(unsigned char*)p != 0x55 || *(unsigned short*)(p + 1) != 0xEC8B || *(unsigned char*)(p + 4) != 0xEC; p--);
		return p;
	}

	__declspec(noinline) unsigned long CalcAbsAddress(unsigned long address);



	void SendStringCmd(const char*, ...);
}