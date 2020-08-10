#include "aimbot.h"

int	ragebot = 0;


extern ITrace* trace;
extern IEngine* engine;
extern IPhysics* physics;
extern IEntities* ents;
extern IMovement* movement;
extern IPrediction* pd;
extern GlobalVars* globals;

CBaseEntity* aimbot::lp	= 0;

int		aimbot::next_shot	= 0;
int		aimbot::target_id	= 0;
bool	aimbot::bullet		= 0;
bool	aimbot::dummy		= 0;

void (*aimbot::GetMaterialParameters)(int, float&, float&) = 0;
void (__stdcall* aimbot::GetBulletTypeParameters)(int, float&, float&) = 0;

class BulletFilter
{
public:
	CBaseEntity* hTarget, *hSelf;

	virtual bool ShouldHitEntity(CBaseEntity* E, int)
	{
		if (E == hTarget || E == hSelf)
			return 0;

		if (ClientClass* cl = E->GetClientClass())
		{
			const char* cclass = cl->m_pNetworkName;

			if (tf2() && (
						!strcmp(cclass, "CTFReviveMarker") ||
						!strcmp(cclass, "CTFMedigunShield") ||
						!strcmp(cclass, "CFuncRespawnRoomVisualizer")
					)
				)
				return 0;
			
			if (dod() && !strcmp(cclass, "CFuncNewTeamWall"))
				return 0;

			if (MENU_AIMGLASS && !strcmp(cclass, "CBreakableSurface"))
				return 0;
		}

		return 1;
	}

	virtual int	GetTraceType() const
	{
		return 0;
	}
};

int aimbot::GetAimBone(CBaseEntity* e)
{
	int bone = 0;
	for (int i = 0; i < e->Hitboxes(); i++)
	{
		mstudiobbox* box = e->GetHitbox(i);

		if (!box)
			continue;

		if (box->group != (MENU_AIMBSPOT == 0 ? HITGROUP_HEAD : (MENU_AIMBSPOT == 1 ? HITGROUP_CHEST : HITGROUP_STOMACH)))
			continue;

		bone = i;
	}

	return bone;
}
int real_flags = 0, next_flags = 0;

void aimbot::PerformPrediction(CUserCmd* cmd)
{
	nospread::recoil = lp->GetViewPunch();
	static auto o_predcmd = 0;
	
	if (!o_predcmd)
		o_predcmd = *(int*)(util::FindPattern(pd->GetMethod(17), 0x64, "\x89?????\xE8") + 2);

	WritePtr<CUserCmd*>(lp, o_predcmd, cmd);

	char md[0xA4];
	
	real_flags = ReadPtr<int>(lp, m_fFlags);

	pd->SetupMove(lp, cmd, md);
	movement->ProcessMovement(lp, md);
	pd->FinishMove(lp, cmd, md);

	next_flags = ReadPtr<int>(lp, m_fFlags);

	WritePtr<CUserCmd*>(lp, o_predcmd, 0);
	
	GetVFunc<void (__thiscall*)(void*)>(lp, 9)(lp);
}

int CBaseEntity::GetFlags()
{
	return real_flags;
}

int CBaseEntity::GetNextFlags()
{
	return next_flags;
}

bool aimbot::Ignore(CBaseEntity* e)
{
	if (MENU_RAGEBOTE && e->EntIndex() == ragebot)
		return 0;

	int team = e->GetTeam();

	switch (MENU_AIM4TEAM)
	{
	case 1:
		return team == LocalPlayer()->GetTeam();

	case 2:
		return team != LocalPlayer()->GetTeam();

	case 3:
		return 1;
	}

	return 0;
}

inline bool Penetrate(float &distance, Vector &start, Vector &dir, Vector &end, float step, float max)
{
	while (distance <= max)
	{
		distance += step;
		end = start + dir * distance;
		
		rt r;
		trace->TraceRay(ray(end - dir * step * 2.f, end), MASK_SOLID, 0, &r);

		if (r.contents & MASK_SOLID)
		{
			return true;
		}
	}

	return 0;
}

void aimbot::Update()
{
	lp		= LocalPlayer();
	dummy	= true;

	if ((ragebot != 0) && (!ents->GetClientEntity(ragebot) || ragebot > globals->max_clients))
	{
		ragebot = 0;
	}

	if (dod() && !lp->IsOnGround())
	{
		return;
	}

	auto w = lp->GetActiveWeapon();

	if ((w == nullptr) || (w->GetClip1() == 0) || w->IsReloading() || w->IsMelee())
	{
		return;
	}

	bullet	= !(w->GetNextPrimaryFire() > globals->cur_time);
	dummy	= false;
}

BulletFilter* bf;
bool aimbot::BulletTrace(Vector src, Vector &dst, CBaseEntity* e)
{
	bf->hTarget = e;

	ray r(src, dst);
	rt	tr;

	trace->TraceRay(r, MASK_SHOT, bf, &tr);

	if (tr.fraction == 1.f)
		return 1;

	if (MENU_AUTOWALL)
	{
		if (css())
		{
			WeaponInfo* w = lp->GetActiveWeapon()->GetWeaponInfo();

			int penetration = w->CSS_GetPenetration();
			float distance	= w->CSS_GetDistance();
			float range		= w->CSS_GetRangeModifier();
			
			float mindamage	= (float)w->CSS_GetDamage() * 0.2f;

			float current_distance = 0.f, exit_distance = 0.f;

			float damage_mul		= 5.f;
			float penetration_mul	= 1.f;

			Vector direction = NormalizeVector(dst - src);

			rt wall;


			float p_distance, p_power;
			GetBulletTypeParameters(w->CSS_GetAmmoType(), p_power, p_distance);

			for (float current_damage = (float)w->CSS_GetDamage(); current_damage > mindamage;)
			{
				trace->TraceRay(ray(src, dst), MASK_SHOT, bf, &wall);

				if (wall.fraction == 1.f)
					return 1;



				surfacedata* sdata = physics->GetSurfaceData(wall.surfaceprops);

				int material	= sdata->material;
				bool grate		= wall.contents & 8;

				GetMaterialParameters(material, penetration_mul, damage_mul);

				if (grate)
				{
					penetration_mul	= 1.0f;
					damage_mul		= 0.99f;
				}

				current_distance += wall.fraction * distance;
				current_damage	 *= pow(range, (current_distance / 500.f));

				if (current_distance > p_distance && penetration > 0)
					penetration = 0;

				if (penetration < 0 || penetration == 0 && !grate)
					break;

				Vector p_end;

				if (!Penetrate(exit_distance, wall.endpos, direction, p_end, 24.f, 128.f))
					break;

				ray exit(p_end, wall.endpos);
				rt	w_exit;

				trace->TraceRay(exit, MASK_SHOT, 0, &w_exit);

				if (w_exit.ent != wall.ent && w_exit.ent)
				{
					FilterGeneric skip1ent(w_exit.ent);
					trace->TraceRay(exit, MASK_SHOT, &skip1ent, &w_exit);
				}

				sdata = physics->GetSurfaceData(w_exit.surfaceprops);
				grate = grate && (w_exit.contents & 8);

				int e_material = sdata->material;
				if (material == e_material && (e_material == 0x57 || e_material == 0x4D))
					penetration_mul *= 2.f;

				float trace_len = w_exit.endpos.DistTo(wall.endpos);

				if (trace_len > (p_power * penetration_mul))
					break;
			
				src = w_exit.endpos;

				p_power -= trace_len / penetration_mul;

				current_distance += trace_len;
				current_damage *= damage_mul;

				distance = (distance - current_distance) * 0.5f;
				penetration--;
			}
		}
	}

	return 0;
}

float aimbot::Rate(CBaseEntity* me, CBaseEntity* pl, bool npc)
{
	if (!npc && MENU_RAGEBOTE && pl->EntIndex() == ragebot)
		return 0.f;

	float rate = 0.f;

	switch (MENU_PRIORSYS)
	{
	case 0:
		rate = me->GetAbsOrigin().DistTo(pl->GetAbsOrigin());
		break;

	case 1:
		{
			Vector path = (pl->GetAbsOrigin() - me->GetAbsOrigin()).Angle();
			Vector eyes = me->GetEyeAngles();

			NormalizeAngles(eyes);

			rate = path.DistTo(eyes);

			break;
		}

	case 2:
		{
			Vector path = (me->GetAbsOrigin() - pl->GetAbsOrigin()).Angle();
			Vector eyes = pl->GetEyeAngles();

			NormalizeAngles(eyes);

			rate = path.DistTo(eyes);

			break;
		}

	case 3:
		{
			return pl->EntIndex() < next_shot ? 0.f : 1.f;
		}
	}

	if (npc)
	{
		rate *= 3.f;
	}
	else
	{
		if (tf2())
		{
			if (pl->TF2_HasCond(PlayerCond_Cloaked))
				rate *= 50.f;

			if (pl->TF2_HasCond(PlayerCond_DeadRingered))
				rate *= 100.f;

			if (pl->TF2_GetClassNum() == Class_Medic)
			if (ReadPtr<bool>(pl->GetActiveWeapon(), m_bHealing))
				rate *= 0.05f;
		}
	}

	return rate;
}

bool aimbot::DoStateCheck(CBaseEntity* pl)
{
	if (!pl->IsAlive())
		return 0;

	if (Ignore(pl))
		return 0;

	if (tf2())
	{
		if (pl->TF2_IsUbercharged())
			return 0;
		if (pl->TF2_HasCond(PlayerCond_Bonked))
			return 0;
	}

	if (MENU_SPAWPROT == 1 && ALPHA(pl->GetMDLColor()) == 128)
		return 0;

	if (MENU_SPAWPROT == 2 && pl->GetHealth() > 500)
		return 0;

	if (MENU_SPAWPROT == 3 && ALPHA(pl->GetMDLColor()) == 200)
		return 0;
	
	if (!MENU_STEAMBRO && pl->IsSteamFriend())
		return 0;

	return 1;
}

bool aimbot::Think(CUserCmd* cmd)
{
	float best = std::numeric_limits<float>::quiet_NaN();

	if (css())
	{
		if (!GetMaterialParameters)
			GetMaterialParameters = (void (__cdecl*)(int, float&, float&))util::FindPattern("client", "\x55\x8B\xEC\x8B\x45\x08\x83\xC0\xBD");

		if (!GetBulletTypeParameters)
			GetBulletTypeParameters = (void (__stdcall*)(int, float&, float&))util::FindPattern("client",
				"\x55\x8B\xEC\x56\x8B\x75\x08\x68????\x56\xE8????\x83\xC4\x08\x84\xC0"
			);
	}

	if (!bf)
		bf = new BulletFilter();

	target_id = 0;
	bf->hSelf = lp;

	Vector sp = lp->GetShootPos();
	Vector tp = Vector();

	CBaseEntity* w = lp->GetActiveWeapon();

	if (gmod() && MENU_SPAWPROT == 3 && ALPHA(lp->GetMDLColor()) == 200)
		return 0;

	int lteam = lp->GetTeam();
	int maxcl = (MENU_NPCAIMBT ? ents->GetHighestEntityIndex() : globals->max_clients);

	for (int i = 1; i <= maxcl; ++i)
		if (CBaseEntity* pl = ents->GetClientEntity(i))
		{
			if (pl == lp)
				continue;

			if (pl->IsDormant())
				continue;

			if (!pl->GetModel())
				continue;

			bool npc = i > globals->max_clients;
			bool lowp = 1;

			if (npc)
			{
				const char* cclass = pl->GetClientClass()->m_pNetworkName;

				if (gmod())
				{
					RecvTable* p = pl->GetClientClass()->m_pRecvTable->m_pProps[0].m_pDataTable;

					if ((strcmp(cclass, "CAI_BaseNPC") && (!p || strcmp(p->m_pNetTableName, "DT_AI_BaseNPC"))) || !pl->IsAlive())
						continue;
				}

				if (tf2())
				{
					if (lowp = (strcmp(cclass, "CObjectSentrygun") ||ReadPtr<bool>(pl, m_bHasSapper)) &&(strcmp(cclass, "CTFGrenadePipebombProjectile") ||!ReadPtr<int>(pl, m_iType) || !ReadPtr<bool>(pl, m_bTouched) ||!pl->IsDummyProjectile() ||sp.DistTo(lp->GetAbsOrigin()) > 768.f))
						continue;
					if (pl->GetTeam() == lteam)
						continue;
				}
			}
			else if (!DoStateCheck(pl))
				continue;

			if (!pl->UpdateBones())
				continue;
			
			float rate = Rate(lp, pl, npc);
			if (!lowp)
				rate *= 0.1f;

			if (rate > best)
				continue;

			if (css() && pl->GetOrigin().DistTo(sp) > 8192.f)
				continue;

			int aim = GetAimBone(pl);
			bool doscan = 1;
#define trace if(BulletTrace(sp,box,pl))\
			  {\
				target_id = i;\
				best = rate;\
				tp = box;\
				continue;\
			  }
			if (pl->GetHitbox(aim))
			{
				Vector box = pl->GetBoxPos(aim);
				trace
			}
			

			if (doscan && MENU_BONESCAN)
			{
				int m = pl->Hitboxes();
				for (int j = 0; j < m; ++j)
				{
					if (j == aim)
						continue;
					if (pl->GetHitbox(j))
					{
						Vector box = pl->GetBoxPos(j);
						trace
					} 
				}
			}
		}

	if (target_id > 0)
	{
		CBaseEntity* pl = ents->GetClientEntity(target_id);

		if (!pl)
			return 0;

		if (target_id > globals->max_clients) // TODO: predict non-lag comepnsated stuff
		{
			tp -= pl->GetAbsOrigin();
			tp += pl->GetOrigin() + pl->GetVelocity() * engine->GetNetChannel()->GetPing();
		}

		cmd->viewangles = (tp - sp).Angle();
		NormalizeAngles(cmd->viewangles);

		if (MENU_AUTOSHOT)
			add(cmd->buttons, IN_ATTACK);

		return 1;
	}

	return 0;
}