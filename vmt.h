#pragma once
#include "main.h"

class VMT
{
public:
	unsigned long**	classbase;
	unsigned long*	rpVMT;
	unsigned long*	oldVMT;
	unsigned long	szVMT;

	VMT()
	{
		memset(this, 0, sizeof(VMT));
	}

	VMT(unsigned long** pclassbase)
	{
		classbase	= pclassbase;
		oldVMT		= *pclassbase;
		szVMT		= GetVMTSize(*pclassbase);
		rpVMT		= (unsigned long*)malloc(sizeof(unsigned long) * szVMT);

		memcpy(rpVMT, oldVMT, sizeof(unsigned long) * szVMT);

		*pclassbase = rpVMT;
	}

	void Reinit(unsigned long** pclassbase)
	{
		classbase	= pclassbase;
		oldVMT		= *pclassbase;
		szVMT		= GetVMTSize(*pclassbase);
		rpVMT		= (unsigned long*)realloc(rpVMT, sizeof(unsigned long) * szVMT);
		
		memcpy(rpVMT, oldVMT, sizeof(unsigned long) * szVMT);

		*pclassbase = rpVMT;
	}

	unsigned long GetMethodAddress(unsigned i)
	{
		return oldVMT[i];
	}

	unsigned long* GetOldVMT()
	{
		return oldVMT;
	}

	unsigned long HookMethod(unsigned long f, int i)
	{
		if (rpVMT && oldVMT && i <= (int)szVMT)
		{
			rpVMT[i] = f;
			return oldVMT[i];
		}

		return 0;
	}
	
	unsigned long GetVMTSize(unsigned long* v)
	{
		unsigned long i = 0;
		for (; v[i]; i++);

		return i;
	}
};