#pragma once

#include "../raylib.h"
#include "../types.c"

typedef struct ShaderBasicColor
{
	Shader m_shader;
	i32 m_specularity;
	i32 m_glosiness;
	i32 m_camClipNear;
	i32 m_camClipFar;
} ShaderBasicColor;

void ShaderBasicColorInit(ShaderBasicColor* _shaderBasicColor)
{
	_shaderBasicColor->m_shader = LoadShader("./resources/shaders/fs_basicColor.glsl", "./resources/shaders/vs_basicColor.glsl");
	_shaderBasicColor->m_specularity = GetShaderLocation(_shaderBasicColor->m_shader, "specularity");
	_shaderBasicColor->m_glosiness = GetShaderLocation(_shaderBasicColor->m_shader, "glossiness");
	_shaderBasicColor->m_camClipNear = GetShaderLocation(_shaderBasicColor->m_shader, "camClipNear");
	_shaderBasicColor->m_camClipFar = GetShaderLocation(_shaderBasicColor->m_shader, "camClipFar");
}

void ShaderBasicColorUpdate(ShaderBasicColor* _shaderBasicColor, const f32 _specilarity, const f32 _glosiness, const f32 _camClipNear, const f32 _camClipFar)
{
	SetShaderValue(_shaderBasicColor->m_shader, _shaderBasicColor->m_specularity, &_specularity, SHADER_UNIFORM_FLOAT);
	SetShaderValue(_shaderBasicColor->m_shader, _shaderBasicColor->m_glosiness, &_glossiness, SHADER_UNIFORM_FLOAT);
	SetShaderValue(_shaderBasicColor->m_shader, _shaderBasicColor->m_camClipNear, &_camClipNear, SHADER_UNIFORM_FLOAT);
	SetShaderValue(_shaderBasicColor->m_shader, _shaderBasicColor->m_camClipFar, &_camClipFar, SHADER_UNIFORM_FLOAT);
}
