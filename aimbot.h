#pragma once
#include "main.h"

namespace aimbot
{
	extern CBaseEntity* lp;
	extern int next_shot;
	extern int target_id;
	extern bool bullet;
	extern bool dummy;

	extern void (*GetMaterialParameters)(int, float&, float&);
	extern void (__stdcall* GetBulletTypeParameters)(int, float&, float&);

	int GetAimBone(CBaseEntity* e);
	float Rate(CBaseEntity* e, CBaseEntity* pl, bool npc);

	bool DoStateCheck(CBaseEntity* pl);
	bool Ignore(CBaseEntity* me);
	bool BulletTrace(Vector src, Vector &dst, CBaseEntity* e);

	void Update();
	void PerformPrediction(CUserCmd* cmd);

	bool Think(CUserCmd* cmd);
}