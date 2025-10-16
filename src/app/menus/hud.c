#pragma once

#include "../../includes.c"

typedef struct HUD
{
	f32 m_fontSize;
} HUD;

void HUDInit(HUD* _hud)
{
	_hud->m_fontSize = 24;
}

void HUDUpdate(HUD* _hud, const f32 _dt)
{

}

void HUDRender(HUD* _hud)
{

}