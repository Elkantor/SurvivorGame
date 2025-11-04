#pragma once

#include "../raylib.h"
#include "../types.c"
#include "math.h"

typedef struct Grid
{
    f32 m_lines;
    f32 m_columns;
} Grid;

vec2u32 GridSelect(const Grid* _grid, const Vector2 _screenPos, const Camera _camera3D)
{
    const Ray mouseRay = GetScreenToWorldRay(_screenPos, _camera3D);

    if (fabsf(mouseRay.direction.y) < TinyEpsilon)
    {
        return (vec2u32) { IndexInvalid, IndexInvalid };
    }

    const f32 t = -mouseRay.position.y / mouseRay.direction.y;
    if (t < 0.0f)
    {
        return (vec2u32) { IndexInvalid, IndexInvalid };
    }

    const Vector3 point =
    {
        .x = mouseRay.position.x + t * mouseRay.direction.x,
        .y = 0.0f,
        .z = mouseRay.position.z + t * mouseRay.direction.z
    };

    const f32 min_x = -_grid->m_columns / 2.0f;
    const f32 min_z = -_grid->m_lines / 2.0f;

    const f32 fx = point.x - min_x;
    const f32 fz = point.z - min_z;

    const f32 ix = floorf(fx);
    const f32 iy = floorf(fz);

    if (ix >= 0 && ix < _grid->m_columns &&
        iy >= 0 && iy < _grid->m_lines &&
        fx >= 0.0f && fz >= 0.0f) {
        return (vec2u32) { (u32)ix, (u32)iy };
    }

    return (vec2u32) { IndexInvalid, IndexInvalid };
}

void GridRender(Grid* _grid, const Camera _gameCamera, const Color _color)
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

    // Draw overed cell in a specific color
    {
        const Vector2 mousePos = GetMousePosition();
        const vec2u32 cellOvered = GridSelect(_grid, mousePos, _gameCamera);
        if (cellOvered.m_x == IndexInvalid || cellOvered.m_y == IndexInvalid)
            return;

        const f32 left = -_grid->m_columns / 2.f + (f32)cellOvered.m_x;
        const f32 right = left + 1.f;
        const f32 bottom = -_grid->m_lines / 2.f + (f32)cellOvered.m_y;
        const f32 top = bottom + 1.f;

        const Color highlightColor = BLUE;

        // Bottom line
        const Vector3 bottomStart = {.x = left, .y = 0.f, .z = bottom };
        const Vector3 bottomEnd = {.x = right, .y = 0.f, .z = bottom };
        DrawLine3D(bottomStart, bottomEnd, highlightColor);

        // Top line
        const Vector3 topStart = {.x = left, .y = 0.f, .z = top };
        const Vector3 topEnd = {.x = right, .y = 0.f, .z = top };
        DrawLine3D(topStart, topEnd, highlightColor);

        // Left line
        const Vector3 leftStart = {.x = left, .y = 0.f, .z = bottom };
        const Vector3 leftEnd = {.x = left, .y = 0.f, .z = top };
        DrawLine3D(leftStart, leftEnd, highlightColor);

        // Right line
        const Vector3 rightStart = {.x = right, .y = 0.f, .z = bottom };
        const Vector3 rightEnd = {.x = right, .y = 0.f, .z = top };
        DrawLine3D(rightStart, rightEnd, highlightColor);
    }
}