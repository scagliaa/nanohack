/*

  ______                                  
 |  ____|                                 
 | |__     ___    _ __   _ __ ___    ___ 
 |  __|   / _ \  | '__| | '_ ` _ \  / __|
 | |     | (_) | | |    | | | | | | \__ \
 |_|      \___/  |_|    |_| |_| |_| |___/


	Credits:
		NanoCat
*/

#pragma once

#include "main.h"

// #define	FRAME_95_TITLE

#define	FRAME_TITLE_TEXT	Color(255, 255, 255)
#define	FRAME_CONTENTS_TEXT	Color(255, 255, 255)
#define	FRAME_BACKGROUND	Color(255, 255, 255)
#define	FRAME_BORDER		Color(110, 200, 25)
#define	FRAME_INNER			Color(0, 0, 0)
#define	FRAME_OUTER			Color(0, 0, 0)
#define	FRAME_GEOMETRY		Color(0, 0, 0)
#define	FRAME_CROSS			Color(255, 255, 255)



#define	CHECKBOX_GRAY		Color(212, 208, 200)



#define	BUTTON_OVER			Color(220, 235, 250)
#define	BUTTON_OUT			Color(230, 230, 230)
#define	BUTTON_OVER_OUTLINE	Color(120, 165, 245)
#define	BUTTON_OUT_OUTLINE	Color(152, 152, 152)


/* === Non-user code === */









namespace forms
{
	extern int mousex;
	extern int mousey;

	struct Buttons
	{
		bool Left;
		bool LeftPrev;
		bool LeftClick;

		bool Right;
		bool RightPrev;
		bool RightClick;

		void Refresh()
		{
			LeftClick	= false;
			RightClick	= false;

			Left	= GetAsyncKeyState(VK_LBUTTON);
			Right	= GetAsyncKeyState(VK_RBUTTON);

			if (LeftPrev != Left)
				LeftPrev = LeftClick = Left;

			if (RightPrev != Right)
				RightPrev = RightClick = Right;
		}
	};

	extern Buttons buttons;

	class Color
	{
	public:
		int r, g, b;

		Color(int _r, int _g, int _b)
		{
			r = _r;
			g = _g;
			b = _b;
		}

		// conv to dx color system
		inline DWORD GetDX_ARGB()
		{
			return ((DWORD)((255<<24)|(((r)&0xFF)<<16)|(((g)&0xFF)<<8)|((b)&0xFF)));
		}

		// conv to my color system
		inline DWORD GetCustom()
		{
			return ((DWORD)((((r)&0xFF)<<24)|(((g)&0xFF)<<16)|(((b)&0xFF)<<8)|255));
		}
	};

	void Fn_DrawFilledRect(int x, int y, int w, int h, Color color);
	void Fn_DrawOutlinedRect(int x, int y, int w, int h, Color color);
	void Fn_DrawText(int x, int y, const char* text, Color color);
	void Fn_DrawLine(int x0, int y0, int x1, int y1, Color color);
	void Fn_GetScreenSize(int &w, int &h);
	void Fn_GetTextSize(const char* text, int &w, int &h);

	const char* ExplainKey(int vk);

	inline bool IsMouseOver(int x, int y, int w, int h)
	{
		return mousex >= x && mousex <= (x + w) && mousey >= y && mousey <= (y + h);
	}

	extern bool inputallowed;

	inline bool CanAcceptInput()
	{
		return inputallowed;
	}

	inline void DenyFutureInputThisFrame()
	{
		inputallowed = false;
	}

	inline void DenyFutureInput()
	{
		DenyFutureInputThisFrame();

		buttons.LeftClick	= false;
		buttons.RightClick	= false;
	}







	class F_BaseObject
	{
	public:
		F_BaseObject* Parent;
		
		int x, y;
		int w, h;

		bool visible;

		static std::vector<F_BaseObject*> elements;
		static bool IsValidElement(F_BaseObject* obj)
		{
			for (unsigned i = 0; i < elements.size(); i++)
			if (elements[i] = obj)
				return true;

			return false;
		}

		F_BaseObject()
		{
			SetParent(0);
			SetPos(0, 0);
			SetSize(0, 0);

			SetVisible();
			Register();
		}

		virtual ~F_BaseObject()
		{
			for (unsigned i = 0; i < elements.size(); i++)
			if (elements[i]->GetParent() == this)
				delete elements[i];
		}

		inline void Register()
		{
			elements.push_back(this);
		}
		
		inline void UnRegister()
		{
			for (unsigned i = 0; i < elements.size(); i++)
			if (elements[i] == this)
				elements.erase(elements.begin() + i);
		}

		inline void InvokeChildrenDrawing(int x, int y)
		{
			for (unsigned i = 0; i < elements.size(); i++)
			if (elements[i]->GetParent() == this)
			{
				int px, py, w, h;

				elements[i]->GetPos(px, py);
				elements[i]->GetSize(w, h);
				elements[i]->Paint(x + px, y + py, w, h);
			}
		}

		inline void InvokeChildrenThinking(int x, int y)
		{
			for (int i = elements.size() - 1; i > -1; --i)
			if (elements[i]->GetParent() == this)
			{
				int px, py, w, h;

				elements[i]->GetPos(px, py);
				elements[i]->GetSize(w, h);
				elements[i]->Think(x + px, y + py, w, h);
			}
		}

		inline void MakeTop()
		{
			for (unsigned i = 0; i < elements.size(); i++)
			if (elements[i] == this)
			{
				elements.erase(elements.begin() + i);
				elements.push_back(this);

				break;
			}
		}

		inline void SetParent(F_BaseObject* obj)
		{
			Parent = obj;

			for (unsigned i = 0; i < elements.size(); i++)
			if (elements[i] == obj)
				elements[i]->Attached(this);
		}

		inline F_BaseObject* GetParent()
		{
			return Parent;
		}

		inline void SetPos(int _x = -1, int _y = -1)
		{
			if (_x != -1)
				x = _x;

			if (_y != -1)
				y = _y;
		}

		inline void SetSize(int _w = 0, int _h = 0)
		{
			if (_w > 0)
				w = _w;

			if (_h > 0)
				h = _h;
		}

		inline void GetPos(int &_x, int &_y)
		{
			_x = x;
			_y = y;
		}

		inline void GetSize(int &_w, int &_h)
		{
			_w = w;
			_h = h;
		}

		inline void SetVisible(bool _visible = 1)
		{
			visible = _visible;
		}

		inline bool GetVisible()
		{
			return visible;
		}
		
		virtual void Paint(int x, int y, int w, int h)	{ };
		virtual void Think(int x, int y, int w, int h)	{ };
		virtual void Attached(F_BaseObject* obj)		{ };
	};

	/*
		 _           _          _ 
		| |         | |        | |
		| |     __ _| |__   ___| |
		| |    / _` | '_ \ / _ \ |
		| |___| (_| | |_) |  __/ |
		\_____/\__,_|_.__/ \___|_|

	*/

	class F_Label : public F_BaseObject
	{
	public:
		virtual void Paint(int x, int y, int w, int h)
		{
			if (!GetVisible())
				return;

			Fn_DrawText(x, y, text, FRAME_CONTENTS_TEXT);
		}

		inline void SetText(const char* _text)
		{
			if (!GetVisible())
				return;

			text = _text;
		}

		inline const char* GetText()
		{
			return text;
		}

		const char* text;
	};

	/*
		 _____         _     _               
		|_   _|       | |   | |              
		  | | _____  _| |_  | |__   _____  __
		  | |/ _ \ \/ / __| | '_ \ / _ \ \/ /
		  | |  __/>  <| |_  | |_) | (_) >  < 
		  \_/\___/_/\_\\__| |_.__/ \___/_/\_\

	*/

	class F_TextBox : public F_BaseObject
	{
	public:
		F_TextBox()
		{
			acceptinput = 0;
			filesystem	= 0;
			textptr		= 0;
			*entry		= 0;

			SetMaxLength();
		}

		virtual void Paint(int x, int y, int w, int h)
		{
			if (!GetVisible())
				return;

			if (IsMouseOver(x, y, w, h))
			{
				Fn_DrawFilledRect(x, y, w, h, BUTTON_OVER);
				Fn_DrawOutlinedRect(x, y, w, h, BUTTON_OVER_OUTLINE);
			}
			else
			{
				Fn_DrawFilledRect(x, y, w, h, BUTTON_OUT);
				Fn_DrawOutlinedRect(x, y, w, h, BUTTON_OUT_OUTLINE);
			}

			if (acceptinput)
			{
				int tw, th;
				Fn_GetTextSize(GetText(), tw, th);

				if ((GetTickCount() / 500) % 2)
					Fn_DrawLine(x + 3 + tw, y + 2, x + 3 + tw, y + h - 3, FRAME_GEOMETRY);
			}

			Fn_DrawText(x + 3, y + 3, GetText(), FRAME_CONTENTS_TEXT);
		}

		virtual void Think(int x, int y, int w, int h)
		{
			if (!GetVisible())
			{
				if (acceptinput)
				{
					acceptinput = 0;
					glob_acceptinput = 0;
				}

				return;
			}

			if (acceptinput)
			{
				bool shift = GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT);
				bool caps = GetKeyState(VK_CAPITAL);

				for (int i = 1; i <= 255; i++)
				if (GetAsyncKeyState(i) & 1)
				{
					char chr = MapVirtualKey(i, MAPVK_VK_TO_CHAR) & 255;

					if (!chr || (chr == '\t'))
						continue;

					if (wait4release)
						return;

					if (shift)
					{
						if (chr == '-') chr = '_';
						if (chr == '=') chr = '+';
						if (chr == '0') chr = ')';
						if (chr == '9') chr = '(';
						if (chr == '8') chr = '*';
						if (chr == '7') chr = '&';
						if (chr == '6') chr = '^';
						if (chr == '5') chr = '%';
						if (chr == '4') chr = '$';
						if (chr == '3') chr = '#';
						if (chr == '2') chr = '@';
						if (chr == '1') chr = '!';

						if (chr == ';') chr = ':';
						if (chr == '\'') chr = '"';
						if (chr == '/') chr = '?';
						if (chr == '.') chr = '>';
						if (chr == ',') chr = '<';
						if (chr == ']') chr = '}';
						if (chr == '[') chr = '{';
					}

					if (chr == '\r' || chr == '\n')
					{
						acceptinput = 0;
						glob_acceptinput = 0;
					}
					else if (chr == '\b')
						entry[max(strlen(entry) - 1, 0)] = 0;
					else if (strlen(entry) < textlimit)
					{
						if (filesystem ? (
								(chr != '<') &&
								(chr != '>') &&
								(chr != ':') &&
								(chr != '"') &&
								(chr != '/') &&
								(chr != '\\') &&
								(chr != '|') &&
								(chr != '?') &&
								(chr != '*')
							) : 1)
							entry[strlen(entry)] = (caps != shift) ? toupper(chr) : tolower(chr);
					}

					if (textptr)
						strcpy(textptr, entry);
				}

				wait4release = 0;
			}

			if (buttons.LeftClick)
			if (CanAcceptInput() && IsMouseOver(x, y, w, h))
			{
				if (!acceptinput && !glob_acceptinput)
				{
					wait4release = 1;
					acceptinput = 1;
					glob_acceptinput = 1;

					DenyFutureInputThisFrame();
				}
			}
			else if (acceptinput)
			{
				acceptinput = 0;
				glob_acceptinput = 0;
			}
		}

		inline const char* GetText()
		{
			return entry;
		}

		inline void SetText(const char* in)
		{
			strcpy(entry, in);
		}

		inline void NTFSReserved(bool deny = 0)
		{
			filesystem = deny;
		}

		inline void SetMaxLength(int len = 20)
		{
			textlimit = len;
		}

		inline void BindToVariable(char* variable)
		{
			textptr = variable;

			if (variable)
				SetText(variable);
		}
		
		int		textlimit;
		bool	filesystem;
		bool	acceptinput;
		bool	wait4release;
		char	entry[255];
		char*	textptr;

		static bool glob_acceptinput;
	};

	/*
		 _   __             _                   
		| | / /            | |                  
		| |/ /  ___ _   _  | |_ _ __ __ _ _ __  
		|    \ / _ \ | | | | __| '__/ _` | '_ \ 
		| |\  \  __/ |_| | | |_| | | (_| | |_) |
		\_| \_/\___|\__, |  \__|_|  \__,_| .__/ 
					 __/ |               | |    
					|___/                |_|    
	*/

	class F_KeyTrap : public F_BaseObject
	{
	public:
		F_KeyTrap()
		{
			active				= 0;
			wait_for_release	= 0;
		}

		virtual void Paint(int x, int y, int w, int h)
		{
			if (!GetVisible())
				return;

			if (IsMouseOver(x, y, w, h) || active)
			{
				Fn_DrawFilledRect(x, y, w, h, BUTTON_OVER);
				Fn_DrawOutlinedRect(x, y, w, h, BUTTON_OVER_OUTLINE);
			}
			else
			{
				Fn_DrawFilledRect(x, y, w, h, BUTTON_OUT);
				Fn_DrawOutlinedRect(x, y, w, h, BUTTON_OUT_OUTLINE);
			}

			if (active)
			{
				int tw, th;
				Fn_GetTextSize("Press a key", tw, th);
				Fn_DrawText(x + w / 2 - tw / 2, y + h / 2 - th / 2, "Press a key", FRAME_CONTENTS_TEXT);
			}
			else
			{
				const char* key = *variable ? ExplainKey(*variable) : "Unbound";

				int tw, th;
				Fn_GetTextSize(key, tw, th);
				Fn_DrawText(x + w / 2 - tw / 2, y + h / 2 - th / 2, key, FRAME_CONTENTS_TEXT);
			}
		}

		virtual void Think(int x, int y, int w, int h)
		{
			if (!GetVisible())
			{
				if (active)
				{
					active = false;
					glob_active = false;
				}

				return;
			}

			if (active)
			{
				for (int i = 1; i <= 255; i++)
				if ((GetAsyncKeyState(i) & 1) && (*ExplainKey(i) || i == VK_ESCAPE))
				{
					if (wait_for_release)
						return;

					*variable = (i == VK_ESCAPE) ? 0 : i;

					active = false;
					glob_active = false;

					DenyFutureInput();

					break;
				}

				wait_for_release = false;
			}

			if (CanAcceptInput() && IsMouseOver(x, y, w, h) && buttons.LeftClick)
			{
				if (!active && !glob_active)
				{
					glob_active = true;
					active = true;
					wait_for_release = true;

					DenyFutureInput();
				}
			}
		}

		inline void SetVariable(int* v)
		{
			variable = v;
		}

		int* variable;
		bool active;
		bool wait_for_release;

		static bool glob_active;
	};

	/*
		 _____ _ _     _           
		/  ___| (_)   | |          
		\ `--.| |_  __| | ___ _ __ 
		 `--. \ | |/ _` |/ _ \ '__|
		/\__/ / | | (_| |  __/ |   
		\____/|_|_|\__,_|\___|_|   

	*/

	class F_Slider : public F_BaseObject
	{
	public:
		virtual void Paint(int x, int y, int w, int h)
		{
			Fn_DrawText(x, y, text, FRAME_CONTENTS_TEXT);

			char value[64];
			int tw, th;
			sprintf(value, "%i", *variable);

			Fn_GetTextSize(value, tw, th);
			Fn_DrawText(x + w - tw, y, value, FRAME_CONTENTS_TEXT);

			Fn_DrawFilledRect(x, y + 23, w, 4, BUTTON_OUT);
			Fn_DrawOutlinedRect(x, y + 23, w, 4, BUTTON_OUT_OUTLINE);

			bool over = IsMouseOver(x, y + 15, w, 20);

			int offs = (int)(((float)(clamp(*variable, mn, mx) - mn) / (float)(mx - mn)) * ((float)w - 10.f));

			Fn_DrawFilledRect(x + offs, y + 15, 10, 20, over ? BUTTON_OVER : BUTTON_OUT);
			Fn_DrawOutlinedRect(x + offs, y + 15, 10, 20, over ? BUTTON_OVER_OUTLINE : BUTTON_OUT_OUTLINE);
		}

		virtual void Think(int x, int y, int w, int h)
		{
			bool over = IsMouseOver(x, y + 15, w, 20);

			if (buttons.Left && over)
			{
				*variable = clamp(mn + (int)(((float)(mousex - x - 5) / ((float)w - 10.f)) * (float)(mx - mn)), mn, mx);
				DenyFutureInputThisFrame();
			}
		}

		inline void SetText(const char* _text)
		{
			text = _text;
		}

		inline void SetRange(int _min, int _max)
		{
			mn = _min;
			mx = _max;
		}

		inline void SetVariable(int* v)
		{
			variable = v;
		}

		const char* text;
		int mn, mx;
		int* variable;
	};

	/*
		 _____                 _           _               
		/  __ \               | |         | |              
		| /  \/ ___  _ __ ___ | |__   ___ | |__   _____  __
		| |    / _ \| '_ ` _ \| '_ \ / _ \| '_ \ / _ \ \/ /
		| \__/\ (_) | | | | | | |_) | (_) | |_) | (_) >  < 
		 \____/\___/|_| |_| |_|_.__/ \___/|_.__/ \___/_/\_\

	*/

	class F_ComboBox : public F_BaseObject
	{
	public:
		F_ComboBox()
		{
			item_selected = 0;
			item_selected_e = -1;
			variable = 0;
			open = 0;

			cbSelect = 0;
			cbOpen = 0;

			Clear();
		}

		virtual void Paint(int x, int y, int w, int h)
		{
			SetSize(0, 15);
			
			int tw, th;

			if (open)
			{
				Fn_DrawFilledRect(x, y + h - 1, w, items.size() * h,	BUTTON_OUT);
				Fn_DrawOutlinedRect(x, y + h - 1, w, items.size() * h,	BUTTON_OUT_OUTLINE);

				for (unsigned i = 0; i < items.size(); i++)
				{
					if (IsMouseOver(x, y + h * (i + 1) - 1, w, h))
					{
						Fn_DrawFilledRect(x, y + h * (i + 1) - 1, w, h,		BUTTON_OVER);
						Fn_DrawOutlinedRect(x, y + h * (i + 1) - 1, w, h,	BUTTON_OVER_OUTLINE);
					}

					Fn_GetTextSize(items[i], tw, th);
					Fn_DrawText(x + 5, y + h * (i + 1) + h / 2 - th / 2, items[i], FRAME_CONTENTS_TEXT);
				}
			}

			if (IsMouseOver(x, y, w, h) || open)
			{
				Fn_DrawFilledRect(x, y, w, h,	BUTTON_OVER);
				Fn_DrawOutlinedRect(x, y, w, h,	BUTTON_OVER_OUTLINE);

				Fn_DrawLine(x + w - h, y + 3, x + w - h, y + h - 4, BUTTON_OVER_OUTLINE);
			}
			else
			{
				Fn_DrawFilledRect(x, y, w, h,	BUTTON_OUT);
				Fn_DrawOutlinedRect(x, y, w, h,	BUTTON_OUT_OUTLINE);

				Fn_DrawLine(x + w - h, y + 3, x + w - h, y + h - 4, BUTTON_OUT_OUTLINE);
			}

			item_selected = clamp(item_selected, 0, items.size() - 1);

			Fn_GetTextSize(items[item_selected], tw, th);
			Fn_DrawText(x + 5, y + h / 2 - th / 2 + 1, items[item_selected], FRAME_CONTENTS_TEXT);
		}

		virtual void Think(int x, int y, int w, int h)
		{
			if (CanAcceptInput() && IsMouseOver(x, y, w, h) && buttons.LeftClick)
			{
				open = !open;
				MakeTop();
				DenyFutureInput();

				if (open && cbOpen)
					((void (*)(void*))cbOpen)(this);
			}

			if (open)
			{
				if (!IsMouseOver(x, y, w, h + h * items.size()))
					open = 0;

				if (CanAcceptInput() && buttons.LeftClick)
				for (unsigned i = 0; i < items.size(); i++)
				{
					if (IsMouseOver(x, y + h * (i + 1) - 1, w, h))
					{
						DenyFutureInputThisFrame();

						item_selected = item_selected_e = i;
						open = 0;

						if (cbSelect)
						{
							int ov = ((int (*)(void*))cbSelect)(this);

							if (variable)
								*variable = ov;
						}
						else if (variable)
							*variable = i;

						break;
					}
				}
			}
			else if (variable && item_selected_e != *variable && !cbSelect)
			{
				item_selected = clamp(*variable, 0, items.size() - 1);
				item_selected_e = *variable;
			}
		}



		inline void Clear()
		{
			for (unsigned i = 0; i < items.size(); i++)
				free((void*)items[i]);

			items.clear();
		}

		inline void AddItems(int n, ...)
		{
			va_list v;
			va_start(v, n);

			for (int i = 0; i < n; i++)
				AddItem(va_arg(v, char*));

			va_end(v);
		}

		inline void SetVariable(int* v)
		{
			variable = v;
		}

		inline void AddItem(const char* item)
		{
			char* i = (char*)malloc(strlen(item) + 1);
			strcpy(i, item);

			items.push_back(i);
		}

		void* cbOpen;
		void* cbSelect;

		bool open;
		int* variable;
		int item_selected;
		int item_selected_e;
		std::vector<const char*> items;
	};

	/*
		 _____ _               _    _               
		/  __ \ |             | |  | |              
		| /  \/ |__   ___  ___| | _| |__   _____  __
		| |   | '_ \ / _ \/ __| |/ / '_ \ / _ \ \/ /
		| \__/\ | | |  __/ (__|   <| |_) | (_) >  < 
		 \____/_| |_|\___|\___|_|\_\_.__/ \___/_/\_\

	*/

	class F_Checkbox : public F_BaseObject
	{
	public:
		virtual void Paint(int x, int y, int w, int h)
		{
			if (!GetVisible())
				return;

			Fn_DrawOutlinedRect(x, y, 11, 11, FRAME_GEOMETRY);

			int tw, th;
			Fn_GetTextSize(label, tw, th);

			if (IsMouseOver(x, y, 17 + tw, 11))
				Fn_DrawFilledRect(x + 2, y + 2, 7, 7, CHECKBOX_GRAY);

			Fn_DrawText(x + 15, y, label, FRAME_CONTENTS_TEXT);

			if (GetChecked())
			{
				Fn_DrawLine(x + 6, y + 8, x + 8, y + 3, FRAME_GEOMETRY);
				Fn_DrawLine(x + 2, y + 6, x + 6, y + 8, FRAME_GEOMETRY);

				y--;

				Fn_DrawLine(x + 6, y + 8, x + 7, y + 3, FRAME_GEOMETRY);
				Fn_DrawLine(x + 2, y + 6, x + 6, y + 8, FRAME_GEOMETRY);
			}
		}

		virtual void Think(int x, int y, int w, int h)
		{
			if (variable)
				SetChecked(*variable);

			if (!GetVisible())
				return;

			int tw, th;
			Fn_GetTextSize(label, tw, th);

			if (IsMouseOver(x, y, 17 + tw, 11))
			if (CanAcceptInput() && buttons.LeftClick)
			{
				SetChecked(!GetChecked());

				if (variable)
					*variable = GetChecked();

				DenyFutureInput();
			}
		}

		inline void SetVariable(int* v)
		{
			variable = v;
		}

		inline void SetText(const char* _text)
		{
			label = _text;
		}

		inline void SetChecked(bool _checked = 0)
		{
			checked = _checked;
		}

		inline bool GetChecked()
		{
			return checked;
		}

		const char* label;
		int* variable;
		bool checked;
	};

	/*
		 _____       _ _ _   _            
		/  ___|     | (_) | | |           
		\ `--. _ __ | |_| |_| |_ ___ _ __ 
		 `--. \ '_ \| | | __| __/ _ \ '__|
		/\__/ / |_) | | | |_| ||  __/ |   
		\____/| .__/|_|_|\__|\__\___|_|   
			  | |                         
			  |_|                         

	*/

	class F_Splitter : public F_BaseObject
	{
	public:
		virtual void Paint(int x, int y, int w, int h)
		{
			int tw, th;
			Fn_GetTextSize(text, tw, th);

			Fn_DrawText(x + w - tw - 10, y, text, FRAME_CONTENTS_TEXT);
			Fn_DrawLine(x, y + th / 2, x + w - tw - 15, y + th / 2, FRAME_GEOMETRY);
		}

		inline void SetText(const char* _text)
		{
			text = _text;
		}

		const char* text;
	};

	/*
		______                _ 
		| ___ \              | |
		| |_/ /_ _ _ __   ___| |
		|  __/ _` | '_ \ / _ \ |
		| | | (_| | | | |  __/ |
		\_|  \__,_|_| |_|\___|_|

	*/

	class F_Panel : public F_BaseObject
	{
	public:
		F_Panel()
		{
			fill_x		= 0;
			fill_vert	= 0;
			autofill	= 0;
		}

		virtual void Paint(int x, int y, int w, int h)
		{
			if (!GetVisible())
				return;

			InvokeChildrenDrawing(x, y);
		}

		virtual void Think(int x, int y, int w, int h)
		{
			if (!GetVisible())
				return;

			InvokeChildrenThinking(x, y);
		}

		virtual void Attached(F_BaseObject* obj)
		{
			if (!autofill)
				return;

			int pw, ph;
			obj->GetSize(pw, ph);

			if (ph < 1)
				ph = 11;

			ph += 5;

			obj->SetPos(fill_x + 5, fill_vert + 5);

			fill_vert += ph;
		}

		inline void Skip()
		{
			fill_vert += 15;
		}

		inline void AutoFill(bool fill = 0)
		{
			fill_vert = 0;
			autofill = fill;
		}

		inline void AddSplitter(const char* title)
		{
			F_Splitter* splt = new F_Splitter();

			splt->SetText(title);
			splt->SetSize(w, 11);
			splt->SetParent(this);
		}

		int fill_x;
		int fill_vert;
		bool autofill;
	};

	/*
		______                _     _           _     _ 
		| ___ \              | |   | |         | |   | |          
		| |_/ /_ _ _ __   ___| |   | |__   ___ | | __| | ___ _ __ 
		|  __/ _` | '_ \ / _ \ |   | '_ \ / _ \| |/ _` |/ _ \ '__|
		| | | (_| | | | |  __/ |   | | | | (_) | | (_| |  __/ |   
		\_|  \__,_|_| |_|\___|_|   |_| |_|\___/|_|\__,_|\___|_|   

	*/

	class F_PanelHolder : public F_BaseObject
	{
	public:
		F_PanelHolder()
		{
			attachments.clear();
			selected_panel = 0;
		}

		class Attachment
		{
		public:
			Attachment(const char* _pname, F_BaseObject* _panel)
			{
				pname = _pname;
				panel = _panel;
			}

			const char*		pname;
			F_BaseObject*	panel;
		};

		virtual void Paint(int x, int y, int w, int h)
		{
			int tab_w = w / attachments.size();

			Fn_DrawLine(x, y + 20, x + selected_panel * tab_w, y + 20, BUTTON_OUT_OUTLINE);
			Fn_DrawLine(x + selected_panel * tab_w + tab_w, y + 20, x + w, y + 20, BUTTON_OUT_OUTLINE);
			
			Fn_DrawLine(x + selected_panel * tab_w, y + 2, x + selected_panel * tab_w + tab_w - 1, y + 2, BUTTON_OUT_OUTLINE);
			Fn_DrawLine(x + selected_panel * tab_w, y + 2, x + selected_panel * tab_w, y + 20, BUTTON_OUT_OUTLINE);
			Fn_DrawLine(x + selected_panel * tab_w + tab_w - 1, y + 2, x + selected_panel * tab_w + tab_w - 1, y + 20, BUTTON_OUT_OUTLINE);

			int tw, th;
			Fn_GetTextSize(attachments[selected_panel]->pname, tw, th);

			Fn_DrawText(x + selected_panel * tab_w + tab_w / 2 - tw / 2, y + 12 - th / 2, attachments[selected_panel]->pname, FRAME_CONTENTS_TEXT);

			attachments[selected_panel]->panel->GetSize(tw, th);
			attachments[selected_panel]->panel->Paint(x, y + 21, tw, th);

			for (unsigned i = 0; i < attachments.size(); i++)
			{
				if (i == selected_panel)
					continue;

				Fn_GetTextSize(attachments[i]->pname, tw, th);

				if (IsMouseOver(x + i * tab_w, y, tab_w, 20))
				{
					Fn_DrawFilledRect(x + i * tab_w, y + 6, tab_w, 14, BUTTON_OVER);
					Fn_DrawLine(x + i * tab_w, y + 6, x + i * tab_w + tab_w - 1, y + 6, BUTTON_OVER_OUTLINE);
					Fn_DrawLine(x + i * tab_w, y + 6, x + i * tab_w, y + 20, BUTTON_OVER_OUTLINE);
					Fn_DrawLine(x + i * tab_w + tab_w - 1, y + 6, x + i * tab_w + tab_w - 1, y + 20, BUTTON_OVER_OUTLINE);
				}
				else
				{
					Fn_DrawFilledRect(x + i * tab_w, y + 6, tab_w, 14, BUTTON_OUT);
					Fn_DrawLine(x + i * tab_w, y + 6, x + i * tab_w + tab_w - 1, y + 6, BUTTON_OUT_OUTLINE);
					Fn_DrawLine(x + i * tab_w, y + 6, x + i * tab_w, y + 20, BUTTON_OUT_OUTLINE);
					Fn_DrawLine(x + i * tab_w + tab_w - 1, y + 6, x + i * tab_w + tab_w - 1, y + 20, BUTTON_OUT_OUTLINE);
				}

				Fn_DrawText(x + i * tab_w + tab_w / 2 - tw / 2, y + 14 - th / 2, attachments[i]->pname, FRAME_CONTENTS_TEXT);
			}
		}

		virtual void Think(int x, int y, int w, int h)
		{
			int tab_w = w / attachments.size();

			for (unsigned i = 0; i < attachments.size(); i++)
			{
				int tw, th;
				Fn_GetTextSize(attachments[i]->pname, tw, th);
				
				if (i == selected_panel)
				{
					int _w, _h;

					attachments[i]->panel->GetSize(_w, _h);
					attachments[i]->panel->Think(x, y + 21, _w, _h); 
				}
				else if (CanAcceptInput() && IsMouseOver(x + i * tab_w, y, tab_w, 20) && buttons.LeftClick)
				{
					selected_panel = i;
					DenyFutureInput();
				}
			}
		}

		inline void AddPanel(F_BaseObject* panel, const char* title)
		{
			panel->SetParent(this);
			panel->SetSize(w, h - 20);
			attachments.push_back(new Attachment(title, panel));
		}

		int selected_panel;
		std::vector<Attachment*> attachments;
	};

	/*
		______       _   _              
		| ___ \     | | | |             
		| |_/ /_   _| |_| |_ ___  _ __  
		| ___ \ | | | __| __/ _ \| '_ \ 
		| |_/ / |_| | |_| || (_) | | | |
		\____/ \__,_|\__|\__\___/|_| |_|                                

	*/

	class F_Button : public F_BaseObject
	{
	public:
		typedef void (*OnClickFn)(F_BaseObject* self);

		virtual void Paint(int x, int y, int w, int h)
		{
			if (!GetVisible())
				return;

			if (IsMouseOver(x, y, w, h))
			{
				Fn_DrawFilledRect(x, y, w, h, BUTTON_OVER);
				Fn_DrawOutlinedRect(x, y, w, h, BUTTON_OVER_OUTLINE);
			}
			else
			{
				Fn_DrawFilledRect(x, y, w, h, BUTTON_OUT);
				Fn_DrawOutlinedRect(x, y, w, h, BUTTON_OUT_OUTLINE);
			}

			int tw, th;
			Fn_GetTextSize(label, tw, th);
			Fn_DrawText(x + w / 2 - tw / 2, y + h / 2 - th / 2, label, FRAME_CONTENTS_TEXT);
		}

		virtual void Think(int x, int y, int w, int h)
		{
			if (!GetVisible())
				return;

			if (CanAcceptInput() && fn && IsMouseOver(x, y, w, h) && buttons.LeftClick)
			{
				fn(this);
				DenyFutureInput();
			}
		}

		inline void SetOnClickFn(OnClickFn _fn = 0)
		{
			fn = _fn;
		}

		inline void SetText(const char* _text)
		{
			label = _text;
		}

		OnClickFn fn;
		const char* label;
	};

	/*
		______                   
		|  ___|
		| |_ ___  _ __ _ __ ___  
		|  _/ _ \| '__| '_ ` _ \ 
		| || (_) | |  | | | | | |
		\_| \___/|_|  |_| |_| |_|

	*/

	class F_Form : public F_BaseObject
	{
	public:
		F_Form()
		{
			SetTitle("");
			SetSizeable();

			resize	= 0;
			drag	= 0;
			drag_x	= 0;
			drag_y	= 0;
		}

		virtual void Paint(int x, int y, int w, int h)
		{
			if (!GetVisible())
				return;

			Fn_DrawFilledRect(x + 4, y + 20, w - 8, h - 24, FRAME_BACKGROUND);

			Fn_DrawFilledRect(x, y, w, 20,					FRAME_BORDER); // top
			Fn_DrawFilledRect(x, y + h - 4, w, 4,			FRAME_BORDER); // bottom
			Fn_DrawFilledRect(x, y + 20, 4, h - 24,			FRAME_BORDER); // left
			Fn_DrawFilledRect(x + w - 4, y + 20, 4, h - 24,	FRAME_BORDER); // right

			Fn_DrawOutlinedRect(x, y, w, h, FRAME_OUTER);
			Fn_DrawOutlinedRect(x + 4, y + 20, w - 8, h - 24, FRAME_INNER);

			Fn_DrawFilledRect(x + w - 19, y + 3, 15, 15, IsMouseOver(x + w - 19, y + 3, 15, 15) ? BUTTON_OVER : BUTTON_OUT);
			Fn_DrawOutlinedRect(x + w - 19, y + 3, 15, 15, FRAME_GEOMETRY);

			Fn_DrawLine(x + w - 16, y + 6, x + w - 8, y + 15, FRAME_GEOMETRY);
			Fn_DrawLine(x + w - 15, y + 6, x + w - 7, y + 15, FRAME_GEOMETRY);
			Fn_DrawLine(x + w - 9, y + 6, x + w - 17, y + 15, FRAME_GEOMETRY);
			Fn_DrawLine(x + w - 8, y + 6, x + w - 16, y + 15, FRAME_GEOMETRY);

			if (title)
			{
				int tw, th;
				Fn_GetTextSize(title, tw, th);

#ifdef FRAME_95_TITLE
				Fn_DrawText(x + 6, y + 11 - th / 2, title, FRAME_TITLE_TEXT);
#else
				Fn_DrawText(x + w / 2 - tw / 2, y + 11 - th / 2, title, FRAME_TITLE_TEXT);
#endif
			}

			if (sizeable)
			{
				Fn_DrawLine(x + w - 10, y + h - 7, x + w - 7, y + h - 10, FRAME_GEOMETRY);
				Fn_DrawLine(x + w - 13, y + h - 7, x + w - 7, y + h - 13, FRAME_GEOMETRY);
				Fn_DrawLine(x + w - 16, y + h - 7, x + w - 7, y + h - 16, FRAME_GEOMETRY);
			}
			
			InvokeChildrenDrawing(x + 5, y + 21);
		}

		virtual void Think(int x, int y, int w, int h)
		{
			SetPos((x < 0) ? 0 : -1, (y < 0) ? 0 : -1);

			if (!GetVisible())
			{
				if (drag)
				{
					drag = false;
					glob_drag = false;
				}

				return;
			}

			if (IsMouseOver(x + w - 19, y + 3, 15, 15))
			{
				if (CanAcceptInput() && buttons.LeftClick)
				{
					SetVisible(0);
					DenyFutureInput();
				}
			}
			else
			{
				if (IsMouseOver(x, y, w, 20) && CanAcceptInput() && !resize && !drag && !glob_drag && buttons.LeftClick)
				{
					drag = glob_drag = true;

					drag_x = mousex - x;
					drag_y = mousey - y;

					MakeTop();
					DenyFutureInputThisFrame();
				}
				else if (sizeable && IsMouseOver(x + w - 15, y + h - 15, 15, 15) && CanAcceptInput() && !resize && !drag && !glob_drag && buttons.LeftClick)
				{
					resize = glob_drag = true;

					MakeTop();
					DenyFutureInputThisFrame();
				}
			}

			if (resize)
			if (buttons.Left && sizeable)
				SetSize(max(mousex - x, 55), max(mousey - y, 60));
			else
			{
				resize = false;
				glob_drag = false;
			}
			
			if (drag)
			if (buttons.Left)
				SetPos(mousex - drag_x, mousey - drag_y);
			else
			{
				drag = false;
				glob_drag = false;
			}
			else
			{
				int sw, sh;
				Fn_GetScreenSize(sw, sh);

				x = clamp(x, 0, sw - w);
				y = clamp(y, 0, sh - h);
			}
			
			InvokeChildrenThinking(x + 5, y + 21);

			if (!glob_drag && CanAcceptInput() && buttons.LeftClick && IsMouseOver(x, y, w, h))
			{
				MakeTop();
				DenyFutureInputThisFrame();
			}
		}

		inline void SetTitle(const char* _title = 0)
		{
			title = _title;
		}

		inline void SetSizeable(bool _sizeable = 0)
		{
			sizeable = _sizeable;
		}

		inline void Center()
		{
			int sw, sh;
			Fn_GetScreenSize(sw, sh);
			SetPos(sw / 2 - w / 2, sh / 2 - h / 2);
		}

		static bool glob_drag;

		const char* title;
		bool sizeable;
		bool resize;
		bool drag;

		int drag_x;
		int drag_y;
	};

	void Render();
	bool Init();
}