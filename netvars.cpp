#include "netvars.h"

int m_hActiveWeapon;
int m_iTeamNum;
int m_lifeState;
int m_vecOrigin;
int m_fFlags;
int m_nWaterLevel;
int m_iHealth;
int m_Local;
int m_vecPunchAngle;
int m_vecViewOffset0;
int m_vecVelocity0;
int m_angEyeAngles0;
int m_nTickBase;
int m_flNextPrimaryAttack;
int m_iClip1;
int m_iState;
int m_Shared;
int m_nPlayerCond;
int m_PlayerClass;
int m_iClass;
int m_iWeaponState;
int m_bHealing;
int m_iType;
int m_bTouched;
int m_bHasSapper;
int m_hBuilder;
int m_clrRender;
int m_bReadyToBackstab;
int m_flSimulationTime;
int m_flTimeWeaponIdle;
int m_iPrimaryAmmoType;
int m_vecMins;
int m_vecMaxs;
int m_Collision;
int m_flLastFireTime;
int m_flLaggedMovementValue;

RecvTable* recvTables[8192];

void Netvars::Dump(const char* f)
{
	FILE* o = pipe::OpenFile(f, "w");

	if (!o)
		return;

	Dump(o);
	fclose(o);
}

extern IClient* client;

void Netvars::Dump(_iobuf* o)
{
	const char* lprev = NULL;

	for (int i = 0; i < GetHighestIndex(); i++)
	{
		for (ClientClass* CC = client->GetAllClasses(); CC; CC = CC->m_pNext)
			if (CC->m_pRecvTable == recvTables[i])
			{
				if (lprev != CC->m_pNetworkName)
				{
					fprintf(o, "%s:\n", CC->m_pNetworkName);
					lprev = CC->m_pNetworkName;
				}

				break;
			}
		
		fprintf(o, "\t%s:\n", recvTables[i]->m_pNetTableName);

		for (int x = 0; x < recvTables[i]->m_nProps; x++)
			if (recvTables[i]->m_pProps[x].m_pDataTable)
				fprintf(o, "\t\t0x%04X -> %s : %s\n",
					recvTables[i]->m_pProps[x].m_Offset,
					recvTables[i]->m_pProps[x].m_pVarName,
					recvTables[i]->m_pProps[x].m_pDataTable->m_pNetTableName);
			else
				fprintf(o, "\t\t0x%04X -> %s\n",
					recvTables[i]->m_pProps[x].m_Offset,
					recvTables[i]->m_pProps[x].m_pVarName);

		fputc('\n', o);
		fflush(o);
	}
}

int Netvars::GetHighestIndex()
{
	for (int i = 0; i < (sizeof(recvTables) / 4); i++)
		if (!recvTables[i])
			return i;

	return 0;
}

void Netvars::ScanRecursively(RecvTable* recvTable)
{
	recvTables[GetHighestIndex()] = recvTable;

	for (int i = 0; i < recvTable->m_nProps; i++)
	{
		RecvProp* prop = &recvTable->m_pProps[i];

		if (prop->m_pDataTable)
		{
			bool hasvalue = false;
			for (int x = 0; x < GetHighestIndex(); x++)
				if (recvTables[x] == prop->m_pDataTable)
					hasvalue = true;

			if (!hasvalue)
				ScanRecursively(prop->m_pDataTable);
		}
	}
}

int Netvars::GetNetvar(const char* dt, const char* m)
{
	bool any = dt[0] == '*';

	for (int i = 0; i < GetHighestIndex(); i++)
		if (any || !strcmp(recvTables[i]->m_pNetTableName, dt))
			for (int x = 0; x < recvTables[i]->m_nProps; x++)
				if (!strcmp(recvTables[i]->m_pProps[x].m_pVarName, m))
					return recvTables[i]->m_pProps[x].m_Offset;
	
	util::Message("Failed to get %s -> %s", dt, m);

	return 0;
}

bool Netvars::HookNetvar(const char* dt, const char* m, void* fn)
{
	bool any = dt[0] == '*';

	for (int i = 0; i < GetHighestIndex(); i++)
		if (any || !strcmp(recvTables[i]->m_pNetTableName, dt))
			for (int x = 0; x < recvTables[i]->m_nProps; x++)
				if (!strcmp(recvTables[i]->m_pProps[x].m_pVarName, m))
					recvTables[i]->m_pProps[x].m_ProxyFn = fn;

	return true;
}

#define RESOLVE(DT, VAR)		VAR = GetNetvar(#DT, #VAR);
#define RESOLVE2(DT, VAR, VAR2)	VAR = GetNetvar(DT, VAR2);

void Netvars::Initialize()
{
	memset(recvTables, 0, sizeof(recvTables));

	for (ClientClass* CC = client->GetAllClasses(); CC; CC = CC->m_pNext)
		ScanRecursively(CC->m_pRecvTable);

	RESOLVE(DT_BaseCombatCharacter, m_hActiveWeapon)
	RESOLVE(DT_BaseEntity, m_iTeamNum)
	RESOLVE(DT_BaseEntity, m_vecOrigin)
	RESOLVE(DT_BaseEntity, m_clrRender)
	RESOLVE(DT_BaseEntity, m_flSimulationTime)
	RESOLVE(DT_BaseEntity, m_Collision)
	RESOLVE(DT_BasePlayer, m_lifeState)
	RESOLVE(DT_BasePlayer, m_fFlags)
	RESOLVE(DT_BasePlayer, m_iHealth)
	RESOLVE(DT_BaseCombatWeapon, m_iState)
	RESOLVE(DT_LocalPlayerExclusive, m_Local)
	RESOLVE(DT_LocalPlayerExclusive, m_nTickBase)
	RESOLVE(DT_LocalPlayerExclusive, m_flLaggedMovementValue)
	RESOLVE(DT_LocalActiveWeaponData, m_flNextPrimaryAttack)
	RESOLVE(DT_LocalActiveWeaponData, m_flTimeWeaponIdle)
	RESOLVE(DT_LocalWeaponData, m_iPrimaryAmmoType)
	RESOLVE(DT_Local, m_vecPunchAngle)
	RESOLVE(DT_CollisionProperty, m_vecMins)
	RESOLVE(DT_CollisionProperty, m_vecMaxs)

	if (tf2())
	{
		RESOLVE(DT_TFPlayer, m_Shared);
		RESOLVE(DT_TFPlayer, m_PlayerClass);
		RESOLVE(DT_TFPlayerShared, m_nPlayerCond);
		RESOLVE(DT_TFPlayerClassShared, m_iClass);
		RESOLVE(DT_TFProjectile_Pipebomb, m_iType);
		RESOLVE(DT_TFProjectile_Pipebomb, m_bTouched);
		RESOLVE(DT_TFWeaponKnife, m_bReadyToBackstab);
		RESOLVE(DT_WeaponMinigun, m_iWeaponState);
		RESOLVE(DT_WeaponMedigun, m_bHealing);
		RESOLVE(DT_BaseObject, m_bHasSapper);
		RESOLVE(DT_LocalTFWeaponData, m_flLastFireTime);
	}
	
	RESOLVE2("*", m_iClip1, "m_iClip1")
	RESOLVE2("*", m_nWaterLevel, "m_nWaterLevel")
	RESOLVE2("*", m_angEyeAngles0, "m_angEyeAngles[0]")
	RESOLVE2("*", m_vecViewOffset0, "m_vecViewOffset[0]")
	RESOLVE2("DT_LocalPlayerExclusive", m_vecVelocity0, "m_vecVelocity[0]")
}