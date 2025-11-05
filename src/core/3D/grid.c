#pragma once

#include "../raylib.h"
#include "../rlgl.h"
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

//void DrawThickLine3D(const Vector3 start, const Vector3 end, const f32 thickness, const Color color)
//{
//    const f32 radius = thickness / 2.0f;
//    const i32 sides = 4;
//
//    DrawCylinderEx(start, end, radius, radius, sides, color);
//}

void DrawThickLine3D(const Vector3 start, const Vector3 end, const float thickness, Color color, const Camera camera)
{
    const Vector3 midpoint = 
    {
        .x = (start.x + end.x) / 2.0f,
        .y = (start.y + end.y) / 2.0f,
        .z = (start.z + end.z) / 2.0f
    };

    const f32 dist = Vector3Distance(camera.position, midpoint);

    const float fadeStart = 15.0f;
    const float fadeEnd = 100.0f;

    float alpha = 1.0f;
    if (dist > fadeStart)
    {
        alpha = fmaxf(0.0f, 1.0f - (dist - fadeStart) / (fadeEnd - fadeStart));
    }

    color.a = (unsigned char)(255.0f * alpha);

    const float radius = thickness / 2.0f;
    const int sides = 4;

    DrawCylinderEx(start, end, radius, radius, sides, color);
}

void GridRender(Grid* _grid, const Camera _gameCamera, const Color _color)
{
    const f32 thickness = 0.05f;
    const f32 thicknessSelected = 0.06f;

    for (f32 i = 0; i < _grid->m_lines + 1; ++i)
    {
        const Vector3 startPos = { .x = -_grid->m_columns / 2.f, .y = 0.f, .z = (-_grid->m_lines / 2.f) + i };
        const Vector3 endPos = { .x = _grid->m_columns / 2.f, .y = 0.f, .z = (-_grid->m_lines / 2.f) + i };
        DrawThickLine3D(startPos, endPos, thickness, _color, _gameCamera);
    }

    for (f32 i = 0; i < _grid->m_columns + 1; ++i)
    {
        const Vector3 startPos = { .x = (-_grid->m_columns / 2.f) + i, .y = 0.f, .z = -_grid->m_lines / 2.f };
        const Vector3 endPos = { .x = (-_grid->m_columns / 2.f) + i, .y = 0.f, .z = _grid->m_lines / 2.f };
        DrawThickLine3D(startPos, endPos, thickness, _color, _gameCamera);
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
        DrawThickLine3D(bottomStart, bottomEnd, thicknessSelected, highlightColor, _gameCamera);

        // Top line
        const Vector3 topStart = {.x = left, .y = 0.f, .z = top };
        const Vector3 topEnd = {.x = right, .y = 0.f, .z = top };
        DrawThickLine3D(topStart, topEnd, thicknessSelected, highlightColor, _gameCamera);

        // Left line
        const Vector3 leftStart = {.x = left, .y = 0.f, .z = bottom };
        const Vector3 leftEnd = {.x = left, .y = 0.f, .z = top };
        DrawThickLine3D(leftStart, leftEnd, thicknessSelected, highlightColor, _gameCamera);

        // Right line
        const Vector3 rightStart = {.x = right, .y = 0.f, .z = bottom };
        const Vector3 rightEnd = {.x = right, .y = 0.f, .z = top };
        DrawThickLine3D(rightStart, rightEnd, thicknessSelected, highlightColor, _gameCamera);
    }
}