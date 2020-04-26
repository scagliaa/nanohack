#pragma once
#include "main.h"

class CBaseEntity;
class ConVar;

namespace namestealer
{
	struct Letter
	{
		char	latin;
		char*	cyrilic;
	};

	extern int p, q;
	extern char beacon[32];

	extern Letter swap[];

	void Think();
	void UpdateNameOnServer(char* new_name);

	int	ValidPlayers();

	bool IsValidPlayer(CBaseEntity* pl);
	bool IsValidPlayer(int pl);
	bool CanDoUnicodeSwap(const char* s);
}