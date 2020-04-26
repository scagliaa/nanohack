#pragma once
#include "main.h"

#define COLORA(r,g,b,a)	((unsigned)((((r)&0xFF)<<24)|(((g)&0xFF)<<16)|(((b)&0xFF)<<8)|((a)&0xFF)))
#define COLOR(r,g,b)	((unsigned)((((r)&0xFF)<<24)|(((g)&0xFF)<<16)|(((b)&0xFF)<<8)|0xFF))
#define RED(CC)			((int)((CC)>>24))
#define BLUE(CC)		((int)((CC)>>8)&0xFF)
#define GREEN(CC)		((int)((CC)>>16)&0xFF)
#define ALPHA(CC)		((int)(CC)&0xFF)
#define RGBA(CC)		RED(CC),GREEN(CC),BLUE(CC),ALPHA(CC)

#define CC_RED			COLOR(255, 0, 0)
#define	CC_PURPLE		COLOR(150, 0, 225)
#define CC_LIGHTRED		COLOR(255, 100, 100)
#define CC_YELLOW		COLOR(200, 255, 0)
#define CC_LIGHTYELLOW	COLOR(255, 255, 100)
#define CC_GREEN		COLOR(0, 255, 0)
#define CC_LIGHTGREEN	COLOR(100, 225, 100)
#define CC_LIGHTBLUE	COLOR(70, 120, 255)
#define CC_BLUE			COLOR(0, 0, 255)
#define CC_GREY			COLOR(200, 200, 200)
#define CC_WHITE		COLOR(255, 255, 255)
#define CC_BLACK		COLOR(0, 0, 0)

namespace draw
{
	extern matrix4x4 &w2s;

	unsigned long GetFont();

	void	ToScreen(const Vector& world, Vector& scr);

	void	Line(int x0, int y0, int x1, int y1, unsigned color);
	void	Rect(int x0, int y0, int x1, int y1, unsigned color);
	void	OutlinedRect(int x0, int y0, int x1, int y1, unsigned color);
	void	Text(int x, int y, int align, unsigned color, const char* text, ...);

	
	inline void OutlinedOutlinedRect(int x0, int y0, int x1, int y1, unsigned color)
	{
		OutlinedRect(x0 - 1, y0 - 1, x1 + 2, y1 + 2, CC_BLACK);
		OutlinedRect(x0 + 1, y0 + 1, x1 - 2, y1 - 2, CC_BLACK);
		OutlinedRect(x0, y0, x1, y1, color);
	}
}