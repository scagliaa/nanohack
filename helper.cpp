#include "helper.h"

const char* format(const char* mask, ...)
{
	static char reserve[256] = {0};
	
	va_list v;
	va_start(v, mask);
	vsprintf_s(reserve, mask, v);
	va_end(v);

	return reserve;
}