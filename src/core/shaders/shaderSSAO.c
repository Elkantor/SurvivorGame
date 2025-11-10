#pragma once

#include "../types.c"
#include "../raylib.h"

typedef struct ShaderSSAO
{
    Shader m_shader;
    i32 m_gBufferNormal;
    i32 m_gBufferDepth;
    i32 m_camView;
    i32 m_camProj;
    i32 m_camInvProj;
    i32 m_camInvViewProj;
    i32 m_lightViewProj;
    i32 m_shadowMap;
    i32 m_shadowInvResolution;
    i32 m_camClipNear;
    i32 m_camClipFar;
    i32 m_lightClipNear;
    i32 m_lightClipFar;
    i32 m_lightDir;
} ShaderSSAO;

void ShaderSSAOInit(ShaderSSAO* _shaderSSAO)
{
    _shaderSSAO->m_shader = LoadShader("./resources/shaders/vs_post.glsl", "./resources/shaders/fs_ssao.glsl");
    _shaderSSAO->m_gBufferNormal = GetShaderLocation(_shaderSSAO->m_shader, "gbufferNormal");
    _shaderSSAO->m_gBufferDepth = GetShaderLocation(_shaderSSAO->m_shader, "gbufferDepth");
    _shaderSSAO->m_camView = GetShaderLocation(_shaderSSAO->m_shader, "camView");
    _shaderSSAO->m_camProj = GetShaderLocation(_shaderSSAO->m_shader, "camProj");
    _shaderSSAO->m_camInvProj = GetShaderLocation(_shaderSSAO->m_shader, "camInvProj");
    _shaderSSAO->m_camInvViewProj = GetShaderLocation(_shaderSSAO->m_shader, "camInvViewProj");
    _shaderSSAO->m_lightViewProj = GetShaderLocation(_shaderSSAO->m_shader, "lightViewProj");
    _shaderSSAO->m_shadowMap = GetShaderLocation(_shaderSSAO->m_shader, "shadowMap");
    _shaderSSAO->m_shadowInvResolution = GetShaderLocation(_shaderSSAO->m_shader, "shadowInvResolution");
    _shaderSSAO->m_camClipNear = GetShaderLocation(_shaderSSAO->m_shader, "camClipNear");
    _shaderSSAO->m_camClipFar = GetShaderLocation(_shaderSSAO->m_shader, "camClipFar");
    _shaderSSAO->m_lightClipNear = GetShaderLocation(_shaderSSAO->m_shader, "lightClipNear");
    _shaderSSAO->m_lightClipFar = GetShaderLocation(_shaderSSAO->m_shader, "lightClipFar");
    _shaderSSAO->m_lightDir = GetShaderLocation(_shaderSSAO->m_shader, "lightDir");
}