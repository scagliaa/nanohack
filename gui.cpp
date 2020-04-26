#include "gui.h"
#include "menu.h"

extern ITrace* trace;
extern IEngine* engine;
extern IEntities* ents;
extern IModelInfo* mdlinfo;

extern GlobalVars* globals;

int menu[MENU_ITEMS];

void gui::Proxy_FlashSmoke(const RecvProxyData* i, void*, float* o)
{
	if (MENU_INTERFRN)
		*o = 0.f;
	else
		*o = i->m_Value.m_Float;
}

void gui::Proxy_Jarate(const RecvProxyData* i, CBaseEntity* pl, int* o)
{
	int cond = i->m_Value.m_Int;

	if (MENU_INTERFRN && pl == LocalPlayer())
		del(cond, PlayerCond_Jarated);

	*o = cond;
}

void gui::RenderInGameOverlay()
{
	int maxpl = globals->max_clients;
	int maxen = ents->GetHighestEntityIndex();

	CBaseEntity* me = LocalPlayer();
	int myteam = me->GetTeam();
	
	if (MENU_ENEMYESP) for (int i = 1; i <= maxpl; ++i)
	if (CBaseEntity* pl = ents->GetClientEntity(i))
	{
		if (pl == me)
			continue;

		if (pl->IsDormant())
			continue;
			
		if (!pl->IsAlive())
			continue;
		
		if (!pl->GetModel())
			continue;
		
		int plteam = pl->GetTeam();
		if (MENU_ENEMYESP == 1 || ((MENU_ENEMYESP == 2 && myteam != plteam) || (MENU_ENEMYESP == 3 && myteam == plteam)))
		{
			Vector vec = pl->GetAbsCenter(), svec;
			draw::ToScreen(vec, svec);

			unsigned long ESP = pl->GetESPColor();

			int n_lines = 0;

			#define esp_x svec.x
			#define esp_y svec.y + (n_lines++) * 12

			if (MENU_ESPNAMES)
				draw::Text(esp_x, esp_y, 0, ESP, pl->GetNickSafe());

			if (MENU_WEAPONWH)
			if (CBaseEntity* w = pl->GetActiveWeapon())
			{
				char wname[256]; strcpy(wname, w->GetClassname());

				#define	filter(w) if (char* kw = strstr(wname, w)) strcpy(kw, kw + strlen(w))
				
				filter("weapon");

				if (tf2())
				{
					filter("tf");
					filter("hwg");
					filter("medic");
					filter("primary");
				}

				draw::Text(esp_x, esp_y, 0, ESP, util::MakeFancy(wname));
			}
			else
				draw::Text(esp_x, esp_y, 0, ESP, "**Weaponless**");

			if (tf2() && MENU_CLASSESP)
				draw::Text(esp_x, esp_y, 0, ESP, pl->TF2_GetClass());
				
			if (MENU_HPBARESP)
				draw::Text(esp_x, esp_y, 0, pl->GetHPColor(), "%02i HP", pl->GetHealth());

			//todo: recode
			if (MENU_STATEESP)
			{
				char state[128];
				int cond = pl->TF2_GetPlayerCond();
				
				#define ConfirmState(s)	{ *state = '*'; strcpy(state + strlen(state), s); }
				*state = 0;

				if (tf2())
				{
					#define alias(a, c) if (chk(cond, c)) ConfirmState(a)

					alias("Slow", PlayerCond_Slowed	);
					alias("Disg", PlayerCond_Disguised);
					alias("Clkd", PlayerCond_Cloaked);
					alias("Uber", PlayerCond_Ubercharged);
					alias("Bonk", PlayerCond_Bonked);
					alias("Tntn", PlayerCond_Taunting);

					#undef alias
				}

				if (*state == '*')
					draw::Text(esp_x, esp_y, 0, ESP, state);

				#undef ConfirmState
			}

			#undef esp_x
			#undef esp_y
		}
	}

	if (MENU_ENTITYWH) for (int i = maxpl + 1; i <= maxen; ++i)
	if (CBaseEntity* e = ents->GetClientEntity(i))
	{
		if (e->IsDormant())
			continue;

		if (!e->GetModel())
			continue;

		ClientClass* cl = e->GetClientClass();
		Vector sorigin;
			
		if (!cl)
			continue;
			
		const char* cclass = cl->m_pNetworkName;
		
		if (!cclass)
			continue;

		#define	TEAM_COLOR(E) (E->GetTeam() == TEAM_RED ? CC_LIGHTRED : (E->GetTeam() == TEAM_BLU ? CC_LIGHTBLUE : CC_GREEN))

		#define RegEntityESP(CLASS, ICON) if (!strcmp(cclass, #CLASS))\
		{\
			draw::ToScreen(e->GetAbsCenter(), sorigin);\
			draw::Text(sorigin.x, sorigin.y, 1, TEAM_COLOR(e), ICON);\
			continue;\
		}

		#define RegSpecificESP(CLASS, ICON, COLOR) if (!strcmp(cclass, #CLASS))\
		{\
			draw::ToScreen(e->GetAbsCenter(), sorigin);\
			draw::Text(sorigin.x, sorigin.y, 1, COLOR, ICON);\
			continue;\
		}

		if (css())
		{
			RegSpecificESP(CC4, "[C4]", CC_LIGHTRED);
			RegSpecificESP(CPlantedC4, "[*C4]", CC_RED);

			// e->GetESPColor()

			RecvTable* p = cl->m_pRecvTable->m_pProps[0].m_pDataTable;

			if (p && strstr(p->m_pNetTableName, "Weapon") && !ReadPtr<char>(e, m_iState))
			{
				char wname[128];
				strcpy(wname, e->GetClassname());

				draw::ToScreen(e->GetAbsCenter(), sorigin);
				draw::Text(sorigin.x, sorigin.y, 1, CC_GREY, util::MakeFancy(wname));
			}
		}

		if (tf2())
		{
			#define RegStructureESP(CLASS, ICON) if (!strcmp(cclass, #CLASS))\
			{\
				draw::ToScreen(e->GetAbsCenter(), sorigin);\
				draw::Text(sorigin.x, sorigin.y, 1, TEAM_COLOR(e), "%s", ICON);\
				if (ReadPtr<bool>(e, m_bHasSapper))\
					draw::Text(sorigin.x, sorigin.y + 13, 1, COLOR(0, 255, 225), "**Sapped**");\
				continue;\
			}

			if (!strcmp(cclass, "CBaseAnimating"))
			{
				const char* mdl = mdlinfo->GetModelName(e->GetModel());
				
				if (strstr(mdl, "ammopack"))
				{
					draw::ToScreen(e->GetAbsCenter(), sorigin);
					draw::Text(sorigin.x, sorigin.y, 1, CC_GREY, "Ammo");
				}

				if (strstr(mdl, "medkit"))
				{
					draw::ToScreen(e->GetAbsCenter(), sorigin);
					draw::Text(sorigin.x, sorigin.y, 1, CC_LIGHTGREEN, "Medkit");
				}

				continue;
			}

			RegEntityESP(CZombie,		"Skeleton");
			RegEntityESP(CTFRobotDestruction_Robot, "Robot");
			RegSpecificESP(CTFAmmoPack, "Debris", CC_GREY);

			RegSpecificESP(CTFTankBoss,		"Tank",				CC_LIGHTBLUE);
			RegSpecificESP(CMerasmus,		"Merasmus",			COLOR(100, 225, 100));
			RegSpecificESP(CEyeballBoss,	"Monoculus",		CC_LIGHTBLUE);
			RegSpecificESP(CHeadlessHatman,	"Headless Hatman",	COLOR(170, 0, 225));
				
			RegStructureESP(CObjectSentrygun,	"Sentry");
			RegStructureESP(CObjectDispenser,	"Dispenser");
			RegStructureESP(CObjectTeleporter,	"Teleporter");
				
			if (!strcmp(cclass, "CTFGrenadePipebombProjectile"))
			{
				draw::ToScreen(e->GetAbsCenter(), sorigin);
				draw::Text(sorigin.x, sorigin.y, 1, TEAM_COLOR(e),
					ReadPtr<int>(e, m_iType) ? "<^>" : "[_|)");
				
				continue;
			}
			if (e->IsDummyProjectile())
				continue;

			RegEntityESP(CTFStunBall,					"(_)");
			RegEntityESP(CTFFlameRocket,				"~|_)");
			RegEntityESP(CTFProjectile_Arrow,			"-===>");
			RegEntityESP(CTFProjectile_Rocket,			"[==)>");
			RegEntityESP(CTFProjectile_EnergyBall,		"--=o}");
			RegEntityESP(CTFProjectile_HealingBolt,		"=====");
			RegEntityESP(CTFProjectile_SentryRocket,	"=> =>");
		}
	}

	if (MENU_CROSHAIR)
	{
		int cx, cy;
		engine->GetScreenSize(cx, cy);

		cx /= 2;
		cy /= 2;

		draw::Rect(cx - 2,	cy - 1,	2, 2, CC_WHITE);
		draw::Rect(cx,		cy - 1,	2, 2, CC_WHITE);
		draw::Rect(cx - 1,	cy - 2,	2, 2, CC_WHITE);
		draw::Rect(cx - 1,	cy,		2, 2, CC_WHITE);
	}
}

forms::F_Form* usermenu;




void rage_Open(forms::F_ComboBox* box)
{
	box->Clear();
	box->AddItem("");

	if (engine->IsInGame())
	for (int i = 1, lp = engine->GetLocalPlayer(); i <= globals->max_clients; ++i)
	{
		if (i == lp)
		{
			continue;
		}

		CBaseEntity* pl = ents->GetClientEntity(i);

		if (!pl)
		{
			continue;
		}

		const char* name = pl->GetNickSafe();

		if (!name)
			continue;

		box->AddItem(name);
	}
}

int rage_Select(forms::F_ComboBox* box)
{
	if (!box->item_selected)
		return 0;

	if (engine->IsInGame())
	for (int i = 1, j = 1, lp = engine->GetLocalPlayer(); i <= globals->max_clients; ++i)
	{
		if (i == lp)
			continue;

		CBaseEntity* pl = ents->GetClientEntity(i);

		if (!pl)
			continue;

		const char* name = pl->GetNickSafe();

		if (!name)
			continue;

		if (j == box->item_selected)
			return pl->EntIndex();

		j++;
	}

	return 0;
}

void gui::InitForms()
{
	int sw, sh;
	engine->GetScreenSize(sw, sh);

	// Menu stuff

	usermenu = new forms::F_Form();
	usermenu->SetSizeable();
	usermenu->SetSize(350, 475);
	usermenu->SetTitle("NanoHack 2.2");
	usermenu->Center();
	
	forms::F_PanelHolder*
		holder = new forms::F_PanelHolder();
		holder->SetParent(usermenu);
		holder->SetSize(340, 455);

#define	push()	panel->fill_x += 10
#define	pop()	panel->fill_x -= 10

#define	AddCheckbox(FeatureName, FeatureReference) {\
	forms::F_Checkbox* box = new forms::F_Checkbox();\
	box->SetSize(250, 11);\
	box->SetText(FeatureName);\
	box->SetVariable(&FeatureReference);\
	box->SetParent(panel);}

#define AddKeyTrap(FeatureReference, Y) {\
	forms::F_KeyTrap* trap = new forms::F_KeyTrap();\
	trap->SetSize(100, 16);\
	trap->SetPos(235, Y);\
	trap->SetVariable(&FeatureReference);\
	trap->SetParent(panel);}

#define AddComboBox(FeatureReference, Y, Items) {\
	forms::F_ComboBox* cbox = new forms::F_ComboBox();\
	cbox->SetSize(100);\
	cbox->SetParent(panel);\
	cbox->SetPos(235, Y);\
	cbox->SetVariable(&FeatureReference);\
	cbox->AddItems Items;}

#define AddSlider(FeatureName, FeatureReference, MIN, MAX) {\
	forms::F_Slider* slider = new forms::F_Slider();\
	slider->SetSize(200, 35);\
	slider->SetVariable(&FeatureReference);\
	slider->SetText(FeatureName);\
	slider->SetRange(MIN, MAX);\
	slider->SetParent(panel);}

#define AddButton(FeatureName, ButtonCallback) {\
	forms::F_Button* button = new forms::F_Button();\
	button->SetOnClickFn(ButtonCallback);\
	button->SetText(FeatureName);\
	button->SetSize(100, 16);\
	button->SetParent(panel);}

#define AddTextBox(E_NAME, W, X, Y)\
	forms::F_TextBox* E_NAME = new forms::F_TextBox();\
	E_NAME->SetSize(W, 16);\
	E_NAME->SetPos(X, Y);\
	E_NAME->SetParent(panel);

#define new_Panel() if (forms::F_Panel* panel = new forms::F_Panel())
#define	def_Panel(title) panel->AutoFill(1); holder->AddPanel(panel, title);

	new_Panel()
	{
		def_Panel("Main");

		panel->AddSplitter("Ragebot");

		AddCheckbox("Override aimbot filter", MENU_RAGEBOTE);

		{
			forms::F_ComboBox* cbox = new forms::F_ComboBox();
				cbox->SetSize(usermenu->w - 20);
				cbox->SetParent(panel);
				cbox->AddItem("");
				cbox->SetVariable(&ragebot);

				cbox->cbOpen = rage_Open;
				cbox->cbSelect = rage_Select;
		}
	}

	new_Panel()
	{
		def_Panel("Aimbot");

		panel->AddSplitter("Aimbot");
		
		AddCheckbox("Enabled", MENU_AIMBOTEN);

		push();

		AddCheckbox("SMAC mode", MENU_SMACAIMB);
		
		if (tf2())
			AddCheckbox("Airblast mode", MENU_PROPKILL);

		pop();

		AddCheckbox("Bone scan", MENU_BONESCAN);

		if (css())
			AddCheckbox("Auto wall", MENU_AUTOWALL);

		AddCheckbox("Auto shoot", MENU_AUTOSHOT);
		AddCheckbox("Silent aim", MENU_FAKEVIEW);

		panel->autofill = 0;

		AddKeyTrap(MENU_AIMBOTKB, 20);

		AddComboBox(MENU_AIM4TEAM, 45,
			(4,
				"Both teams",
				"Enemies",
				"Teammates",
				"NPCs only"
			)
		);

		AddComboBox(MENU_PRIORSYS, 70,
			(4,
				"Vector distance",
				"Angular distance",
				"Threat level",
				"Next shot"
			)
		);
		
		AddComboBox(MENU_AIMBSPOT, 95,
			(3,
				"Head",
				"Chest",
				"Stomach"
			)
		);

		AddComboBox(MENU_SPAWPROT, 120,
			(4,
				"Off",
				"SP 1.5",
				"CSS DM",
				"GMOD Stronghold"
			)
		);

		panel->autofill = 1;

		panel->Skip();
		
		AddCheckbox("NP aimbot", MENU_NPCAIMBT);
		AddCheckbox("Aim at Steam friends", MENU_STEAMBRO);
		AddCheckbox("Aim through breakables", MENU_AIMGLASS);
		
		panel->AddSplitter("Accuracy");
		
		AddCheckbox("No punch", MENU_NORECOIL);
		AddCheckbox("No recoil", MENU_NOSETVRC);
		
		panel->Skip();
		
		if(!tf2())
			AddCheckbox("No spread", MENU_NOSPREAD); 

		if (tf2()) // deprecated
			AddCheckbox("SMAC minigun mode", MENU_SMACSEED);
	}

	new_Panel()
	{
		def_Panel("Auto");

		panel->AddSplitter("Controls");
		
		if (gmod())
		{
			AddCheckbox("Propkill helper", MENU_PROPKILL);
			panel->Skip();
		}

		if (tf2())
			AddCheckbox("Auto backstab", MENU_AUTOSTAB);

		AddCheckbox("Auto pistol", MENU_SEMIFULL);
		AddCheckbox("Auto reload", MENU_AUTORELD);

		panel->Skip();

		AddCheckbox("Auto jump", MENU_BUNNYHOP);

		push();

			AddCheckbox("SMAC mode", MENU_BHOPSAFE);

		pop();
	}

	new_Panel()
	{
		def_Panel("Screenspace");

		panel->AddSplitter("Visuals");

		AddCheckbox("No visual interferences", MENU_INTERFRN);
		AddCheckbox("No visual recoil", MENU_NOVISREC);

		panel->AddSplitter("ESP");
		panel->autofill = 0;

		AddComboBox(MENU_ENEMYESP, panel->fill_vert + 5,
			(4,
				"No ESP",
				"Both teams",
				"Enemies",
				"Teammates"
			)
		);

		panel->autofill = 1;

		AddCheckbox("NP ESP", MENU_ENTITYWH);

		panel->Skip();
		
		AddCheckbox("Draw name", MENU_ESPNAMES);
		AddCheckbox("Draw health", MENU_HPBARESP);
		AddCheckbox("Draw weapon", MENU_WEAPONWH);

		if (tf2())
			AddCheckbox("Draw class", MENU_CLASSESP);

		AddCheckbox("Draw state", MENU_STATEESP);
		
		panel->AddSplitter("Miscellaneous");

		AddCheckbox("Crosshair", MENU_CROSHAIR);
	}
	
	new_Panel()
	{
		def_Panel("Misc");

		if (css())
		{
			panel->AddSplitter("Fake duck");

			AddCheckbox("Enabled", MENU_FAKEDUCK);
		}

		panel->AddSplitter("Fake lag");

		AddCheckbox("Enabled", MENU_FAKELAGB);

		panel->Skip();

		AddCheckbox("On shot", MENU_FAKELAGS);
		AddCheckbox("On speed", MENU_FAKELAGA);
		
		panel->Skip();

		AddSlider("Factor", MENU_FAKELAGN, 0, 22);

/*		panel->AddSplitter("Speedhack");
		panel->autofill = 0;
		
		AddKeyTrap(MENU_SPEEDHAK, panel->fill_vert + 2);

		panel->autofill = 1;

		AddSlider("Factor", MENU_OVERSPED, 0, 21);*/

		// speedhack is patched. Now with 62 ticks hardcoded limitation. Smh.

		panel->AddSplitter("Movement");
		
		AddCheckbox("No restrictions", MENU_NOMOVMNT);

		panel->Skip();

		AddCheckbox("Air strafer", MENU_AUTOSTRF);
		AddCheckbox("Ground strafer", MENU_WALKBOST);

		panel->AddSplitter("Namestealer");
		panel->autofill = 0;

		AddKeyTrap(MENU_CALLVOTE, panel->fill_vert + 2);
		
		panel->autofill = 1;

		AddCheckbox("Enabled", MENU_NAMESTLR);
		AddCheckbox("Same team only", MENU_NMSTTEAM);
		AddCheckbox("SMAC mode", MENU_NMSTSWAP);

		if (gmod())
			AddCheckbox("DarkRP mode", MENU_DARKRPNM);

		panel->AddSplitter("IO jammer");

		AddCheckbox("Enabled", MENU_CHATSPAM);
	}
}