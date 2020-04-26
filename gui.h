#pragma once
#include "main.h"

class CBaseEntity;
class RecvProxyData;

namespace gui
{
	void PaintSpectators(forms::F_BaseObject*, int x, int y, int w, int h);
	void PaintRadar(forms::F_BaseObject*, int x, int y, int w, int h);

	void InitForms();
	void RenderInGameOverlay();

	void Proxy_FlashSmoke(const RecvProxyData*, void*, float*);
	void Proxy_Jarate(const RecvProxyData*, CBaseEntity*, int*);
}