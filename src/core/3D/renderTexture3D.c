#pragma once

#include "../types.c"
#include "../raylib.h"

typedef struct RenderTexture3D
{
	RenderTexture m_texture;
} RenderTexture3D;

void RenderTexture3DInit(RenderTexture3D* _renderTexture3D)
{
	_renderTexture3D->m_texture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
}

void RenderTexture3DClose(RenderTexture3D* _renderTexture3D)
{
	UnloadRenderTexture(_renderTexture3D->m_texture);
}

void RenderTexture3DUpdate(RenderTexture3D* _renderTexture3D)
{
	const bool screenWidthChanged = _renderTexture3D->m_texture.texture.width != GetScreenWidth();
	const bool screenHeightChanged = _renderTexture3D->m_texture.texture.height != GetScreenHeight();
	const bool screenSizeChanged = screenHeightChanged || screenWidthChanged;

	if (screenSizeChanged == false)
	{
		return;
	}

	RenderTexture3DClose(_renderTexture3D);
	RenderTexture3DInit(_renderTexture3D);
}