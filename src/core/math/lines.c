#pragma once

#include "../types.c"
#include "../raylib.h"
#include "../raymath.h"

typedef enum LineSide
{
	LINE_LEFT,
	LINE_RIGHT,
	LINE_COLLINEAR,
	LINE_CROSSING
} LineSide;

LineSide GetLineSide(const Vector2 _p1, const Vector2 _p2, const Vector2 _p3, const Vector2 _p4)
{
	// Vector from p1 to p2 (direction of the reference line)
	const Vector2 v1 = { _p2.x - _p1.x, _p2.y - _p1.y };

	// Vectors from p1 to p3 and p1 to p4
	const Vector2 v2 = { _p3.x - _p1.x, _p3.y - _p1.y };
	const Vector2 v3 = { _p4.x - _p1.x, _p4.y - _p1.y };

	// Compute 2D cross products: v1 × v2 and v1 × v3
	float cross1 = v1.x * v2.y - v1.y * v2.x;
	float cross2 = v1.x * v3.y - v1.y * v3.x;

	// Use a small epsilon to handle floating-point precision issues
	const float epsilon = 0.0001f;

	// Normalize cross products to account for near-zero values (collinear case)
	bool cross1_zero = fabsf(cross1) < epsilon;
	bool cross2_zero = fabsf(cross2) < epsilon;

	// Both points are collinear (or nearly collinear)
	if (cross1_zero && cross2_zero) {
		return LINE_COLLINEAR;
	}

	// Check if both points are on the same side
	if (cross1_zero || cross2_zero || (cross1 > 0 && cross2 > 0) || (cross1 < 0 && cross2 < 0)) {
		// If either cross product is zero, or both have the same sign
		if (cross1 > 0 || cross2 > 0) {
			return LINE_LEFT; // At least one point is strictly to the left
		}
		else {
			return LINE_RIGHT; // At least one point is strictly to the right
		}
	}

	// Cross products have different signs, so the line crosses the reference line
	return LINE_CROSSING;
}