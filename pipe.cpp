#include "pipe.h"

int		pipe::appid	= 0;
_iobuf*	pipe::log	= 0;

bool pipe::LoadConfig(const char* fn, bool test)
{
	_iobuf* f = fopen(format("nanohack/%s", fn), "rb");

	if (!f)
		return 0;

	fseek(f, 0, SEEK_END);

	bool ok = 0;
	if (ftell(f) == (sizeof(int) * MENU_ITEMS + sizeof(unsigned long) + 1))
	{
		int sh[MENU_ITEMS];
		char h = 0;

		//CRC_t m_crc;

		rewind(f);
		fread(&h, 1, 1, f);
		fread(sh, 1, sizeof(sh), f);
		//fread(&m_crc, 4, 1, f);

		if (ok = (h == 'p' /* && (CRC32_ProcessSingleBuffer(sh, sizeof(sh)) == m_crc) */))
		if (!test)
			memcpy(menu, sh, sizeof(int) * MENU_ITEMS);
	}

	fclose(f);

	return ok;
}

bool pipe::SaveConfig(const char* fn)
{
	_iobuf* f = OpenFile(format("nanohack/%s", fn));

	if (!f)
		return 0;

	//CRC32_t crc = CRC32_ProcessSingleBuffer(menu, sizeof(menu));
	
	fwrite("p", 1, 1, f);
	fwrite(menu, 1, sizeof(menu), f);
	//fwrite(&crc, sizeof(crc), 1, f);
	fclose(f);

	return 1;
}

char* pipe::ModQuery()
{
	if (!appid)
	{
		FILE* f = fopen("steam_appid.txt", "r");

		if (!f)
			return "unknown";

		char a[10];

		fread(a, 1, 10, f);
		appid = atoi(a);
		fclose(f);
	}

	switch (appid)
	{
	case 240:	return "css";
	case 300:	return "dod";
	case 440:	return "tf2";
	case 4000:	return "gmod";
	default:	return "unknown";
	}
}

_iobuf* pipe::OpenFile(const char* fn, const char* m)
{
	if (access(fn, 0) == -1)
	{
		char p[MAX_PATH]; strcpy(p, fn);

		for (unsigned i = 0; i < strlen(p); i++)
	 	if (p[i] == '/')
		{
			char oldch = p[i]; p[i] = 0;
			CreateDirectory(p, 0);
			p[i] = oldch;
		}
	}

	return fopen(fn, m);
}