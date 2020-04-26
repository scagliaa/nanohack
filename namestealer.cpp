#include "namestealer.h"

extern IEngine* engine;
extern IEntities* ents;
extern GlobalVars* globals;

int	 namestealer::p = 0, namestealer::q = 0;
char namestealer::beacon[32] = {0};

namestealer::Letter namestealer::swap[] =
{
	{'a', "\xD0\xB0"},
	{'A', "\xD0\x90"},
	{'E', "\xD0\x95"},
	{'e', "\xD0\xB5"},
	{'o', "\xD0\xBE"},
	{'y', "\xD1\x83"},
	{'H', "\xD0\x9D"},
	{'T', "\xD0\xA2"},
	{'M', "\xD0\x9C"},
	{'C', "\xD0\xA1"},
	{'c', "\xD1\x81"},
	{'B', "\xD0\x92"},
	{'P', "\xD0\xA0"},
	{'p', "\xD1\x80"},
	{'X', "\xD0\xA5"},
	{'x', "\xD1\x85"},
	{'K', "\xD0\x9A"},
};


void namestealer::UpdateNameOnServer(char* new_name)
{
	static char msg_SetConVar[0x250];
	memset(msg_SetConVar, 0, sizeof(msg_SetConVar));

	static void (__thiscall* ob_SetConVar)(void*, const char*, const char*) = (void (__thiscall*)(void*, const char*, const char*))util::FunctionStart(util::FindPattern("engine", "\x08\xC6\x47\x04\x01\xC7"));

	ob_SetConVar(msg_SetConVar, "name", new_name);

	if (gmod() && MENU_DARKRPNM)
		util::SendStringCmd("darkrp rpname %s%%", util::SafeFormat(new_name) + 3);

	engine->GetNetChannel()->SendNetMsg(msg_SetConVar);
}

void namestealer::Think()
{
	if (forms::inputallowed && (GetAsyncKeyState(MENU_CALLVOTE) & 1))
	{
		player_info info;

		if (engine->GetPlayerInfo(p, &info))
			util::SendStringCmd("callvote kick %i", info.userid);
	}

	if (q)
	{
		player_info info;
		engine->GetPlayerInfo(engine->GetLocalPlayer(), &info);

		if (strcmp(beacon, info.name))
		{
			p = q;
			q = 0;

			strcpy(beacon, info.name);
		}

		return;
	}

	if (ValidPlayers() < 2)
		return;

	int user = 0;
	while (!user)
	{
		int randp = (rand() % globals->max_clients) + 1;

		if (IsValidPlayer(randp))
			user = randp;
	}

	char new_name[64] = "\xE2\x80\x8F";

	if (MENU_NMSTSWAP)
	{
		char prc_name[64];
		strcpy(prc_name, ents->GetClientEntity(user)->GetNick());
		
		for (int i = 0;; i = rand() % strlen(prc_name))
		for (int x = 0; x < sizeof(swap) / sizeof(Letter); x++)
		{
			if (swap[x].latin == prc_name[i])
			{
				memcpy(new_name, prc_name, i);
				memcpy(new_name + i, swap[x].cyrilic, 2);
				strcpy(new_name + i + 2, prc_name + i + 1);
				
				goto steal_name;
			}

			if (*(wchar_t*)(prc_name + i) == *(wchar_t*)swap[x].cyrilic)
			{
				memcpy(new_name, prc_name, i);
				memset(new_name + i, swap[x].latin, 1);
				strcpy(new_name + i + 1, prc_name + i + 2);

				goto steal_name;
			}
		}
	}
	else
		strcat(new_name, ents->GetClientEntity(user)->GetNick());

steal_name:

	q = user;

	UpdateNameOnServer(new_name);
}

int namestealer::ValidPlayers()
{
	int valid_players = 0;

	for (int i = 1; i <= globals->max_clients; ++i)
	if (IsValidPlayer(ents->GetClientEntity(i)))
		valid_players++;

	return valid_players;
}

bool namestealer::IsValidPlayer(CBaseEntity* pl)
{
	if (!pl)
		return 0;

	if (MENU_NMSTTEAM && pl->GetTeam() != LocalPlayer()->GetTeam())
		return 0;

	if (pl == LocalPlayer())
		return 0;

	if (pl->EntIndex() == p)
		return 0;

	if (pl->EntIndex() == q)
		return 0;

	if (MENU_NMSTSWAP && !CanDoUnicodeSwap(pl->GetNick()))
		return 0;

	if (pl->IsSteamFriend())
		return 0;

	return 1;
}

bool namestealer::CanDoUnicodeSwap(const char* s)
{
	for (; *s; ++s)
	for (int x = 0; x < sizeof(swap) / sizeof(Letter); x++)
	{
		if (swap[x].latin == *s)
			return 1;

		if (*(wchar_t*)s == *(wchar_t*)swap[x].cyrilic)
			return 1;
	}

	return 0;
}

bool namestealer::IsValidPlayer(int pl)
{
	return IsValidPlayer(ents->GetClientEntity(pl));
}