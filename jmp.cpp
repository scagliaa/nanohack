#include "jmp.h"

#define UNHOOK()	memcpy(destination, org, 5)
#define REHOOK()	memcpy(destination, mod, 5)

JMP::JMP(void* pointer, void* hook)
{
	origin		= 0;
	destination = pointer;

	jmp = 0 - ((DWORD)destination - (DWORD)hook) - 5;
	mod[0] = 0xE9;

	memcpy(org, destination, 5);
	memcpy((void*)((DWORD)mod + 1), &jmp, 4);

	VirtualProtect(destination, 5, PAGE_EXECUTE_READWRITE, &oldp);
	REHOOK();
}

JMP::~JMP()
{
	UNHOOK();
	VirtualProtect(destination, 5, oldp, &oldp);
}

void* JMP::Call(int n, ...)
{
	UNHOOK();
	
	va_list v;
	va_start(v, n);

	v += n * 4 + 4;

	for (int i = 0; i < n; i++)
	{
		void* a = *(void**)((v -= 4) - 4);
		__asm push a
	}
	
	void* m = destination;
	void* t = origin;
	void* r;

	if (t)
		__asm mov ecx, t

	__asm
	{
		call m
		mov r, eax
	}

	va_end(v);
	REHOOK();

	return r;
}