#pragma once
#include "main.h"

extern int m_hActiveWeapon;
extern int m_iTeamNum;
extern int m_lifeState;
extern int m_vecOrigin;
extern int m_fFlags;
extern int m_nWaterLevel;
extern int m_iHealth;
extern int m_Local;
extern int m_vecPunchAngle;
extern int m_vecViewOffset0;
extern int m_vecVelocity0;
extern int m_angEyeAngles0;
extern int m_nTickBase;
extern int m_flNextPrimaryAttack;
extern int m_iClip1;
extern int m_iState;
extern int m_Shared;
extern int m_nPlayerCond;
extern int m_PlayerClass;
extern int m_iClass;
extern int m_iWeaponState;
extern int m_bHealing;
extern int m_iType;
extern int m_bTouched;
extern int m_bHasSapper;
extern int m_hBuilder;
extern int m_clrRender;
extern int m_bReadyToBackstab;
extern int m_flSimulationTime;
extern int m_flTimeWeaponIdle;
extern int m_iPrimaryAmmoType;
extern int m_vecMins;
extern int m_vecMaxs;
extern int m_Collision;
extern int m_flLastFireTime;
extern int m_flLaggedMovementValue;

class RecvProp;
class RecvTable;

class NetvarValue
{
public:
	union
	{
		float	m_Float;
		long	m_Int;
		char*	m_pString;
		void*	m_pData;
		float	m_Vector[3];
	};

	int			m_Type;
};

class RecvProxyData
{
public:
	const RecvProp*	m_pRecvProp;
	NetvarValue		m_Value;
	int				m_iElement;
	int				m_ObjectID;
};

class RecvProp
{
public:
	char*		m_pVarName;
	int			m_RecvType;
	int			m_Flags;
	int			m_StringBufferSize;
	bool		m_bInsideArray;
	void*		m_pExtraData;
	RecvProp*	m_pArrayProp;
	void*		m_ArrayLengthProxy;
	void*		m_ProxyFn;
	void*		m_DataTableProxyFn;
	RecvTable*	m_pDataTable;
	int			m_Offset;
	int			m_ElementStride;
	int			m_nElements;
	const char*	m_pParentArrayPropName;
};

class RecvTable
{
public:
	RecvProp*	m_pProps;
	int			m_nProps;
	void*		m_pDecoder;
	char*		m_pNetTableName;
	bool		m_bInitialized;
	bool		m_bInMainList;
};

class ClientClass
{
public:
	char			header[8];
	const char*		m_pNetworkName;
	RecvTable*		m_pRecvTable;
	ClientClass*	m_pNext;
	int				m_ClassID;
};

namespace Netvars
{
	void Initialize();
	void ScanRecursively(RecvTable* recvTable);
	int GetHighestIndex();

	void Dump(_iobuf* o);
	void Dump(const char* o);
	bool HookNetvar(const char* dt, const char* m, void* hook = NULL);
	int GetNetvar(const char* dt, const char* m);
}