#pragma once

#include "../raylib.h"
#include "../types.c"

typedef struct ShaderShadow
{
    Shader m_shader;
    i32 m_lightClipNearLoc;
    i32 m_lightClipFarLoc;
} ShaderShadow;

void ShaderShadowInit(ShaderShadow* _shaderShadow)
{
    _shaderShadow->m_shader = LoadShader("./resources/shaders/vs_shadow.glsl", "./resources/shaders/fs_shadow.glsl");

    _shaderShadow->m_lightClipNearLoc = GetShaderLocation(_shaderShadow->m_shader, "lightClipNear");
    _shaderShadow->m_lightClipFarLoc = GetShaderLocation(_shaderShadow->m_shader, "lightClipFar");
}

void ShaderShadowUpdate(const ShaderShadow* _shaderShadow, const f32 _clipNear, const f32 _clipFar)
{
    SetShaderValue(_shaderShadow->m_shader, _shaderShadow->m_lightClipNearLoc, &_clipNear, SHADER_UNIFORM_FLOAT);
    SetShaderValue(_shaderShadow->m_shader, _shaderShadow->m_lightClipFarLoc, &_clipFar, SHADER_UNIFORM_FLOAT);
}