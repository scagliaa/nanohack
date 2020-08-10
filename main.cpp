#include "main.h"

EInterface*	input	= 0;

IPanel*		panel	= 0;
ITrace*		trace	= 0;
IClient*	client	= 0;
IEngine*	engine	= 0;
IPhysics*	physics	= 0;
ISurface*	surface	= 0;
IEntities*	ents	= 0;
IMovement*	movement= 0;
IModelInfo*	mdlinfo	= 0;
ILuaShared* glua	= 0;
GlobalVars*	globals	= 0;
IInputSystem* isystem = 0;
IPlayerInfoManager* iplayer = 0;

// IGameEventManager*	evmgr	= 0;
IPrediction*pd		= 0;

auto fix_n_tick_base(const RecvProxyData * data, CBaseEntity * pl, int * out) -> void
{
	if (pl == LocalPlayer())
	{
		static int stuck = 0;
		const int update = data->m_Value.m_Int;

		*out	= (update == stuck) ? pl->GetTickBase() + 1 : update;
		stuck	= update;
	}
	else
		*out = data->m_Value.m_Int;
}

auto send_packet = false;

void create_move(CUserCmd* cmd)
{
	send_packet = true;

	auto me	= LocalPlayer();
	auto w	= me->GetActiveWeapon();

	const auto qrv = cmd->viewangles;
	
	globals->cur_time = static_cast<float>(me->GetTickBase()) * ReadPtr<float>(me, m_flLaggedMovementValue) * globals->interval_per_tick;

	auto start_aimbot_processing = [&](CUserCmd* pcmd)
	{
		aimbot::Update();
		aimbot::PerformPrediction(pcmd);
	};

	start_aimbot_processing(cmd);

	auto aim = false;
	if (MENU_AIMBOTEN && (!MENU_AIMBOTKB || util::IsKeyDown(MENU_AIMBOTKB)) && !aimbot::dummy)
	{
		aim = aimbot::Think(cmd);
	}

	if (!MENU_FAKEVIEW && aim)
	{
		engine->SetViewAngles(cmd->viewangles);
	}

	auto handle_nospread = [&](CUserCmd* pcmd)
	{
		nospread::HandleCmd(pcmd, w);
		if (MENU_NORECOIL && !aimbot::dummy)
		{
			nospread::FixRecoil(pcmd);
			NormalizeAngles(pcmd->viewangles);
		}
	};

	handle_nospread(cmd);
	
	if (MENU_SEMIFULL && !aimbot::dummy && !aimbot::bullet)
		del(cmd->buttons, IN_ATTACK);

	if (tf2() && MENU_AUTOSTAB && w && !strcmp(w->GetClientClass()->m_pNetworkName, "CTFKnife") &&
			!me->TF2_IsCloaked() && ReadPtr<bool>(w, m_bReadyToBackstab))
	{
		add(cmd->buttons, IN_ATTACK);
	}


	if (MENU_AUTORELD && w && !w->IsMelee() && !w->GetClip1() && !w->IsReloading())
	{
		add(cmd->buttons, IN_RELOAD);
		del(cmd->buttons, IN_ATTACK);
	}


	if (aim)
	{
		if (chk(cmd->buttons, IN_ATTACK) && aimbot::bullet)
		{
			aimbot::next_shot = aimbot::target_id;
		}
	}
	else
	{
		aimbot::next_shot = 0;
	}


	if (gmod() && MENU_PROPKILL && w && !strcmp(w->GetClientClass()->m_pNetworkName, "CWeaponPhysGun"))
	{
		static auto hold = 0;
		static auto punt = 0;

		if (chk(cmd->buttons, IN_ATTACK))
		{
			const auto latency{ engine->GetNetChannel()->GetPing() };

			hold = static_cast<int>((1.0f / globals->interval_per_tick) * (latency + 0.05f));
			punt = static_cast<int>((1.0f / globals->interval_per_tick) * (latency + 0.2f));
		}
		else
		{
			if (hold > 0)
			{
				add(cmd->buttons, IN_ATTACK);
				hold--;
			}
			
			constexpr auto offset{ 0x7F };
			if (punt > 0)
			{
				*cmd->mousewheel() = offset;
				punt--;
			}
		}
	}

	if (MENU_FAKEDUCK && css() && me->IsAlive() && me->IsOnGround() && chk(cmd->buttons, IN_DUCK) && (cmd->tick_count % 2))
	{
		del(cmd->buttons, IN_DUCK);
		del(cmd->buttons, IN_ATTACK);

		send_packet = false;
	}

	if (MENU_SMACAIMB)
	{
		static auto old = cmd->viewangles;
		auto snap = (cmd->viewangles - old);

		NormalizeAngles(snap);

		const auto smac{ 42.f };
		if (snap.Normalize() > smac)
		{
			cmd->viewangles = old + snap * smac;
			NormalizeAngles(cmd->viewangles);

			if (aimbot::bullet)
			{
				del(cmd->buttons, IN_ATTACK);
			}
		}

		old = cmd->viewangles;
	}

	if (me->IsAlive())
	{
		static auto move = 400.f; // move = max(move, (abs(cmd->move.x) + abs(cmd->move.y)) * 0.5f);
		const auto s_move = move * 0.5065f;

		if (MENU_AUTOSTRF)
		{
			if ((chk(cmd->buttons, IN_JUMP) || !me->IsOnGround()) && me->GetWaterLevel() < 2)
			{
				cmd->move.x = move * 0.015f;
				cmd->move.y += static_cast<float>(((cmd->tick_count % 2) * 2) - 1) * s_move;
				
				if (cmd->mousex)
				{
					cmd->move.y = static_cast<float>(clamp(cmd->mousex, -1, 1)) * s_move;
				}

				static auto strafe	= cmd->viewangles.y;

				const auto rt = cmd->viewangles.y;
				const auto rotation{ strafe - rt };

				if (rotation < FloatNegate(globals->interval_per_tick))
					cmd->move.y = -s_move;

				if (rotation > globals->interval_per_tick)
				{
					cmd->move.y = s_move;
				}

				strafe = rt;
			}
		}

		Vector qmove;
		auto fixed_va = cmd->viewangles;
		const auto speed = cmd->move.Length2D();
		
		VectorAngles(cmd->move, qmove);
		NormalizeAngles(fixed_va);

		const auto yaw = Deg2Rad(fixed_va.y - qrv.y + qmove.y);
		const auto nyaw = FloatNegate(yaw);

		if (cmd->viewangles.x < -90.f || cmd->viewangles.x > 90.f)
		{
			cmd->move.x = FloatNegate(sin(nyaw) * speed);
			cmd->move.y = FloatNegate(cos(nyaw) * speed);
		}
		else
		{
			cmd->move.x = cos(yaw) * speed;
			cmd->move.y = sin(yaw) * speed;
		}

		if (MENU_BUNNYHOP)
		{
			static bool firstjump = false;
			static bool fakejmp;

			if (chk(cmd->buttons, IN_JUMP) && me->GetWaterLevel() < 2)
			{
				if (!firstjump)
				{
					firstjump = fakejmp = 1;
				}
				else if (!me->IsOnGround())
				{
					if (MENU_BHOPSAFE && fakejmp && me->GetVelocity().z < 0.0f)
					{
						fakejmp = false;
					}
					else
					{
						del(cmd->buttons, IN_JUMP);
					}
				}
				else
				{
					fakejmp = true;
				}
			}
			else
			{
				firstjump = false;
			}
		}
	}

	if (MENU_FAKELAGB)
	{
		static auto q = 0;

		if (q > 0 && !((MENU_FAKELAGA && LocalPlayer()->GetVelocity().Length() < 100.f) ||
			MENU_FAKELAGS && !aimbot::dummy && aimbot::bullet && chk(cmd->buttons, IN_ATTACK)))
		{
			q--;
			send_packet = false;
		}
		else
		{
			q = MENU_FAKELAGN;
		}
	}

	if (MENU_CHATSPAM && !(cmd->tick_count % static_cast<int>(0.2f / globals->interval_per_tick)))
	{
		char disrupt[] =
		{
			0x7F,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			0
		};

		util::SendStringCmd("say %s", disrupt);
	}
	
	if (MENU_NAMESTLR)
	{
		namestealer::Think();
	}
}

DECLVHOOK(void, SetViewAngles, (Vector &a))
{
	auto cmd = get_SI<CUserCmd*>();
	auto sequence_number = (*get_BP<unsigned**>() + 2);
	if (MENU_NOSETVRC)
	{
		if (gmod())
		{
			if ((cmd != nullptr) && cmd->tick_count == 0 && cmd->predicted)
			{
				return;
			}
		}
		if (dod())
		{
			if (get_SI<CBaseEntity*>() == LocalPlayer())
			{
				return;
			}
		}
	}
	
	orgSetViewAngles(a);
	
	if ((cmd != nullptr) && cmd->command_number == *sequence_number)
	{
		static auto i = 0;
		if (MENU_OVERSPED && util::IsKeyDown(MENU_SPEEDHAK) && i-- > 0)
		{
			*(****get_BP<unsigned long*****>() + 1) -= 5;
		}
		else
		{
			i = MENU_OVERSPED;
		}
		
		create_move(cmd);
		
		*sequence_number = cmd->command_number;
		*(***get_BP<bool****>() - 1) &= send_packet;
	}
}
DECLVHOOK(void, RunCommand, (CBaseEntity* pl, CUserCmd* cmd, void* mv))
{
	__asm push ecx
	
	static auto tick_count = 0;
	auto norun = tick_count == cmd->tick_count;
	tick_count = cmd->tick_count;
	
	__asm pop ecx
	
	if (norun)
	{
		static auto o_predcmd{ 0 };
		
		if (!o_predcmd)
		{
			o_predcmd = *reinterpret_cast<int*>(util::FindPattern(pd->GetMethod(17), 0x64, "\x89?????\xE8") + 2);
		}
		
		WritePtr<CUserCmd*>(pl, o_predcmd, cmd);
		
		char md[0xA4];
		pd->SetupMove(pl, cmd, md);
		movement->ProcessMovement(pl, md);
		pd->FinishMove(pl, cmd, md);
	}
	else
	{
		orgRunCommand(pl, cmd, mv);
	}
}
static bool open_menu = 0; 
DECLVHOOK(void, PaintTraverse, (unsigned p, void* a2, void* a3))
{
	orgPaintTraverse(p, a2, a3);
	if (((gmod() && !strcmp(panel->GetName(p), "OverlayPopupPanel")) || (e_orgbx() && !strcmp(panel->GetName(p), "FocusOverlayPanel")) || false) && !panel->GetChildCount(p)) {
		surface->DrawSetTextFont(draw::GetFont());
		if (engine->IsInGame() && LocalPlayer())
		{
			gui::RenderInGameOverlay();
		}
		
		extern forms::F_Form* usermenu;
		
		if (forms::Init())
		{
			gui::InitForms();
		}
		
		forms::Render();
		
		if (usermenu)
		{
			if (((GetAsyncKeyState(VK_INSERT) & 1) != 0) && !forms::F_KeyTrap::glob_active)
			{
				usermenu->SetVisible(!usermenu->GetVisible());
			}
			
			open_menu ? panel->SetMouseInputEnabled(p, true) : panel->SetMouseInputEnabled(p, false);
			
			if (open_menu != usermenu->GetVisible())
			{
				open_menu = usermenu->GetVisible() ? pipe::LoadConfig("generic.cfg") : pipe::SaveConfig("generic.cfg");
			}
		}
	} else {
		static auto p_view = 0;
		if (!p_view && !strcmp(panel->GetName(p), "CBaseViewport"))
		{
			p_view = p;
		}

		if (p == p_view)
		{
			draw::w2s = engine->GetViewMatrix();
		}
	}
}
DECLVHOOK(void, LockCursor, ())
{
	if (open_menu) 
	{
		isystem->EnableInput(false);
		surface->UnlockCursor();
	}
	else 
	{
		isystem->EnableInput(true);
		return orgLockCursor();
	}
}
DECLVHOOK(bool, DispatchUserMessage, (int msgn, char** buf))
{
	if ((strstr(*buf, "Chat") != nullptr) && unsigned(buf[2]) > 0x4B0)
	{
		return true;
	}
	return orgDispatchUserMessage(msgn, buf);
}
char hooked_get_user_cmd[0x64];

int __stdcall DllMain(void*, const int r, void*)
{
	if (r == 1 /* && OpenMutex(1, 1, "/nhmtx") */)
	{
		memset(menu, 0, sizeof(menu));
		const auto mod = pipe::ModQuery();
		
		pipe::log = pipe::OpenFile("beta/nanohack.log", "a");
		
		panel = new IPanel();
		surface = new ISurface();
		client = new IClient();
		
		input = new EInterface(**(unsigned long**)(util::FindPattern(client->GetMethod(21), 0x100, "\x8B\x0D") + 2));
		input->HookMethod(8, hooked_get_user_cmd, 0);
		
		memcpy(hooked_get_user_cmd, (void*)input->GetMethod(8), 0x64);
		memset(
			(void*)(util::FindPatternComplex((unsigned long)hooked_get_user_cmd, 0x64, 2, "\x0F\x95?", "\x0F\x45?") - 3
			), 0x90, 3);
		
		DWORD unused;
		VirtualProtect(hooked_get_user_cmd, sizeof(hooked_get_user_cmd), PAGE_EXECUTE_READWRITE, &unused);
		
		trace = new ITrace();
		engine = new IEngine();
		physics = new IPhysics();
		ents = new IEntities();
		mdlinfo = new IModelInfo();

		movement = new IMovement();
		pd = new IPrediction();
		isystem = new IInputSystem();
		iplayer = new IPlayerInfoManager();
		globals = iplayer->GetGlobalVars();
		
		Netvars::Initialize();
		Netvars::Dump("nanohack/netvars.txt");

		Netvars::HookNetvar("DT_LocalPlayerExclusive", "m_nTickBase", fix_n_tick_base);
		if (css())
		{
			Netvars::HookNetvar("DT_CSPlayer", "m_flFlashMaxAlpha", gui::Proxy_FlashSmoke);
			Netvars::HookNetvar("DT_ParticleSmokeGrenade", "m_flSpawnTime", gui::Proxy_FlashSmoke);
		}
		
		if (tf2())
		{
			Netvars::HookNetvar("DT_TFPlayerShared", "m_nPlayerCond", gui::Proxy_Jarate);
		}
		
		if (gmod())
		{
			extern JMP* jmp_fire_bullets;
			extern void __fastcall hooked_FireBullets(CBaseEntity * ecx, void*, char* fb);
			jmp_fire_bullets = new JMP(
				(void*)util::FindPattern(
					"client",
					"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC????\x56\x8B\x73\x08"),
				static_cast<void*>(hooked_FireBullets));
		}
		
		client->HookMethod(36, &hooked_DispatchUserMessage, &orgDispatchUserMessage);
		
		engine->HookMethod(20, &hooked_SetViewAngles, &orgSetViewAngles);
		
		panel->HookMethod(41, &hooked_PaintTraverse, &orgPaintTraverse); //you could try hooking Paint.
		
		pd->HookMethod(17, &hooked_RunCommand, &orgRunCommand);

		surface->HookMethod(62, &hooked_LockCursor, &orgLockCursor);
		
		util::Message("NanoHack 2.2 startup (build " __TIMESTAMP__ ", %s)", mod);
		pipe::LoadConfig("generic.cfg");
	}
	return 1;
}
