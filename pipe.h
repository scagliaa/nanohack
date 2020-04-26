#pragma once
#include "main.h"

namespace pipe
{
	extern int		appid;
	extern _iobuf*	log;

	char*	ModQuery();
	bool	LoadConfig(const char* fn, bool test = 0);
	bool	SaveConfig(const char* fn);
	_iobuf*	OpenFile(const char* fn, const char* m = "wb");
}