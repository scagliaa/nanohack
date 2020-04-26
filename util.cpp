#include "util.h"

void util::Message(const char* f, ...)
{
	typedef void (*MsgFn)(const char* fmt, ...);

	static MsgFn Wrg = (MsgFn)IMPORT("tier0", "Warning");
	static MsgFn Msg = (MsgFn)IMPORT("tier0", "Msg");

	char buf[1024] = {0};

	va_list v;
	va_start(v, f);
	vsprintf(buf, f, v);
	va_end(v);

	strcat(buf, "\n");

	Wrg("[NanoHack] ");
	Msg(buf);

	fputs(buf, pipe::log);
	fflush(pipe::log);
}

void util::SendStringCmd(const char* fmt, ...)
{
	typedef void (__thiscall* SendStringCmdFn)(unsigned long, const char*);

	static unsigned long sig = util::FindPattern("engine", "\x68????\xB9????\xE8????\x8D??????\x68");
	static SendStringCmdFn
		SendStringCmd = (SendStringCmdFn)util::CalcAbsAddress(sig + 11);

	char cmd[256];

	va_list v;
	va_start(v, fmt);
	vsprintf(cmd, fmt, v);
	va_end(v);

	SendStringCmd(*(unsigned long*)(sig + 6), cmd);
}

char* util::SafeFormat(char* string)
{
	char* org = string;

	for (; *string; ++string)
		if (*string == '%')
			*string = ' ';

	return org;
}

bool util::IsKeyDown(int keycode)
{
	if (!keycode)
		return 0;

	static HWND Valve001 = FindWindow("Valve001", NULL);

	if (GetActiveWindow() != Valve001)
		return 0;

	return GetAsyncKeyState(keycode) != 0;
}

char* util::MakeFancy(const char* string)
{
	static char fancy[1024];
	char* pu = fancy;
	bool abv  = 1;
	bool trim = 1;
	for (; *string; ++string)
	{
		if (trim && *string == ' ')
			continue;

		if (!abv && *(pu - 1) != ' ' && *string >= 'A' && *string <= 'Z') // capital check
			*(pu++) = ' ';

		trim = 0;
		abv	 = *string >= 'A' && *string <= 'Z' && *string >= 'A' && *string <= 'Z';

		if (*string == '_')
		{
			if (*(pu - 1) != ' ' && pu != fancy)
				*(pu++) = ' ';
		}
		else
		{
			if (*string != ' ' || *(pu - 1) != ' ')
				*(pu++) = *string;
		}
	}

	*pu = 0;

	if (*fancy > 'Z')
	{
		*fancy -= 32;
	}

	return fancy;
}

unsigned long util::FindRefString(const char* dll, const char* string)
{
	char m_push[6] = {0x68};
	*(unsigned long*)(m_push + 1) = FindPattern(dll, string);

	return FindPattern(dll, m_push);
}

unsigned long util::FindPattern(const char* dll, const char* pattern)
{
	return FindPattern((unsigned long)GetModuleHandle(dll), (unsigned long)-1, pattern);
}

unsigned long util::FindPattern(unsigned long address, unsigned long size, const char* pattern)
{
	for (unsigned long i = 0; i < size; ++i, ++address)
	if (DataCompare((const char*)address, pattern))
		return address;

	return NULL;
}

bool util::DataCompare(const char* base, const char* pattern)
{
	for (; *pattern; ++base, ++pattern)
		if (*pattern != '?' && *base != *pattern)
			return 0;

	return *pattern == 0;
}

unsigned long util::FindPatternComplex(unsigned long base, unsigned long size, int n, ...)
{
	va_list v;
	va_start(v, n);

	for (int i = 0; i < n; i++)
	{
		const char* sig = va_arg(v, const char*);

		if (unsigned long ref = FindPattern(base, size, sig))
			return ref + strlen(sig);
	}

	return 0;
}

unsigned long util::CalcAbsAddress(unsigned long address)
{
	return address + *(signed long*)address + 4;
}