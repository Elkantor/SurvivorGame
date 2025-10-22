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

void convertUint16ToFloat3Array(u16 _input, f32 _output[3])
{
	_output[0] = (f32)(_input & 0xFF);
	_output[1] = (f32)((_input >> 8) & 0xFF);
	_output[2] = (f32)((_input >> 4) & 0xFF);
}

void ShaderFlatColorUpdate(ShaderFlatColor* _shaderFlat, const u16 _id)
{
	f32 color[3] = { 0.f };
	convertUint16ToFloat3Array(_id, color);
	SetShaderValue(_shaderFlat->m_shader, _shaderFlat->m_locColor, &color, SHADER_UNIFORM_VEC3);
}
