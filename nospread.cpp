#include "nospread.h"

Vector
	nospread::hl2ws = Vector(),
	nospread::recoil = Vector();

extern IEntities*	ents;
extern IEngine*		engine;
extern IMovement*	movement;

extern GlobalVars*	globals;

typedef float (__thiscall* GetSpreadFn)(void*);

JMP* jmpFireBullets	= 0;

extern IPrediction* pd;

bool prefire = 0;
void __fastcall hooked_FireBullets(CBaseEntity* ecx, void*, char* fb)
{
	if (ecx == LocalPlayer() && prefire)
	{
		nospread::hl2ws =
			*(Vector*)(fb + 0x1C);

		return;
	}

	jmpFireBullets->SetOrigin(ecx);
	jmpFireBullets->Call(1, fb);
}

void nospread::ApplyBulletSpread(CUserCmd* cmd, CBaseEntity* w, Vector &ref, float m)
{
	static void (*RandomSeed)(unsigned) = (void (__cdecl*)(unsigned))IMPORT("vstdlib", "RandomSeed");
	static float (*RandomFloat)(float, float) = (float (__cdecl*)(float, float))IMPORT("vstdlib", "RandomFloat");
#define css_spread 371
#define tf2_spread 1504
#define hl2dm_spread 267
	cmd->random_seed = MD5_PseudoRandom(cmd->command_number) & 255;

	if (css()) // || csgo())
	{

		void (__thiscall* UpdateAccuracyPenalty)(void*)	= GetVFunc<void (__thiscall*)(void*)>(w, css_spread + 2);
		
		GetSpreadFn GetWeaponSpread	= GetVFunc<GetSpreadFn>(w, css_spread);
		GetSpreadFn GetWeaponCone	= GetVFunc<GetSpreadFn>(w, css_spread + 1);

		Vector forward, right, up;

		RandomSeed(cmd->random_seed + 1);

		UpdateAccuracyPenalty(w);
		
		float rand0 = RandomFloat(0.f, M_PI * 2.f);
		float rand1 = RandomFloat(0.f, GetWeaponSpread(w));
		float rand2 = RandomFloat(0.f, M_PI * 2.f);
		float rand3 = RandomFloat(0.f, GetWeaponCone(w));

		Vector shake = Vector((cosf(rand0) * rand1) + (cosf(rand2) * rand3), (sinf(rand0) * rand1) + (sinf(rand2) * rand3), 0.f);

		NormalizeAngles(cmd->viewangles);
		AngleVectors(cmd->viewangles, &forward, &right, &up);
		
		Vector new_va = forward + (right * shake.x * m) + (up * shake.y * m);
		new_va.NormalizeInPlace();

		VectorAngles(new_va, ref);
		NormalizeAngles(ref);
	}
	else
	{
		Vector shake = Vector();

		if (gmod())
		{
			shake = hl2ws;
		}
		else
		if (tf2())
		{
			if ((globals->cur_time - ReadPtr<float>(w, m_flLastFireTime)) > 2.0f)
				shake.Zero();
			else
			{
				GetSpreadFn GetBulletSpread = GetVFunc<GetSpreadFn>(w, tf2_spread);
				float f = GetBulletSpread(w);
				shake.x = f;
				shake.y = f;
			}
		}
		else
		if (dod())
		{
			float (__thiscall* GetBulletSpread)(void*, float) = GetVFunc<float (__thiscall*)(void*, float)>(w, 368);
			float f = GetBulletSpread(w, LocalPlayer()->GetVelocity().Length2D());

			shake.x = f;
			shake.y = f;
		}

		if (cmd->random_seed == 39)
			cmd->random_seed += 2;

		RandomSeed(cmd->random_seed);

		float x = RandomFloat(-0.5f, 0.5f) + RandomFloat(-0.5f, 0.5f);
		float y = RandomFloat(-0.5f, 0.5f) + RandomFloat(-0.5f, 0.5f);

		Vector forward, right, up;
		
		NormalizeAngles(cmd->viewangles);
		AngleVectors(cmd->viewangles, &forward, &right, &up);

		Vector new_va = forward + (right * shake.x * x * m) + (up * shake.y * y * m);
		new_va.NormalizeInPlace();

		VectorAngles(new_va, ref);
		NormalizeAngles(ref);
	}
}

void nospread::HandleCmd(CUserCmd* cmd, CBaseEntity* w)
{
	CBaseEntity* lp = LocalPlayer();

	if (tf2())
	{
		static unsigned fakeseed = 0, oldindex = 0;

		if (oldindex != lp->EntIndex())
		{
			oldindex = lp->EntIndex();
			fakeseed = cmd->command_number;
		}

		if (w && !strcmp(w->GetClientClass()->m_pNetworkName, "CTFMinigun") && chk(cmd->buttons, IN_ATTACK))
		{
			static char rseed = MD5_PseudoRandom(188) & 255;

			if (MENU_SMACSEED)
			{
				fakeseed++;
			}
			else 
				for (++fakeseed; (MD5_PseudoRandom(fakeseed) & 255) != rseed; ++fakeseed);
		}
		else fakeseed++;
		cmd->command_number = fakeseed;
	}

	if (aimbot::dummy)
		return;

	if (MENU_NOSPREAD && chk(cmd->buttons, IN_ATTACK))
	{
		if (aimbot::bullet && gmod())
		{
			static CBaseEntity* lw = 0;
			if (lw != w)
			{
				lw = w;
				hl2ws.Zero();
			}

			prefire = 1;
		//	pd->RunCommand(LocalPlayer(), cmd, 0); // find better way to do so
		}

		ApplyBulletSpread(cmd, w, cmd->viewangles, -1.0f);

		if (prefire)
			prefire = 0;
	}
}

void nospread::FixRecoil(CUserCmd* cmd)
{
	Vector p = recoil;

	float normal = p.Normalize();

	normal = max(normal - (10.f + normal * 0.5f) * globals->interval_per_tick, 0.f);

	p *= normal;
	p.z = 0.f;

	if (css())
		p *= 2.f;

	cmd->viewangles -= p;
}