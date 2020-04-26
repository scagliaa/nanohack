#pragma once
#include "main.h"

template<typename Fn>
inline Fn GetVFunc(void* v, int i)
{
	return reinterpret_cast<Fn>((*(void***)v)[i]);
}

template<typename T>
inline T* MakePtr(void* base, unsigned long offset)
{
	return (T*)((char*)base + offset);
}

template<typename T>
inline T ReadPtr(void* base, unsigned long offset)
{
	return *(T*)((char*)base + offset);
}

template<typename T>
inline void WritePtr(void* base, unsigned long offset, T value)
{
	*(T*)((char*)base + offset) = value;
}

enum
{
	HITGROUP_GENERIC	= 0,
	HITGROUP_HEAD		= 1,
	HITGROUP_CHEST		= 2,
	HITGROUP_STOMACH	= 3,
	HITGROUP_LEFTARM	= 4,
	HITGROUP_RIGHTARM	= 5,
	HITGROUP_LEFTLEG	= 6,
	HITGROUP_RIGHTLEG	= 7,
	HITGROUP_GEAR		= 10,
};

enum
{
	TEAM_UNASSIGNED,
	TEAM_SPECTATOR,
	TEAM_RED,
	TEAM_BLU,
	TEAM_BOTH
};

enum
{
	MASK_SOLID	= 0x0200400B,
	MASK_SHOT	= 0x46004003
};

enum
{
	Class_None,
	Class_Scout,
	Class_Sniper,
	Class_Soldier,
	Class_Demoman,
	Class_Medic,
	Class_Heavy,
	Class_Pyro,
	Class_Spy,
	Class_Engineer,
};

enum
{
	PlayerCond_None = 0,
	PlayerCond_Slowed			= (1 << 0),
	PlayerCond_Zoomed			= (1 << 1),
	PlayerCond_Disguising		= (1 << 2),
	PlayerCond_Disguised		= (1 << 3),
	PlayerCond_Cloaked			= (1 << 4),
	PlayerCond_Ubercharged		= (1 << 5),
	PlayerCond_TeleportGlow		= (1 << 6),
	PlayerCond_Taunting			= (1 << 7),
	PlayerCond_UberchargeFade	= (1 << 8),
	PlayerCond_CloakFlicker		= (1 << 9),
	PlayerCond_Teleporting		= (1 << 10),
	PlayerCond_Kritzkreiged		= (1 << 11),
	PlayerCond_DeadRingered		= (1 << 13),
	PlayerCond_Bonked			= (1 << 14),
	PlayerCond_Dazed			= (1 << 15),
	PlayerCond_Buffed			= (1 << 16),
	PlayerCond_Charging			= (1 << 17),
	PlayerCond_DemoBuff			= (1 << 18),
	PlayerCond_CritCola			= (1 << 19),
	PlayerCond_InHealRadius		= (1 << 20),
	PlayerCond_Healing			= (1 << 21),
	PlayerCond_OnFire			= (1 << 22),
	PlayerCond_Overhealed		= (1 << 23),
	PlayerCond_Jarated			= (1 << 24),
	PlayerCond_Bleeding			= (1 << 25),
	PlayerCond_DefenseBuffed	= (1 << 26),
	PlayerCond_Milked			= (1 << 27),
	PlayerCond_MegaHeal			= (1 << 28),
	PlayerCond_RegenBuffed		= (1 << 29),
	PlayerCond_MarkedForDeath	= (1 << 30),
};

enum
{
	FL_ONGROUND	= (1 << 0),
	FL_DUCKING	= (1 << 1),
};

enum
{
	IN_ATTACK		= (1 << 0),
	IN_JUMP			= (1 << 1),
	IN_DUCK			= (1 << 2),
	IN_FORWARD		= (1 << 3),
	IN_BACK			= (1 << 4),
	IN_USE			= (1 << 5),
	IN_CANCEL		= (1 << 6),
	IN_LEFT			= (1 << 7),
	IN_RIGHT		= (1 << 8),
	IN_MOVELEFT		= (1 << 9),
	IN_MOVERIGHT	= (1 << 10),
	IN_ATTACK2		= (1 << 11),
	IN_RUN			= (1 << 12),
	IN_RELOAD		= (1 << 13),
	IN_ALT1			= (1 << 14),
	IN_ALT2			= (1 << 15),
	IN_SCORE		= (1 << 16),
	IN_SPEED		= (1 << 17),
	IN_WALK			= (1 << 18),
	IN_ZOOM			= (1 << 19),
	IN_WEAPON1		= (1 << 20),
	IN_WEAPON2		= (1 << 21),
	IN_BULLRUSH		= (1 << 22),
};

#define	DECLFUNCA(I, RET, NAME, A, T, P, OBJ)	inline RET NAME A\
{\
	return GetVFunc<RET (__thiscall*) T>(OBJ, I) P;\
}

struct GlobalVars
{
	float	real_time; // 0
	int		frame_count; // 4
	float	absolute_frame_time; // 8
	float	cur_time; // 12
	float	frame_time; // 16
	int		max_clients; // 20
	int		tick_count; // 24
	float	interval_per_tick; // 28
	float	interpolation_amount; // 32
	int		sim_ticks_this_frame; // 36
	int		network_protocol; // 40
	void*	save_data; // 44
	bool	client; // 48
	int		timestamp_networking_base; // 52
	int		timestamp_randomize_window; // 56
};

struct player_info
{
	char			name[32];
	int				userid;
	char			guid[33];
	unsigned		friendsid;
	char			friendsname[32];
	bool			fakeplayer;
	bool			ishltv;
	unsigned long	customfiles[4];
	unsigned char	filesdownloaded;
};

struct CUserCmd
{
	void*			header;
	unsigned		command_number;
	unsigned		tick_count;
	Vector			viewangles;
	Vector			move;
	unsigned		buttons;
	unsigned char	impulse;
	int				weaponselect;
	int				weaponsubtype;
	unsigned		random_seed;
	short			mousex;
	short			mousey;
	bool			predicted;

	char* mousewheel()
	{
		return MakePtr<char>(this, 0x59);
	}
};

struct WeaponInfo
{
	inline
	int CSS_GetPenetration()
	{
		return ReadPtr<int>(this, 0x884);
	}

	inline
	int CSS_GetDamage()
	{
		return ReadPtr<int>(this, 0x888);
	}

	inline
	float CSS_GetDistance()
	{
		return ReadPtr<float>(this, 0x888);
	}

	inline
	float CSS_GetRangeModifier()
	{
		return ReadPtr<float>(this, 0x890);
	}

	inline
	int CSS_GetBulletsPerShot()
	{
		return ReadPtr<int>(this, 0x894);
	}

	inline
	int CSS_GetAmmoType()
	{
		return ReadPtr<int>(this, 0x6C0);
	}

	inline
	Vector L4D_GetSpread()
	{
		return ReadPtr<Vector>(this, 0xC2C);
	}
};

struct surfacedata
{
	float			friction;
	float			elasticity;
	float			density;
	float			thickness;
	float			dampening;
	float			reflectivity;
	float			hardnessFactor;
	float			roughnessFactor;
	float			roughThreshold;
	float			hardThreshold;
	float			hardVelocityThreshold;
	unsigned short	stepleft;
	unsigned short	stepright;
	unsigned short	impactSoft;
	unsigned short	impactHard;
	unsigned short	scrapeSmooth;
	unsigned short	scrapeRough;
	unsigned short	bulletImpact;
	unsigned short	rolling;
	unsigned short	breakSound;
	unsigned short	strainSound;
	float			maxSpeedFactor;
	float			jumpFactor;
	unsigned short	material;
	unsigned char	climbable;
	unsigned char	pad;
};

struct ray
{
	VectorAligned	m_Start;
	VectorAligned	m_Delta;
	VectorAligned	m_StartOffset;
	VectorAligned	m_Extents;
	//int			reserved;
	bool			m_IsRay;
	bool			m_IsSwept;

	ray(Vector start, Vector end)
	{
		Init(start, end);
	}

	void Init(Vector start, Vector end)
	{
		m_Delta.x		= end.x - start.x;
		m_Delta.y		= end.y - start.y;
		m_Delta.z		= end.z - start.z;
		m_Extents.x		= m_Extents.y = m_Extents.z = 0;
		m_StartOffset.x	= m_StartOffset.y = m_StartOffset.z = 0;
		m_Start.x		= start.x;
		m_Start.y		= start.y;
		m_Start.z		= start.z;

		m_IsRay			= 1;
		m_IsSwept		= m_Delta.LengthSqr() != 0;
	}
};

class CBaseEntity;
struct rt
{
	Vector			startpos;
	Vector			endpos;
	Vector			normal;
	float			distance;
	char			type;
	char			signbits;
	short			unknown;
	float			fraction;
	int				contents;
	unsigned short	dispflags;
	bool			allsolid;
	bool			startsolid;
	float			fractionleftsolid;
	const char*		name;
	short			surfaceprops;
	unsigned short	flags;
	int				hitgroup;
	short			physicsbone;
	CBaseEntity*	ent;
	int				hitbox;
};

struct mstudiobbox
{
	int		bone;
	int		group;
	Vector	bbmin;
	Vector	bbmax;
	int		szhitboxnameindex;
	int		unused[8];
};

struct studiohitboxset
{
	int	sznameindex;
	int	numhitboxes;
	int	hitboxindex;

	inline
	mstudiobbox* hitbox(int i) const
	{
		return (mstudiobbox*)((char*)this + hitboxindex) + i;
	};
};

#define BONE_ARRAY 128

class ClientClass;
class CBaseEntity
{
public:
	DECLFUNCA(2,	ClientClass*,	GetClientClass,	(), (void*), ((void**)this + 2), (void**)this + 2);
	DECLFUNCA(8,	bool,			IsDormant,		(), (void*), ((void**)this + 2), (void**)this + 2);
	DECLFUNCA(9,	int,			EntIndex,		(), (void*), ((void**)this + 2), (void**)this + 2);
	DECLFUNCA(9,	void*,			GetModel,		(), (void*), ((void**)this + 1), (void**)this + 1);

	static studiohitboxset*	hboxset;
	static matrix3x4		matrix[BONE_ARRAY];

	Vector GetAbsOrigin();
	Vector GetAbsAngles();
	
	unsigned long long
		GetSteamID();

	const char*
		GetClassname();

	bool IsSteamFriend();
	bool UpdateBones();

	inline int Hitboxes()
	{
		return hboxset->numhitboxes;
	}

	inline mstudiobbox* GetHitbox(int i)
	{
		return hboxset->hitbox(i);
	}

	inline Vector GetBoxPos(int i)
	{
		return GetBoxPos(GetHitbox(i));
	}

	inline Vector GetBoxPos(mstudiobbox* box)
	{
		Vector mins, maxs;
		VectorTransform(box->bbmin, matrix[box->bone], mins);
		VectorTransform(box->bbmax, matrix[box->bone], maxs);
		
		return (mins + maxs) * 0.5f;
	}

	inline int GetTeam()
	{
		extern int m_iTeamNum;
		return ReadPtr<int>(this, m_iTeamNum);
	}

	
	int GetFlags();
	int GetNextFlags();

	int GetMaxHealth();
	inline int GetHealth()
	{
		extern int m_iHealth;
		return ReadPtr<int>(this, m_iHealth);
	}
	
	inline bool IsAlive()
	{
		extern int m_lifeState;
		return !ReadPtr<bool>(this, m_lifeState);
	}

	inline float GetHealthPercentage()
	{
		return clamp((float)GetHealth() / (float)GetMaxHealth(), 0.0f, 1.0f);
	}

	WeaponInfo*	GetWeaponInfo();
	CBaseEntity* GetActiveWeapon();

	const char*	GetNick();
	const char* GetNickSafe();


	inline Vector GetAbsCenter()
	{
		Vector mins, maxs, up;
		GetBounds(mins, maxs);
		AngleVectors(GetAbsAngles(), 0, 0, &up);

		return GetAbsOrigin() + (up * ((mins.z + maxs.z) * 0.5f));
	}

	inline Vector GetEyeAngles()
	{
		extern int m_angEyeAngles0;
		return ReadPtr<Vector>(this, m_angEyeAngles0);
	}

	inline Vector GetOrigin()
	{
		extern int m_vecOrigin;
		return ReadPtr<Vector>(this, m_vecOrigin);
	}

	inline Vector GetVelocity()
	{
		extern int m_vecVelocity0;
		return ReadPtr<Vector>(this, m_vecVelocity0);
	}

	inline Vector GetShootPos()
	{
		extern int m_vecViewOffset0;
		return GetAbsOrigin() + ReadPtr<Vector>(this, m_vecViewOffset0);
	}

	inline Vector GetViewPunch()
	{
		extern int m_Local, m_vecPunchAngle;
		return ReadPtr<Vector>(this, m_Local + m_vecPunchAngle);
	}


	inline void GetBounds(Vector &mins, Vector &maxs)
	{
		extern int m_Collision, m_vecMins, m_vecMaxs;

		mins = ReadPtr<Vector>(this, m_Collision + m_vecMins);
		maxs = ReadPtr<Vector>(this, m_Collision + m_vecMaxs);
	}
	

	unsigned GetHPColor();
	unsigned GetESPColor();

	inline unsigned GetMDLColor()
	{
		extern int m_clrRender;

		unsigned dw = ReadPtr<unsigned long>(this, m_clrRender);
		return (dw & 0x000000ff) << 24 | (dw & 0x0000ff00) << 8 | (dw & 0x00ff0000) >> 8 | (dw & 0xff000000) >> 24;
	}
	
	inline float GetNextPrimaryFire()
	{
		extern int m_flNextPrimaryAttack;
		return ReadPtr<float>(this, m_flNextPrimaryAttack);
	}

	inline int GetClip1()
	{
		extern int m_iClip1;
		return ReadPtr<int>(this, m_iClip1);
	}

	inline int GetTickBase()
	{
		extern int m_nTickBase;
		return ReadPtr<int>(this, m_nTickBase);
	}
	
	inline char GetWaterLevel()
	{
		extern int m_nWaterLevel;
		return ReadPtr<char>(this, m_nWaterLevel);
	}
	


	bool IsMelee();
	inline bool IsReloading()
	{
		extern int m_flTimeWeaponIdle;
		return ReadPtr<bool>(this, m_flTimeWeaponIdle + 4);
	}

	const char* TF2_GetClass();
	inline int TF2_GetClassNum()
	{
		extern int m_PlayerClass, m_iClass;
		return ReadPtr<int>(this, m_PlayerClass + m_iClass);
	}

	inline int TF2_GetPlayerCond()
	{
		extern int m_Shared, m_nPlayerCond;
		return ReadPtr<int>(this, m_Shared + m_nPlayerCond);
	}

	inline bool TF2_HasCond(int cond)
	{
		return chk(TF2_GetPlayerCond(), cond);
	}

	inline bool TF2_IsUbercharged()
	{
		return TF2_HasCond(PlayerCond_Ubercharged);
	}

	inline bool TF2_IsCloaked()
	{
		return TF2_HasCond(PlayerCond_Cloaked);
	}
	inline bool IsDummyProjectile()
	{
		return GetOrigin() == GetAbsOrigin();
	}

	inline bool HasFlag(int flag)
	{
		return chk(GetFlags(), flag);
	}

	inline bool IsDucked()
	{
		return HasFlag(FL_DUCKING);
	}

	inline bool IsOnGround()
	{
		return HasFlag(FL_ONGROUND);
	}
};

class GameEvent
{
public:
	DECLFUNCA(1, const char*,	GetName,	(),											(void*),							(this),					this);
	DECLFUNCA(8, const char*,	GetString,	(const char* key, const char* default = ""),(void*, const char*, const char*),	(this, key, default),	this);
	DECLFUNCA(6, int,			GetInt,		(const char* key, int	default	= 0),		(void*, const char*, int),			(this, key, default),	this);
};

class NetChannel
{
public:
	DECLFUNCA(9, float,	GetLatency,	(int flow = 0), (void*, int), (this, flow), (this));
	DECLFUNCA(40, void, SendNetMsg, (void* msg), (void*, void*, int, int), (this, msg, 1, 0), (this));

	inline float GetPing()
	{
		return GetLatency() + GetLatency(1);
	}
};


class FilterGeneric
{
	CBaseEntity* hIgnore;

	virtual bool ShouldHitEntity(CBaseEntity* handle, int);
	virtual int	GetTraceType() const;

public:
	FilterGeneric(CBaseEntity* E)
	{
		hIgnore = E;
	}
};




#define	THIS m_vmt
#define	INTERFACE(NAME) class I##NAME : public EInterface { public: I##NAME()

#define	DECLFUNC(I, RET, NAME, A, T, P)	inline RET NAME A\
{\
	return GetVFunc<RET (__thiscall*) T>(THIS, I) P;\
}


class EInterface
{
protected:
	unsigned long** m_vmt;
	VMT*m_hook;
	int	m_size;

public:
	bool Init(unsigned long** base);
	bool Init(const char* dll, const char* name, int ver = 0);
	void HookMethod(int m, void* hook, void* original);
	unsigned long GetMethod(int m);

	inline unsigned long GetBase()
	{
		return (unsigned long)m_vmt;
	}



	EInterface()
	{
	}

	EInterface(unsigned long base)
	{
		if (Init((unsigned long**)base))
		{
		}
	}
};

INTERFACE(Trace)
	{
		if (Init("engine", "EngineTraceClient"))
		{
		}
	}

	DECLFUNC(4, void, TraceRay,
		(const ray &r, int msk, void* flt, rt* tr),
		(void*, const ray&, int, void*, rt*),
		(THIS, r, msk, flt, tr)
	);
};

INTERFACE(PlayerInfoManager)
{
	if (Init("server", "PlayerInfoManager"))
	{
	}
}
	DECLFUNC(1, GlobalVars*, GetGlobalVars, (), (void*), (THIS));
};

INTERFACE(Physics)
	{
		if (Init("vphysics", "VPhysicsSurfaceProps"))
		{

		}
	}

	DECLFUNC(5, surfacedata*, GetSurfaceData,
		(int surfacedataindex),
		(void*, int),
		(THIS, surfacedataindex)
	);
};

INTERFACE(ModelInfo)
	{
		if (Init("engine", "VModelInfoClient"))
		{
		}
	}

	DECLFUNC(3,		const char*,	GetModelName,	(const void* model), (void*, const void*), (THIS, model));
	DECLFUNC(28,	void*,			GetStudiomodel,	(const void* model), (void*, const void*), (THIS, model));
};

INTERFACE(Client)
	{
		if (Init("client", "VClient"))
		{
		}
	}

	DECLFUNC(8, ClientClass*, GetAllClasses, (), (void*), (THIS));
};

INTERFACE(Entities)
	{
		if (Init("client", "VClientEntityList"))
		{
		}
	}

	DECLFUNC(3, CBaseEntity*,	GetClientEntity,		(int entid),			(void*, int),			(THIS, entid));
	DECLFUNC(4, CBaseEntity*,	GetClientEntityHandle,	(unsigned long handle), (void*, unsigned long), (THIS, handle));
	DECLFUNC(6, int,			GetHighestEntityIndex,	(),						(void*),				(THIS));
};

INTERFACE(Engine)
	{
		if (Init("engine", "VEngineClient"))
		{
		}
	}
	
	DECLFUNC(5, void,	GetScreenSize,		(int &w, int &h), (void*, int&, int&), (THIS, w, h))
	DECLFUNC(8, bool,	GetPlayerInfo,		(int entindex, player_info* info), (void*, int, player_info*), (THIS, entindex, info))
	DECLFUNC(12, int,	GetLocalPlayer,		(), (void*), (THIS))
	DECLFUNC(26, bool,	IsInGame,			(), (void*), (THIS))

	DECLFUNC(36,
		matrix4x4&, GetViewMatrix, (), (void*), (THIS))

	DECLFUNC(72, NetChannel*, GetNetChannel, (), (void*), (THIS))
	DECLFUNC(19, void,	GetViewAngles,		(Vector &a), (void*, Vector&), (THIS, a));
	DECLFUNC(20, void,	SetViewAngles,		(Vector &a), (void*, Vector&), (THIS, a));
};

INTERFACE(Panel)
	{
		if (Init("vgui2", "VGUI_Panel"))
		{
		}
	}
	
	DECLFUNC(36, const char*, GetName, (unsigned p), (void*, unsigned), (THIS, p));
	DECLFUNC(17, int, GetChildCount, (unsigned p), (void*, unsigned), (THIS, p));
};

INTERFACE(InputSystem)
	{
		if (Init("inputsystem", "InputSystemVersion")) 
		{
		}
	}
// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/appframework/IAppSystem.h
// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/inputsystem/iinputsystem.h
// thus it is at the 7th index. 
	DECLFUNC(7, void, EnableInput, (bool bEnable), (void*, bool), (THIS, bEnable));
};
INTERFACE(Surface)
	{
		if (Init("vguimatsurface", "VGUI_Surface"))
		{
		}
	}

	DECLFUNC(11,	void, DrawSetColor,		(int r, int g, int b, int a),		(void*, int, int, int, int),	(THIS, r, g, b, a));
	DECLFUNC(12,	void, DrawFilledRect,	(int x0, int y0, int x1, int y1),	(void*, int, int, int, int),	(THIS, x0, y0, x1, y1));
	DECLFUNC(14,	void, DrawOutlinedRect,	(int x0, int y0, int x1, int y1),	(void*, int, int, int, int),	(THIS, x0, y0, x1, y1));
	DECLFUNC(15,	void, DrawLine,			(int x0, int y0, int x1, int y1),	(void*, int, int, int, int),	(THIS, x0, y0, x1, y1));
	
	DECLFUNC(17,	void, DrawSetTextFont,	(unsigned long font),				(void*, unsigned long),			(THIS, font));
	DECLFUNC(19,	void, DrawSetTextColor,	(int r, int g, int b, int a),		(void*, int, int, int, int),	(THIS, r, g, b, a));
	DECLFUNC(20,	void, DrawSetTextPos,	(int x, int y),						(void*, int, int),				(THIS, x, y));
	DECLFUNC(22,	void, DrawPrintText,	(const wchar_t* x, int l),			(void*, const wchar_t*, int, int),	(THIS, x, l, 0));
	DECLFUNC(52, void, SetCursorAlwaysVisible, (bool bEnable),
		(void*, bool),
		(THIS, bEnable)
	);
	DECLFUNC(66,	unsigned long, CreateFont,	(), (void*), (THIS));
	DECLFUNC(67,	void, SetFontGlyphSet,		(unsigned long f, const char* n, int t, int w, int b, int s, int fl, int _min, int _max),
		(void*, unsigned long, const char*, int, int, int, int, int, int, int),
		(THIS, f, n, t, w, b, s, fl, _min, _max)
	);
	//	virtual void SetCursorAlwaysVisible( bool visible ) = 0;
	DECLFUNC(75,	void, GetTextSize,			(unsigned long f, const wchar_t* x, int& w, int& h),
		(void*, unsigned long, const wchar_t*, int&, int&),
		(THIS, f, x, w, h)
	);
	DECLFUNC(61, void, UnlockCursor, (),
		(void*),
		(THIS)
	);
	DECLFUNC(62, void, LockCursor, (),
		(void*),
		(THIS)
	);
};

INTERFACE(Movement)
	{
		if (Init("client", "GameMovement"))
		{
		}
	}

	DECLFUNC(1,	void, ProcessMovement, (CBaseEntity* pl, char* md), (void*, CBaseEntity*, char*), (THIS, pl, md));
};

INTERFACE(Prediction)
	{
		if (Init("client", "VClientPrediction"))
		{
		}
	}
	
	DECLFUNC(17, void, RunCommand,	(CBaseEntity* pl, CUserCmd* cmd, void* mv), (void*, CBaseEntity*, CUserCmd*, void*),		(THIS, pl, cmd, mv));
	DECLFUNC(18, void, SetupMove,	(CBaseEntity* pl, CUserCmd* cmd, char* md), (void*, CBaseEntity*, CUserCmd*, void*, char*), (THIS, pl, cmd, 0, md));
	DECLFUNC(19, void, FinishMove,	(CBaseEntity* pl, CUserCmd* cmd, char* md), (void*, CBaseEntity*, CUserCmd*, char*),		(THIS, pl, cmd, md));
};

class ILuaInterface;

INTERFACE(LuaShared)
	{
		Init("lua_shared", "LUASHARED", 3);
	}

	DECLFUNC(6, ILuaInterface*, GetLuaInterface, (char state), (void*, char), (THIS, state));
};


typedef void* (*CreateInterfaceFn)(const char*, unsigned long);