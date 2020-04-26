#pragma once
#include "main.h"

class JMP
{
	void* destination;
	void* origin;
	
	unsigned char org[5];
	unsigned char mod[5];

	unsigned long jmp;
	unsigned long oldp;

public:
	JMP(void* pointer, void* hook);
	~JMP();
	
	inline void SetOrigin(void* t)
	{
		origin = t;
	}

	void* Call(int n = 0, ...);
};