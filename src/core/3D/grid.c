#pragma once

#include "../raylib.h"
#include "../types.c"

typedef struct Grid
{
	f32 m_lines;
	f32 m_columns;
} Grid;

void GridRender(Grid* _grid, const Color _color)
{
    for (f32 i = 0; i < _grid->m_lines + 1; ++i)
    {
        const Vector3 startPos = { .x = -_grid->m_columns / 2.f, .y = 0.f, .z = (-_grid->m_lines / 2.f) + i };
        const Vector3 endPos = { .x = _grid->m_columns / 2.f, .y = 0.f, .z = (-_grid->m_lines / 2.f) + i };
        DrawLine3D(startPos, endPos, _color);
    }

    for (f32 i = 0; i < _grid->m_columns + 1; ++i)
    {
        const Vector3 startPos = { .x = (-_grid->m_columns / 2.f) + i, .y = 0.f, .z = -_grid->m_lines / 2.f };
        const Vector3 endPos = { .x = (-_grid->m_columns / 2.f) + i, .y = 0.f, .z = _grid->m_lines / 2.f };
        DrawLine3D(startPos, endPos, _color);
    }
}