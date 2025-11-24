#pragma once

#include <stdbool.h>
#include "../raylib.h"
#include "../raymath.h"
#include "../types.c"
#include "../math/lines.c"

typedef struct RadialMenu
{
	Texture2D m_iconsList[8];
	u8 m_iconsListCount;

	Vector2 m_start;
	Vector2 m_end;
	f32 m_radius;
	u8 m_segmentSelected;
	bool m_display;
} RadialMenu;

void DrawCircleSegments(const Vector2 _center, const f32 _radius, const u32 _segments, const f32 _thickness, const Color color)
{
	if (_segments == 0) return;

	const f32 angleStep = ((2.0f * PI) / _segments);

	for (u32 i = 0; i < _segments; ++i)
	{
		const f32 angle = (i * angleStep) + 3 * PI/2.f;
		const Vector2 point = 
		{
			.x = _center.x + _radius * cosf(angle),
			.y = _center.y + _radius * sinf(angle)
		};

		DrawLineEx(_center, point, _thickness, color);
	}
}

void RadialMenuInit(RadialMenu* _radialMenu, Texture2D* _icons, const u8 _iconsCount)
{
	_radialMenu->m_start = Vector2Zero();
	_radialMenu->m_end = Vector2Zero();
	_radialMenu->m_radius = 100.f;
	_radialMenu->m_segmentSelected = IndexInvalidU8;
	
	const u8 iconsCapacity = sizeof(_radialMenu->m_iconsList) / sizeof(_radialMenu->m_iconsList[0]);
	for (u32 i = 0; i < _iconsCount && i < iconsCapacity; ++i)
	{
		_radialMenu->m_iconsList[i] = _icons[i];
	}

	_radialMenu->m_iconsListCount = _iconsCount;
}

void RadialMenuUpdate(RadialMenu* _radialMenu, const f32 _dt, bool* _display, const Vector2 _screenPos)
{
	const bool wasDisplayed = _radialMenu->m_display;
	_radialMenu->m_display = *_display;
	const bool freshDisplay = wasDisplayed == false && _radialMenu->m_display == true;

	if (wasDisplayed == false && freshDisplay == false)
		return;

	if (freshDisplay)
	{
		_radialMenu->m_start = _screenPos;
		_radialMenu->m_end = _radialMenu->m_start;
	}
	else if (_radialMenu->m_display)
	{
		_radialMenu->m_end = _screenPos;
	}
	else
	{
		// Not displayed, reset its fields
		_radialMenu->m_start = Vector2Zero();
		_radialMenu->m_end = Vector2Zero();
		_radialMenu->m_segmentSelected = IndexInvalidU8;
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
	{
		*_display = false;
	}
}

// Returns segment selected
void RadialMenuRender(RadialMenu* _radialMenu)
{
	if (_radialMenu->m_display == false)
	{
		return;
	}

	const Color darkGray = { DARKGRAY.r, DARKGRAY.g, DARKGRAY.b, 80 };
	const Color black = { BLACK.r, BLACK.g, BLACK.b, 80 };
	DrawRectangleGradientEx((Rectangle) { 0.f, 0.f, GetScreenWidth(), GetScreenHeight() }, darkGray, black, black, darkGray);

	const f32 outlineWidth = 2.f;
	const Color color = { 80, 80, 80, 255 };
	DrawCircleV(_radialMenu->m_start, _radialMenu->m_radius + outlineWidth + 1.f, DARKGRAY);
	DrawCircleV(_radialMenu->m_start, _radialMenu->m_radius + outlineWidth, WHITE);
	DrawCircle(_radialMenu->m_start.x, _radialMenu->m_start.y, _radialMenu->m_radius, color);

	const u32 segments = _radialMenu->m_iconsListCount;
	const f32 angleStep = (2.0f * PI) / segments;

	// Draw icons
	const u8 iconsCapacity = sizeof(_radialMenu->m_iconsList) / sizeof(_radialMenu->m_iconsList[0]);
	for (u8 i = 0; i < _radialMenu->m_iconsListCount && i < iconsCapacity; ++i)
	{
		const f32 scale = 0.06f;
		const f32 midAngle = (i + 0.5f) * angleStep + 3.0f * PI / 2.0f;
		const Vector2 midpoint = 
		{
			.x = _radialMenu->m_start.x + _radialMenu->m_radius * cosf(midAngle) * 0.6f,
			.y = _radialMenu->m_start.y + _radialMenu->m_radius * sinf(midAngle) * 0.6f
		};

		const Vector2 finalpoint =
		{
			.x = midpoint.x - 0.5f * (_radialMenu->m_iconsList[i].width * scale),
			.y = midpoint.y - 0.5f * (_radialMenu->m_iconsList[i].height * scale),
		};

		DrawTextureEx(_radialMenu->m_iconsList[i], finalpoint, 0.f, scale, WHITE);
	}

	for (u32 i = 0; i < segments; ++i)
	{
		const f32 startAngle = (i * angleStep) + 3 * PI/2.f;
		const f32 endAngle = ((i + 1) * angleStep) + 3 * PI/2.f;
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
			break;
		}

	}

	const f32 lineWidth = 2.f;
	DrawCircleSegments(_radialMenu->m_start, _radialMenu->m_radius, segments, lineWidth, LIGHTGRAY);
	DrawLineEx(_radialMenu->m_start, _radialMenu->m_end, lineWidth, ORANGE);
}