#pragma once

#include "../raylib.h"
#include "../types.c"

typedef struct ShaderFlatColor
{
	Shader m_shader;
	i32 m_locColor;
} ShaderFlatColor;

void ShaderFlatColorInit(ShaderFlatColor* _shaderFlat)
{
	_shaderFlat->m_shader = LoadShader(NULL, "./resources/shaders/flatColor.fs");
	_shaderFlat->m_locColor = GetShaderLocation(_shaderFlat->m_shader, "color");
}

void ShaderFlatColorUpdate(ShaderFlatColor* _shaderFlat, const u16 _id)
{
	f32 color[3] = { 0.f };
	convertUint16ToFloat3Array(_id, color);
	color[0] /= 255.f;
	color[1] /= 255.f;
	color[2] /= 255.f;
	SetShaderValue(_shaderFlat->m_shader, _shaderFlat->m_locColor, &color, SHADER_UNIFORM_VEC3);
}
