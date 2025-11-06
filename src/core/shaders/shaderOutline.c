#pragma once

#include "../raylib.h"
#include "../raymath.h"
#include "../types.c"

typedef struct ShaderOutline
{
    Shader m_shader;
    i32 m_locThickness;
    i32 m_locWidth;
    i32 m_locHeight;
    i32 m_locColor;
    i32 m_locColorPicker;
} ShaderOutline;

void ShaderOutlineInit(ShaderOutline* _shaderOutline)
{
    _shaderOutline->m_shader = LoadShader(NULL, "./resources/shaders/fs_outline.glsl");

    _shaderOutline->m_locThickness = GetShaderLocation(_shaderOutline->m_shader, "size");
    _shaderOutline->m_locWidth = GetShaderLocation(_shaderOutline->m_shader, "width");
    _shaderOutline->m_locHeight = GetShaderLocation(_shaderOutline->m_shader, "height");
    _shaderOutline->m_locColor = GetShaderLocation(_shaderOutline->m_shader, "color");
    _shaderOutline->m_locColorPicker = GetShaderLocation(_shaderOutline->m_shader, "pickingColor");

    i32 thickness = 1.f;
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locThickness, &thickness, SHADER_UNIFORM_INT);

    i32 width = GetScreenWidth();
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locWidth, &width, SHADER_UNIFORM_INT);

    i32 height = GetScreenHeight();
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locHeight, &height, SHADER_UNIFORM_INT);

    const float color[4] = { ORANGE.r / 255.f, ORANGE.g / 255.f, ORANGE.b / 255.f, ORANGE.a / 255.f };
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locColor, &color, SHADER_UNIFORM_VEC4);
}

void ShaderOutlineUpdate(ShaderOutline* _shaderOutline, const Camera _camera, const Vector3 _targetPos)
{
    const f32 dist = Vector3Distance(_camera.position, _targetPos);

    const f32 baseSize = 5.0f;
    const f32 minSize = 1.0f;
    const f32 falloff = 0.10f;

    const i32 outlineSize = (i32)Clamp(baseSize / (dist * falloff), minSize, baseSize);
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locThickness, &outlineSize, SHADER_UNIFORM_INT);

    i32 width = GetScreenWidth();
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locWidth, &width, SHADER_UNIFORM_INT);

    i32 height = GetScreenHeight();
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locHeight, &height, SHADER_UNIFORM_INT);
}