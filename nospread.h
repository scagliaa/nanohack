#pragma once
#include "main.h"

struct CUserCmd;
class CBaseEntity;

namespace nospread
{
	extern Vector hl2ws;
	extern Vector recoil;
	void FixRecoil(CUserCmd* cmd);
	void HandleCmd(CUserCmd* cmd, CBaseEntity* w);
	void ApplyBulletSpread(CUserCmd* cmd, CBaseEntity* w, Vector &ref, float multiplier);
}