#pragma once

#include "../types.c"
#include "../raylib.h"

typedef struct ShaderWater
{
	Shader m_shader;
    i32 m_locSeconds;
    i32 m_locFreqX;
    i32 m_locFreqY;
    i32 m_locAmpX;
    i32 m_locAmpY;
    i32 m_locSpeedX;
    i32 m_locSpeedY;
} ShaderWater;

void ShaderWaterInit(ShaderWater* _shaderWater)
{
    _shaderWater->m_shader = LoadShader(NULL, "resources/shaders/fs_water.glsl");
    _shaderWater->m_locSeconds = GetShaderLocation(_shaderWater->m_shader, "seconds");
    _shaderWater->m_locFreqX = GetShaderLocation(_shaderWater->m_shader, "freqX");
    _shaderWater->m_locFreqY = GetShaderLocation(_shaderWater->m_shader, "freqY");
    _shaderWater->m_locAmpX = GetShaderLocation(_shaderWater->m_shader, "ampX");
    _shaderWater->m_locAmpY = GetShaderLocation(_shaderWater->m_shader, "ampY");
    _shaderWater->m_locSpeedX = GetShaderLocation(_shaderWater->m_shader, "speedX");
    _shaderWater->m_locSpeedY = GetShaderLocation(_shaderWater->m_shader, "speedY");

    f32 colDiffuse[4] = { 0.0f, 0.8f, 0.8f, 1.0f };
    SetShaderValue(_shaderWater->m_shader, GetShaderLocation(_shaderWater->m_shader, "colDiffuse"), colDiffuse, SHADER_UNIFORM_VEC4);

    f32 tileSize = 1.0f;
    f32 size[2] = { tileSize, tileSize };
    SetShaderValue(_shaderWater->m_shader, GetShaderLocation(_shaderWater->m_shader, "size"), size, SHADER_UNIFORM_VEC2);
}

void ShaderWaterUpdate(ShaderWater* _shaderWater, f32 _freqX, f32 _freqY, f32 _ampX, f32 _ampY, f32 _speedX, f32 _speedY)
{
    float seconds = GetTime() / 60.f;

    SetShaderValue(_shaderWater->m_shader, _shaderWater->m_locSeconds, &seconds, SHADER_UNIFORM_FLOAT);

    SetShaderValue(_shaderWater->m_shader, _shaderWater->m_locFreqX, &_freqX, SHADER_UNIFORM_FLOAT);
    SetShaderValue(_shaderWater->m_shader, _shaderWater->m_locFreqY, &_freqY, SHADER_UNIFORM_FLOAT);
    SetShaderValue(_shaderWater->m_shader, _shaderWater->m_locAmpX, &_ampX, SHADER_UNIFORM_FLOAT);
    SetShaderValue(_shaderWater->m_shader, _shaderWater->m_locAmpY, &_ampY, SHADER_UNIFORM_FLOAT);
    SetShaderValue(_shaderWater->m_shader, _shaderWater->m_locSpeedX, &_speedX, SHADER_UNIFORM_FLOAT);
    SetShaderValue(_shaderWater->m_shader, _shaderWater->m_locSpeedY, &_speedY, SHADER_UNIFORM_FLOAT);
}