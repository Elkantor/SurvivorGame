#pragma once

#include "../types.c"
#include "../raylib.h"

typedef struct MatCap
{
	Shader m_shader;
	Texture m_texture;
	i32 m_textureLoc;
	i32 m_brightnessLoc;
	i32 m_contrastLoc;
} MatCap;

void MatCapInit(MatCap* _matcap)
{
	_matcap->m_shader = LoadShader("resources/shaders/vs_matcap.glsl", "resources/shaders/fs_matcap.glsl");
	_matcap->m_texture = LoadTexture("resources/textures/matcaps/512px/simpleLightGrayDrawing.png");
	_matcap->m_textureLoc = GetShaderLocation(_matcap->m_shader, "matcapTexture");
	_matcap->m_brightnessLoc = GetShaderLocation(_matcap->m_shader, "brightness");
	_matcap->m_contrastLoc = GetShaderLocation(_matcap->m_shader, "contrast");
}

void MatCapUpdate(MatCap* _matcap, const f32 _brightness, const f32 _contrast)
{
	SetShaderValue(_matcap->m_shader, _matcap->m_brightnessLoc, &_brightness, SHADER_UNIFORM_FLOAT);
	SetShaderValue(_matcap->m_shader, _matcap->m_contrastLoc, &_contrast, SHADER_UNIFORM_FLOAT);
}