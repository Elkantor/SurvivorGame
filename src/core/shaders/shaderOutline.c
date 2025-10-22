#pragma once

#include "../raylib.h"
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
    _shaderOutline->m_shader = LoadShader(NULL, "./resources/shaders/outline.fs");

    _shaderOutline->m_locThickness = GetShaderLocation(_shaderOutline->m_shader, "size");
    _shaderOutline->m_locWidth = GetShaderLocation(_shaderOutline->m_shader, "width");
    _shaderOutline->m_locHeight = GetShaderLocation(_shaderOutline->m_shader, "height");
    _shaderOutline->m_locColor = GetShaderLocation(_shaderOutline->m_shader, "color");
    _shaderOutline->m_locColorPicker = GetShaderLocation(_shaderOutline->m_shader, "pickingColor");
}

void ShaderOutlineUpdate(ShaderOutline* _shaderOutline)
{

}