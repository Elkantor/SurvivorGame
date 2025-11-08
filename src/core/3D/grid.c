#pragma once

#include "../raylib.h"
#include "../raymath.h"
#include "../rlgl.h"
#include "../types.c"
#include "math.h"

typedef struct Grid
{
    f32 m_lines;
    f32 m_columns;
} Grid;

bool GridBoundsCheck(const Grid _grid, const vec2u32 _cell)
{
    if (_cell.m_x == IndexInvalid)
        return false;
    else if (_cell.m_y == IndexInvalid)
        return false;
    else if (_cell.m_x > _grid.m_columns)
        return false;
    else if (_cell.m_y > _grid.m_lines)
        return false;

    return true;
}

vec2u32 GridIndexFromWorldPos(const Grid _grid, const Vector3 _worldPos)
{
    const Vector3 point =
    {
        .x = _worldPos.x,
        .y = 0.0f,
        .z = _worldPos.z
    };

    const f32 min_x = -_grid.m_columns / 2.0f;
    const f32 min_z = -_grid.m_lines / 2.0f;

    const f32 fx = point.x - min_x;
    const f32 fz = point.z - min_z;

    const f32 ix = floorf(fx);
    const f32 iy = floorf(fz);

    if (ix >= 0 && ix < _grid.m_columns &&
        iy >= 0 && iy < _grid.m_lines &&
        fx >= 0.0f && fz >= 0.0f) {
        return (vec2u32) { (u32)ix, (u32)iy };
    }

    return (vec2u32) { IndexInvalid, IndexInvalid };
}

Vector3 GridWorldPosFromIndex(const Grid _grid, const vec2u32 _gridIndex)
{
    if (_gridIndex.m_x == IndexInvalid || _gridIndex.m_y == IndexInvalid)
        return (Vector3) { INFINITY, INFINITY, INFINITY };

    const f32 min_x = -_grid.m_columns / 2.0f;
    const f32 min_z = -_grid.m_lines / 2.0f;

    const f32 px = _gridIndex.m_x + min_x + 0.5f; // 0.5 to get the center of the cell
    const f32 pz = _gridIndex.m_y + min_z + 0.5f;

    return (Vector3) { px, 0.f, pz };
}

vec2u32 GridSelect(const Grid _grid, const Vector2 _screenPos, const Camera _camera3D)
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

    return GridIndexFromWorldPos(_grid, point);
}


void DrawThickLine3D(const Vector3 start, const Vector3 end, const float thickness, Color color)
{
    const i32 sides = 4;
    DrawCylinderEx(start, end, thickness, thickness, sides, color);
}

Vector3 GridCameraTargetGet(const Camera _gameCamera)
{
    // Calculate center : intersection with camera and the plane at Y=0
    const Vector3 viewDir = Vector3Normalize(Vector3Subtract(_gameCamera.target, _gameCamera.position));
    Vector3 center = { 0 };

    if (fabsf(viewDir.y) >= TinyEpsilon)
    {
        const f32 t = -_gameCamera.position.y / viewDir.y;

        if (t >= 0.0f)
        {
            center.x = _gameCamera.position.x + t * viewDir.x;
            center.y = 0.0f;
            center.z = _gameCamera.position.z + t * viewDir.z;
        }
    }

    return center;
}

void GridRender(const Grid _grid, const Camera _gameCamera, const Shader _shaderRadialFade, const Color _color)
{
    const Vector3 center = GridCameraTargetGet(_gameCamera);
    const f32 thickness = 0.03f;
    const f32 distScale = 30.0f;

    i32 centerLoc = GetShaderLocation(_shaderRadialFade, "center");
    SetShaderValueV(_shaderRadialFade, centerLoc, &center, SHADER_UNIFORM_VEC3, 1);

    i32 cameraPosLoc = GetShaderLocation(_shaderRadialFade, "cameraPos");
    SetShaderValueV(_shaderRadialFade, cameraPosLoc, &_gameCamera.position, SHADER_UNIFORM_VEC3, 1);

    i32 distScaleLoc = GetShaderLocation(_shaderRadialFade, "distScale");
    SetShaderValue(_shaderRadialFade, distScaleLoc, &distScale, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(_shaderRadialFade);

    for (f32 i = 0; i < _grid.m_lines + 1; ++i)
    {
        const Vector3 startPos = { .x = -_grid.m_columns / 2.f, .y = 0.f, .z = (-_grid.m_lines / 2.f) + i };
        const Vector3 endPos = { .x = _grid.m_columns / 2.f, .y = 0.f, .z = (-_grid.m_lines / 2.f) + i };
        DrawThickLine3D(startPos, endPos, thickness, _color);
    }

    for (f32 i = 0; i < _grid.m_columns + 1; ++i)
    {
        const Vector3 startPos = { .x = (-_grid.m_columns / 2.f) + i, .y = 0.f, .z = -_grid.m_lines / 2.f };
        const Vector3 endPos = { .x = (-_grid.m_columns / 2.f) + i, .y = 0.f, .z = _grid.m_lines / 2.f };
        DrawThickLine3D(startPos, endPos, thickness, _color);
    }

    EndShaderMode();

    // Draw overed cell in a specific color
    {
        const f32 thicknessSelected = 0.04f;
        const Vector2 mousePos = GetMousePosition();
        const vec2u32 cellOvered = GridSelect(_grid, mousePos, _gameCamera);

        if (cellOvered.m_x == IndexInvalid || cellOvered.m_y == IndexInvalid)
            return;

        const f32 left = -_grid.m_columns / 2.f + (f32)cellOvered.m_x;
        const f32 right = left + 1.f;
        const f32 bottom = -_grid.m_lines / 2.f + (f32)cellOvered.m_y;
        const f32 top = bottom + 1.f;

        const Color highlightColor = RED;

        // Bottom line
        const Vector3 bottomStart = {.x = left, .y = 0.f, .z = bottom };
        const Vector3 bottomEnd = {.x = right, .y = 0.f, .z = bottom };
        DrawThickLine3D(bottomStart, bottomEnd, thicknessSelected, highlightColor);

        // Top line
        const Vector3 topStart = {.x = left, .y = 0.f, .z = top };
        const Vector3 topEnd = {.x = right, .y = 0.f, .z = top };
        DrawThickLine3D(topStart, topEnd, thicknessSelected, highlightColor);

        // Left line
        const Vector3 leftStart = {.x = left, .y = 0.f, .z = bottom };
        const Vector3 leftEnd = {.x = left, .y = 0.f, .z = top };
        DrawThickLine3D(leftStart, leftEnd, thicknessSelected, highlightColor);

        // Right line
        const Vector3 rightStart = {.x = right, .y = 0.f, .z = bottom };
        const Vector3 rightEnd = {.x = right, .y = 0.f, .z = top };
        DrawThickLine3D(rightStart, rightEnd, thicknessSelected, highlightColor);
    }
}