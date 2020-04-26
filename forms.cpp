#include "main.h"

/* User code goes below */

void forms::Fn_DrawFilledRect(int x, int y, int w, int h, forms::Color color)
{
	draw::Rect(x, y, w, h, color.GetCustom());
}

void forms::Fn_DrawOutlinedRect(int x, int y, int w, int h, forms::Color color)
{
	draw::OutlinedRect(x, y, w, h, color.GetCustom());
}

void forms::Fn_DrawText(int x, int y, const char* text, forms::Color color)
{
	draw::Text(x, y, 0, color.GetCustom(), text);
}

void forms::Fn_DrawLine(int x0, int y0, int x1, int y1, forms::Color color)
{
	draw::Line(x0, y0, x1, y1, color.GetCustom());
}

extern IEngine* engine;
extern ISurface* surface;

void forms::Fn_GetTextSize(const char* text, int &w, int &h)
{
	wchar_t wstr[128];
	MultiByteToWideChar(CP_UTF8, 0, text, -1, wstr, 128);

	surface->GetTextSize(draw::GetFont(), wstr, w, h);
}

void forms::Fn_GetScreenSize(int &w, int &h)
{
	engine->GetScreenSize(w, h);
}

bool forms::Init()
{
	static bool once = false;

	if (once)
		return false;

	once = true;

	F_BaseObject::elements.empty();
	
	return true;
}











/* User code ends here */














std::vector<forms::F_BaseObject*> forms::F_BaseObject::elements;

int forms::mousex;
int forms::mousey;

forms::Buttons forms::buttons;

bool forms::F_TextBox::glob_acceptinput = 0;
bool forms::F_KeyTrap::glob_active		= 0;
bool forms::F_Form::glob_drag			= 0;
bool forms::inputallowed				= 0;

const char* forms::ExplainKey(int vk)
{
	static char explaination[16] = {0};

#define	DenyVK(_key) if (vk == _key) return explaination

	DenyVK(VK_INSERT);

	if (char exp = MapVirtualKey(vk, MAPVK_VK_TO_CHAR) & 255)
		if (exp == 32)
			return "Space";
		else if (exp == '\t')
			return "Tab";
		else if (exp == '\b')
			return "Backspace";
		else if (exp == '\n' || exp == '\r')
			return "Enter";
		else
			sprintf(explaination, "%c", exp);
	else
	{
		switch (vk)
		{
		case VK_LBUTTON:	return "LMB";
		case VK_RBUTTON:	return "RMB";
		case VK_MBUTTON:	return "Wheel";
		case VK_XBUTTON1:	return "X1";
		case VK_XBUTTON2:	return "X2";
		case VK_INSERT:		return "Ins";
		case VK_DELETE:		return "Del";
		case VK_LSHIFT:		return "L Shift";
		case VK_RSHIFT:		return "R Shift";
		case VK_LMENU:		return "L Alt";
		case VK_RMENU:		return "R Alt";
		case VK_LCONTROL:	return "L Ctrl";
		case VK_RCONTROL:	return "R Ctrl";
		}
	}

	return explaination;
}

void forms::Render()
{
	static HWND Valve001 = FindWindow("Valve001", NULL);
	RECT window;

	if (inputallowed = (GetActiveWindow() == Valve001 && GetWindowRect(Valve001, &window)))
	{
		POINT p;
		GetCursorPos(&p);

		mousex = p.x - window.left;
		mousey = p.y - window.top;

		if (chk(GetWindowLong(Valve001, GWL_STYLE), WS_CAPTION))
		{
			static NONCLIENTMETRICS ncm = {0};

			if (!ncm.cbSize)
				SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize = sizeof(NONCLIENTMETRICS), &ncm, 0);

			mousex -= ncm.iBorderWidth;
			mousey -= ncm.iCaptionHeight;
		}
	}
	else
		mousex = mousey = 0;

	if (forms::F_KeyTrap::glob_active)
		inputallowed = false;

	buttons.Refresh();

	for (unsigned i = 0; i < F_BaseObject::elements.size(); i++)
	{
		F_BaseObject* obj = F_BaseObject::elements[i];

		if (!obj)
			continue;
		
		if (obj->GetParent()) // children should be handled by their parents
			continue;

		int x, y, w, h;

		obj->GetPos(x, y);
		obj->GetSize(w, h);
		
		F_BaseObject::elements[i]->Paint(x, y, w, h);
	}
	
	// TODO: use separate thread for controls running at 20 fps

	for (int i = F_BaseObject::elements.size() - 1; i > -1; --i)
	{
		F_BaseObject* obj = F_BaseObject::elements[i];
		
		if (!obj)
			continue;

		if (obj->GetParent()) // children should be handled by their parents
			continue;

		int x, y, w, h;

		obj->GetPos(x, y);
		obj->GetSize(w, h);

		F_BaseObject::elements[i]->Think(x, y, w, h);
	}
}