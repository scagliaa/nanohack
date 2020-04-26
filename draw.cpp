#include "draw.h"

extern ISurface* surface;
extern IEngine* engine;

matrix4x4 &draw::w2s = matrix4x4();

void draw::ToScreen(const Vector& world, Vector& scr)
{
	float w;

	scr.x	= w2s[0][0] * world.x + w2s[0][1] * world.y + w2s[0][2] * world.z + w2s[0][3];
	scr.y	= w2s[1][0] * world.x + w2s[1][1] * world.y + w2s[1][2] * world.z + w2s[1][3];
	w		= w2s[3][0] * world.x + w2s[3][1] * world.y + w2s[3][2] * world.z + w2s[3][3];

	if (w < 0.001f)
	{
		scr.x *= 100000.f;
		scr.y *= 100000.f;
	}
	else
	{
		float invw = 1.0f / w;

		scr.x *= invw;
		scr.y *= invw;
	}
	
	int sw, sh;
	engine->GetScreenSize(sw, sh);

	float x = sw / 2;
	float y = sh / 2;

	x += 0.5f * scr.x * sw + 0.5f;
	y -= 0.5f * scr.y * sh + 0.5f;

	scr.x = x;
	scr.y = y;
}


void draw::Line(int x0, int y0, int x1, int y1, unsigned color)
{
	surface->DrawSetColor(RGBA(color));
	surface->DrawLine(x0, y0, x1, y1);
}

void draw::Rect(int x0, int y0, int x1, int y1, unsigned color)
{
	surface->DrawSetColor(RGBA(color));
	surface->DrawFilledRect(x0, y0, x0 + x1, y0 + y1);
}

void draw::OutlinedRect(int x0, int y0, int x1, int y1, unsigned color)
{
	surface->DrawSetColor(RGBA(color));
	surface->DrawOutlinedRect(x0, y0, x0 + x1, y0 + y1);
}

void draw::Text(int x, int y, int align, unsigned color, const char* text, ...)
{
	wchar_t wstr[128];
	char	cstr[256];

	va_list	va_alist;
	va_start(va_alist, text);
	vsprintf(cstr, text, va_alist);
	va_end(va_alist);
	
	MultiByteToWideChar(CP_UTF8, 0, cstr, -1, wstr, 128);

	if (align)
	{
		int w, h;
		surface->GetTextSize(GetFont(), wstr, w, h);

		if (align == 1 && w > 0)
			w /= 2;

		x -= w;
	}
	surface->DrawSetTextColor(RGBA(color));
	surface->DrawSetTextPos(x, y);
	surface->DrawPrintText(wstr, strlen(cstr));
}

unsigned long draw::GetFont()
{
	static unsigned long font = 0;

	if (!font)
	{
		font = surface->CreateFont();
		surface->SetFontGlyphSet(font, "tahoma", 14, 700, 0, 0, 0x200, 0, 0);
	}

	return font;
}