#pragma once

#include "../types.c"
#include "../raylib.h"

typedef struct ShaderLighting
{
    Shader m_shader;
    i32 m_gBufferColor;
    i32 m_gBufferNormal;
    i32 m_gBufferDepth;
    i32 m_ssao;
    i32 m_camPos;
    i32 m_camInvViewProj;
    i32 m_lightDir;
    i32 m_sunColor;
    i32 m_sunStrength;
    i32 m_skyColor;
    i32 m_skyStrength;
    i32 m_groundStrength;
    i32 m_ambientStrength;
    i32 m_exposure;
    i32 m_camClipNear;
    i32 m_camClipFar;
} ShaderLighting;

void ShaderLightingInit(ShaderLighting* _shaderLighting)
{
    _shaderLighting->m_shader = LoadShader("./resources/shaders/vs_post.glsl", "./resources/shaders/fs_lighting.glsl");
    _shaderLighting->m_gBufferColor = GetShaderLocation(_shaderLighting->m_shader, "gbufferColor");
    _shaderLighting->m_gBufferNormal = GetShaderLocation(_shaderLighting->m_shader, "gbufferNormal");
    _shaderLighting->m_gBufferDepth = GetShaderLocation(_shaderLighting->m_shader, "gbufferDepth");
    _shaderLighting->m_ssao = GetShaderLocation(_shaderLighting->m_shader, "ssao");
    _shaderLighting->m_camPos = GetShaderLocation(_shaderLighting->m_shader, "camPos");
    _shaderLighting->m_camInvViewProj = GetShaderLocation(_shaderLighting->m_shader, "camInvViewProj");
    _shaderLighting->m_lightDir = GetShaderLocation(_shaderLighting->m_shader, "lightDir");
    _shaderLighting->m_sunColor = GetShaderLocation(_shaderLighting->m_shader, "sunColor");
    _shaderLighting->m_sunStrength = GetShaderLocation(_shaderLighting->m_shader, "sunStrength");
    _shaderLighting->m_skyColor = GetShaderLocation(_shaderLighting->m_shader, "skyColor");
    _shaderLighting->m_skyStrength = GetShaderLocation(_shaderLighting->m_shader, "skyStrength");
    _shaderLighting->m_groundStrength = GetShaderLocation(_shaderLighting->m_shader, "groundStrength");
    _shaderLighting->m_ambientStrength = GetShaderLocation(_shaderLighting->m_shader, "ambientStrength");
    _shaderLighting->m_exposure = GetShaderLocation(_shaderLighting->m_shader, "exposure");
    _shaderLighting->m_camClipNear = GetShaderLocation(_shaderLighting->m_shader, "camClipNear");
    _shaderLighting->m_camClipFar = GetShaderLocation(_shaderLighting->m_shader, "camClipFar");
}
