#pragma once

#include "../types.c"
#include "../raylib.h"

typedef struct ShaderBlur
{
    Shader m_shader;
    i32 m_gBufferNormal;
    i32 m_gBufferDepth;
    i32 m_inputTexture;
    i32 m_camInvProj;
    i32 m_camClipNear;
    i32 m_camClipFar;
    i32 m_invTextureResolution;
    i32 m_blurDirection;
} ShaderBlur;

void ShaderBlurInit(ShaderBlur* _shaderBlur)
{
    _shaderBlur->m_shader = LoadShader("./resources/shaders/vs_post.glsl", "./resources/shaders/fs_blur.glsl");
    _shaderBlur->m_gBufferNormal = GetShaderLocation(_shaderBlur->m_shader, "gbufferNormal");
    _shaderBlur->m_gBufferDepth = GetShaderLocation(_shaderBlur->m_shader, "gbufferDepth");
    _shaderBlur->m_inputTexture = GetShaderLocation(_shaderBlur->m_shader, "inputTexture");
    _shaderBlur->m_camInvProj = GetShaderLocation(_shaderBlur->m_shader, "camInvProj");
    _shaderBlur->m_camClipNear = GetShaderLocation(_shaderBlur->m_shader, "camClipNear");
    _shaderBlur->m_camClipFar = GetShaderLocation(_shaderBlur->m_shader, "camClipFar");
    _shaderBlur->m_invTextureResolution = GetShaderLocation(_shaderBlur->m_shader, "invTextureResolution");
    _shaderBlur->m_blurDirection = GetShaderLocation(_shaderBlur->m_shader, "blurDirection");
}