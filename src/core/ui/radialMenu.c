#pragma once

#include <stdbool.h>
#include "../raylib.h"
#include "../raymath.h"
#include "../types.c"
#include "../math/lines.c"

typedef struct RadialMenu
{
	Vector2 m_start;
	Vector2 m_end;
	f32 m_radius;
	bool m_display;
} RadialMenu;

void DrawCircleSegments(const Vector2 _center, const f32 _radius, const u32 _segments, const f32 _thickness, const Color color)
{
	if (_segments == 0) return;

	const f32 angleStep = (2.0f * PI) / _segments;

	for (u32 i = 0; i < _segments; ++i)
	{
		const f32 angle = i * angleStep;
		const Vector2 point = 
		{
			.x = _center.x + _radius * cosf(angle),
			.y = _center.y + _radius * sinf(angle)
		};

		DrawLineEx(_center, point, _thickness, color);
	}
}

void RadialMenuInit(RadialMenu* _radialMenu)
{
	_radialMenu->m_start = Vector2Zero();
	_radialMenu->m_end = Vector2Zero();
	_radialMenu->m_radius = 100.f;
}

void RadialMenuUpdate(RadialMenu* _radialMenu, const f32 _dt)
{
	const bool wasDisplayed = _radialMenu->m_display;
	_radialMenu->m_display = IsKeyDown(KEY_B);
	const bool freshDisplay = wasDisplayed == false && _radialMenu->m_display == true;

	if (freshDisplay)
	{
		_radialMenu->m_start = GetMousePosition();
		_radialMenu->m_end = _radialMenu->m_start;
	}
	else if (_radialMenu->m_display)
	{
		_radialMenu->m_end = GetMousePosition();
	}
	else
	{
		// Not displayed, reset its fields
		RadialMenuInit(_radialMenu);
	}
}

void RadialMenuRender(RadialMenu* _radialMenu)
{
	if (_radialMenu->m_display == false)
	{
		return;
	}

	const f32 outlineWidth = 2.f;
	const Color color = { 80, 80, 80, 255 };
	DrawCircleV(_radialMenu->m_start, _radialMenu->m_radius + outlineWidth + 1.f, DARKGRAY);
	DrawCircleV(_radialMenu->m_start, _radialMenu->m_radius + outlineWidth, WHITE);
	DrawCircle(_radialMenu->m_start.x, _radialMenu->m_start.y, _radialMenu->m_radius, color);

	
	const f32 lineWidth = 2.f;
	const u32 segments = 6;
	DrawCircleSegments(_radialMenu->m_start, _radialMenu->m_radius, segments, lineWidth, LIGHTGRAY);
	DrawLineEx(_radialMenu->m_start, _radialMenu->m_end, lineWidth, BLACK);

	const f32 angleStep = (2.0f * PI) / segments;

	for (u32 i = 0; i < segments; ++i)
	{
		const f32 startAngle = i * angleStep;
		const f32 endAngle = (i + 1) * angleStep;
		const Vector2 pointSeg1 =
		{
			.x = _radialMenu->m_start.x + _radialMenu->m_radius * cosf(startAngle),
			.y = _radialMenu->m_start.y + _radialMenu->m_radius * sinf(startAngle)
		};
		const Vector2 pointSeg2 =
		{
			.x = _radialMenu->m_start.x + _radialMenu->m_radius * cosf(endAngle),
			.y = _radialMenu->m_start.y + _radialMenu->m_radius * sinf(endAngle)
		};

		const LineSide lineSideSeg1 = GetLineSide(_radialMenu->m_start, pointSeg1, _radialMenu->m_start, _radialMenu->m_end);
		const LineSide lineSideSeg2 = GetLineSide(_radialMenu->m_start, pointSeg2, _radialMenu->m_start, _radialMenu->m_end);

		if (lineSideSeg1 == LINE_LEFT && lineSideSeg2 == LINE_RIGHT)
		{
			Color enableColor = { 0, 117, 44, 150 };
			DrawCircleSector(_radialMenu->m_start, _radialMenu->m_radius, startAngle * RAD2DEG, endAngle * RAD2DEG, segments, enableColor);
			//DrawCircleSectorLines(_radialMenu->m_start, _radialMenu->m_radius, startAngle * RAD2DEG, endAngle * RAD2DEG, segments, DARKGREEN);
			break;
		}
	}
}