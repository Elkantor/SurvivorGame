#pragma once

#include "../types.c"
#include "../raylib.h"

typedef struct MatCap
{
	Shader m_shader;
	Texture m_texture;
	Texture m_texture2;
	i32 m_viewEyeLoc;
	i32 m_textureLoc;
	i32 m_brightnessLoc;
	i32 m_contrastLoc;
	i32 m_rimPowerLoc;
	i32 m_rimIntensityLoc;
	i32 m_rimColorLoc;
} MatCap;

void MatCapInit(MatCap* _matcap)
{
	_matcap->m_shader = LoadShader("resources/shaders/vs_matcap.glsl", "resources/shaders/fs_matcap.glsl");
	_matcap->m_texture = LoadTexture("resources/textures/matcaps/512px/greyHighContrast.png");
	_matcap->m_texture2 = LoadTexture("resources/textures/matcaps/512px/darkPurple.png");
	_matcap->m_viewEyeLoc = GetShaderLocation(_matcap->m_shader, "viewEye");
	_matcap->m_textureLoc = GetShaderLocation(_matcap->m_shader, "matcapTexture");
	_matcap->m_brightnessLoc = GetShaderLocation(_matcap->m_shader, "brightness");
	_matcap->m_contrastLoc = GetShaderLocation(_matcap->m_shader, "contrast");
	_matcap->m_rimPowerLoc = GetShaderLocation(_matcap->m_shader, "rimPower");
	_matcap->m_rimIntensityLoc = GetShaderLocation(_matcap->m_shader, "rimIntensity");
	_matcap->m_rimColorLoc = GetShaderLocation(_matcap->m_shader, "rimColor");

}

void MatCapUpdate(MatCap* _matcap, const f32 _brightness, const f32 _contrast, const f32 _rimPower, const f32 _rimIntensity, const Color _rimColor)
{
	SetShaderValue(_matcap->m_shader, _matcap->m_brightnessLoc, &_brightness, SHADER_UNIFORM_FLOAT);
	SetShaderValue(_matcap->m_shader, _matcap->m_contrastLoc, &_contrast, SHADER_UNIFORM_FLOAT);
	SetShaderValue(_matcap->m_shader, _matcap->m_rimPowerLoc, &_rimPower, SHADER_UNIFORM_FLOAT);
	SetShaderValue(_matcap->m_shader, _matcap->m_rimIntensityLoc, &_rimIntensity, SHADER_UNIFORM_FLOAT);

	f32 rimColor[3] = { _rimColor.r / 255.f, _rimColor.g / 255.f, _rimColor.b / 255.f };
	SetShaderValueV(_matcap->m_shader, _matcap->m_rimColorLoc, rimColor, SHADER_UNIFORM_VEC3, 1);
}