#pragma once

#include "../../includes.c"

void HPBarRender(const Vector3 _worldPos, const f32 _percent, const Camera _gameCam)
{
    const float baseBarWidth = 0.7f;
	const float baseBarHeight = 0.09f;

    const f32 screenWidth = GetScreenWidth();
    const f32 screenHeight = GetScreenHeight();

	const Vector3 headPosition = { _worldPos.x, _worldPos.y, _worldPos.z};
	const Vector2 screenPosition = GetWorldToScreen(headPosition, _gameCam);

    const Vector3 offsetPosition = { _worldPos.x + 1.0f, _worldPos.y, _worldPos.z };
    const Vector2 screenOffset = GetWorldToScreen(offsetPosition, _gameCam);
    f32 unitScale = Vector2Distance(screenPosition, screenOffset); // Pixel for one world unit

    const f32 maxUnitScale = 130.f;
    const f32 minUnitScale = 35.f;

    if (unitScale < 20.f)
        return;

    unitScale = Clamp(unitScale, minUnitScale, maxUnitScale);

    if (screenPosition.x > -unitScale * baseBarWidth && screenPosition.x < screenWidth + unitScale * baseBarWidth &&
        screenPosition.y > -unitScale * baseBarHeight && screenPosition.y < screenHeight + unitScale * baseBarHeight) {

        const f32 barWidth = baseBarWidth * unitScale;
        const f32 barHeight = baseBarHeight * unitScale;

        const f32 barX = screenPosition.x - barWidth / 2;
        const f32 barY = screenPosition.y - barHeight;

        DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);

        const f32 healthWidth = _percent * barWidth;
        const Color greenLight = { 0, 255, 85, 255 };
        const Color greenDark = { 0, 190, 20, 255 };
        DrawRectangleGradientV(barX, barY, healthWidth, barHeight, greenLight, greenDark);

        const f32 borderThickness = 0.02f * unitScale;
        const Rectangle border = { barX - borderThickness, barY - borderThickness, barWidth + borderThickness, barHeight + borderThickness };
        DrawRectangleLinesEx(border, borderThickness, DARKGRAY);
    }
}