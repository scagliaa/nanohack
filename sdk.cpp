#include "sdk.h"

#include "util.h"
#include "pipe.h"

bool EInterface::Init(unsigned long** base)
{
	if (m_vmt = base)
	{
		m_hook = new VMT(m_vmt);
		m_size = m_hook->szVMT;
		return 1;
	}
	return 0;
}

bool EInterface::Init(const char* dll, const char* name, int v)
{
	CreateInterfaceFn CreateInterface = (CreateInterfaceFn)IMPORT(dll, "CreateInterface");
	if (!v)
	{
		for (int i = 99; i && !v; i--)
		if (CreateInterface(format("%s%03i", name, i), 0))
			v = i;
	}
	return Init((unsigned long**)CreateInterface(format("%s%03i", name, v), 0));
}

void EInterface::HookMethod(int m, void* hook, void* original)
{
	unsigned long org = m_hook->HookMethod((unsigned long)hook, m);

	if (original)
		*(unsigned long*)original = org;
}
unsigned long EInterface::GetMethod(int m)
{
	return m_hook->GetMethodAddress(m);
}
unsigned long long CBaseEntity::GetSteamID()
{
	extern IEngine* engine;
	player_info info;
	engine->GetPlayerInfo(EntIndex(), &info);
	return 76561197960265728ull + info.friendsid;
}
bool CBaseEntity::IsSteamFriend()
{
	static unsigned pipe = ((unsigned (*)())IMPORT("steamclient", "Steam_CreateSteamPipe"))();
	static unsigned user = ((unsigned (*)(int))IMPORT("steamclient", "Steam_ConnectToGlobalUser"))(pipe);

	/*
	static void* cl = ((CreateInterfaceFn)IMPORT("steamclient", "CreateInterface"))("SteamClient007", 0);
	static void* fl = GetVFunc<void* (__thiscall*)(void*, unsigned, unsigned, const char*)>(cl, 8)(cl, user, pipe, "SteamFriends003");

	static int e_fs[128];
	
	int uid = (EntIndex() ^ GetTickBase()) & 0x7FFFFFFF;
	int idx = EntIndex() % 128;

	if ((e_fs[idx] & 0x7FFFFFFF) != uid)
	{
		e_fs[idx] = uid;

		if (GetVFunc<char (__thiscall*)(void*, unsigned long long, int)>(fl, 5)(fl, GetSteamID(), 4) == 3)
			e_fs[idx] |= 0x80000000;
	}
	
	if (e_fs[idx] & 0x80000000)
		return 1;
	*/

	return 0;
}
const char* CBaseEntity::GetClassname()
{
	typedef const char* (__thiscall* GetClassnameFn)(void*);
	static GetClassnameFn f = (GetClassnameFn)util::CalcAbsAddress(util::FindPattern("client", "\xE8????\x50\x68????\xFF\x15????\x83\xC4\x0C\x5F\x5E\x8B\xE5") + 1);
	return f(this);
}
WeaponInfo* CBaseEntity::GetWeaponInfo()
{
	if (css())
	{
		typedef unsigned short (*LookupWeaponInfoSlotFn)(const char*);
		typedef WeaponInfo* (*GetFileWeaponInfoFromHandleFn)(unsigned short);

		static LookupWeaponInfoSlotFn LookupWeaponInfoSlot =
			(LookupWeaponInfoSlotFn)util::FindPattern("client", "\x55\x8B\xEC\x8B\x45\x08\x83\xEC\x08\x85\xC0\x74\x18");

		static GetFileWeaponInfoFromHandleFn GetFileWeaponInfoFromHandle =
			(GetFileWeaponInfoFromHandleFn)util::FindPattern("client", "\x55\x8B\xEC\x66\x8B\x45\x08\x66\x3B\x05");

		return GetFileWeaponInfoFromHandle(LookupWeaponInfoSlot(GetClassname()));
	}

	return 0;
}
studiohitboxset* CBaseEntity::hboxset = 0;
matrix3x4 CBaseEntity::matrix[BONE_ARRAY];
bool CBaseEntity::UpdateBones()
{
	extern IModelInfo* mdlinfo;

	void* mdl = GetModel(), *stdhdr = 0;

	if (!mdl)
		return 0;

	stdhdr = mdlinfo->GetStudiomodel(mdl);

	if (!stdhdr)
		return 0;

	typedef bool (__thiscall* SetupBonesFn)(void*, matrix3x4*, int, int, float);
	static SetupBonesFn SetupBones =(SetupBonesFn)util::FunctionStart(util::FindRefString("client", "*** ERROR: Bone access not allowed (entity %i:%s)"));
	if (SetupBones((void**)this + 1, matrix, BONE_ARRAY, 0x100, ReadPtr<float>(this, m_flSimulationTime)))
		if (hboxset = MakePtr<studiohitboxset>(stdhdr, ReadPtr<int>(stdhdr, 0xB0)))
			return 1;

	return 0;
}

CBaseEntity* CBaseEntity::GetActiveWeapon()
{
	extern IEntities* ents;
	extern IEngine* engine;

	return ents->GetClientEntityHandle(ReadPtr<unsigned long>(this, m_hActiveWeapon));
}

Vector CBaseEntity::GetAbsOrigin()
{
	static unsigned long m_vecAbsOrigin = *(unsigned long*)(util::FindPattern((unsigned long)GetVFunc<void*>(this, 9), 0x20, "\xE8????\x8D\x86?????\xC3") + 7); 
	// E8 ? ? ? ? 8D 86 ? ? ? ? ? C3
	//util::Message("m_vecAbsOrigin + %X", m_vecAbsOrigin);
	// m_vecAbsOrigin = m_vecOrigin - 0xD8 // get rekt

	return ReadPtr<Vector>(this, m_vecAbsOrigin);
}

Vector CBaseEntity::GetAbsAngles()
{
	static unsigned long m_vecAbsAngles = *(unsigned long*)(util::FindPattern((unsigned long)GetVFunc<void*>(this, 10), 0x20, "\xE8????\x8D\x86?????\xC3") + 7);
	//util::Message("m_vecAbsAngles + %X", m_vecAbsAngles);
	// m_vecAbsAngles = m_vecOrigin - 0xD8 + 12 // get rekt

	return ReadPtr<Vector>(this, m_vecAbsAngles);
}

/**

?? there's literally a vfunc.
**/
/*int CBaseEntity::GetMaxHealth()
{
	if (tf2())
	{
		switch (TF2_GetClassNum())
		{
		case Class_Scout:
			return 125;

		case Class_Soldier:
			return 200;

		case Class_Pyro:
			return 175;

		case Class_Demoman:
			return 175;

		case Class_Heavy:
			return 300;

		case Class_Engineer:
			return 125;

		case Class_Medic:
			return 150;

		case Class_Sniper:
			return 125;

		case Class_Spy:
			return 125;
		}
	}

	return 100;
}*/

unsigned CBaseEntity::GetHPColor()
{
	float p = GetHealthPercentage();

	if (p < 0.33f)
		return CC_RED;

	if (p < 0.66f)
		return CC_YELLOW;

	return CC_GREEN;
}

unsigned CBaseEntity::GetESPColor()
{
	unsigned retc = CC_LIGHTYELLOW;

	extern IEntities* ents;
	extern IEngine* engine;

	if (gmod())
		retc = aimbot::Ignore(this) ? CC_LIGHTBLUE : CC_LIGHTRED;
	else
	{
		int team = GetTeam();
		
		if (tf2() && team != LocalPlayer()->GetTeam() && TF2_IsCloaked())
			return CC_WHITE;

		if (team == TEAM_RED)
		{
			retc = CC_LIGHTRED;

			if (dod()) retc = CC_LIGHTGREEN;
		}

		if (team == TEAM_BLU)
		{
			retc = CC_LIGHTBLUE;
			
			if (dod()) retc = CC_LIGHTRED;
		}
	}

	if (ragebot == EntIndex())
		retc = CC_PURPLE;

	if (IsSteamFriend())
		retc = COLOR((RED(retc) > 127) ? 255 : 0, (GREEN(retc) > 127) ? 255 : 0, (BLUE(retc) > 127) ? 255 : 0);

	return retc;
}

const char* CBaseEntity::GetNick()
{
	extern IEngine* engine;
	static player_info info;

	if (!engine->GetPlayerInfo(EntIndex(), &info))
		return 0;

	return info.name;
}

const char* CBaseEntity::GetNickSafe()
{
	extern IEngine* engine;
	static player_info info;

	if (!engine->GetPlayerInfo(EntIndex(), &info))
		return 0;

	util::SafeFormat(info.name);

	return info.name;
}

bool CBaseEntity::IsMelee()
{
	int ammo = ReadPtr<int>(this, m_iPrimaryAmmoType);

	return
		(ammo == -1 ||
			(tf2() && (ammo == 4)) ||
			(
				(css() && (ammo == 11)) ||
				(css() && (ammo == 12)) ||
				(css() && (ammo == 13))
			)
		);
}

const char* CBaseEntity::TF2_GetClass()
{
	static char* num2alias[] =
	{
		"",
		"Scout",
		"Sniper",
		"Soldier",
		"Demoman",
		"Medic",
		"Heavy",
		"Pyro",
		"Spy",
		"Engineer",
		""
	};
	return num2alias[clamp(TF2_GetClassNum(), 0, sizeof(num2alias) / sizeof(char*) - 1)];
}
bool FilterGeneric::ShouldHitEntity(CBaseEntity* handle, int)
{
	return handle != hIgnore;
}

int FilterGeneric::GetTraceType() const
{
	return 0;
}