#pragma once

#undef	UNICODE
#define	WIN32_LEAN_AND_MEAN
#define	VC_EXTRALEAN
#define	_CRT_SECURE_NO_WARNINGS

// valve games
#define	css()	(pipe::appid == 240)
#define	tf2()	(pipe::appid == 440)
#define	dod()	(pipe::appid == 300)

// other se games
#define	gmod()	(pipe::appid == 4000)

// engines/series
#define	e_cs()		(dod() || css())
#define	e_hl2()		(tf2() || gmod())
#define	e_orgbx()	(e_cs() || e_hl2())

#define	add(dst, flag)	((dst) |= (flag))
#define	del(dst, flag)	((dst) &=~(flag))
#define	not(dst, flag)	((dst) & ~(flag))
#define	chk(dst, flag)	((dst) &  (flag))

#define	LocalPlayer()	(ents->GetClientEntity(engine->GetLocalPlayer()))
#define	MENU_ITEMS		100

#pragma optimize("s", on)

#pragma warning(disable:4018)
#pragma warning(disable:4244)
#pragma warning(disable:4800)
#pragma warning(disable:4530)
#pragma warning(disable:4996)

#include <io.h>
#include <vector>

#include "helper.h"

#define	IMPORT(DLL, FUNC) GetProcAddress(GetModuleHandle(DLL), FUNC)
#define	DECLVHOOK(RET, NAME, ARGS)\
	typedef RET (__stdcall* NAME##Fn) ARGS;\
	NAME##Fn org##NAME;\
	RET __stdcall hooked_ ## NAME ARGS\

#include "vmt.h"
#include "jmp.h"

#include "md5.h"

#include "util.h"
#include "pipe.h"

#include "netvars.h"
#include "vector.h"
#include "sdk.h"
#include "lua.h"
#include "forms.h"

#include "namestealer.h"
#include "nospread.h"
#include "aimbot.h"
#include "draw.h"
#include "menu.h"
#include "gui.h"

extern int ragebot;
extern int menu[MENU_ITEMS];