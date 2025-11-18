#pragma once

#include "../../../includes.c"

typedef struct RoadCell
{
	Dir m_dir;
	vec2u32 m_cell;
} RoadCell;

void RoadCellInit(RoadCell* _cell)
{
	_cell->m_dir = DIR_NONE;
	_cell->m_cell.m_x = IndexInvalid;
	_cell->m_cell.m_y = IndexInvalid;
}

bool RoadCellExistsAt(const RoadCell* _cell, const Grid _grid, const vec2u32 _gridPos)
{
	if (GridBoundsCheck(_grid, _cell->m_cell) == false)
		return false;

	if (GridBoundsCheck(_grid, _gridPos) == false)
		return false;

	if (_cell->m_cell.m_x == _gridPos.m_x)
		if (_cell->m_cell.m_y == _gridPos.m_y)
			return true;

	return false;
}

void RoadCellUpdate(RoadCell* _cell, const KeyboardKey _keyPressed)
{
	switch (_keyPressed)
	{
		case KEY_UP: _cell->m_dir = DIR_UP; break;
		case KEY_DOWN: _cell->m_dir = DIR_DOWN; break;
		case KEY_LEFT: _cell->m_dir = DIR_LEFT; break;
		case KEY_RIGHT: _cell->m_dir = DIR_RIGHT; break;
		default: return;
	}
}

void RoadCellRender(const RoadCell* _cell, const Grid _grid)
{
	const Vector3 worldPos = GridWorldPosFromIndex(_grid, _cell->m_cell);

	if (_cell->m_dir == DIR_LEFT)
	{
		const Vector3 v1 = { worldPos.x - 0.5f, worldPos.y, worldPos.z };
		const Vector3 v2 = { worldPos.x + 0.5f, worldPos.y, worldPos.z + 0.5f };
		const Vector3 v3 = { worldPos.x + 0.5f, worldPos.y, worldPos.z - 0.5f };
		DrawTriangle3D(v1, v2, v3, GREEN);
	}
	else if (_cell->m_dir == DIR_RIGHT)
	{
		const Vector3 v1 = { worldPos.x + 0.5f, worldPos.y, worldPos.z };
		const Vector3 v2 = { worldPos.x - 0.5f, worldPos.y, worldPos.z - 0.5f };
		const Vector3 v3 = { worldPos.x - 0.5f, worldPos.y, worldPos.z + 0.5f };
		DrawTriangle3D(v1, v2, v3, GREEN);
	}
	else if (_cell->m_dir == DIR_UP)
	{
		const Vector3 v1 = { worldPos.x - 0.5f, worldPos.y, worldPos.z + 0.5f };
		const Vector3 v2 = { worldPos.x + 0.5f, worldPos.y, worldPos.z + 0.5f };
		const Vector3 v3 = { worldPos.x, worldPos.y, worldPos.z - 0.5f };
		DrawTriangle3D(v1, v2, v3, GREEN);
	}
	else if (_cell->m_dir == DIR_DOWN)
	{
		const Vector3 v1 = { worldPos.x - 0.5f, worldPos.y, worldPos.z - 0.5f };
		const Vector3 v2 = { worldPos.x, worldPos.y, worldPos.z + 0.5f };
		const Vector3 v3 = { worldPos.x + 0.5f, worldPos.y, worldPos.z - 0.5f };
		DrawTriangle3D(v1, v2, v3, GREEN);
	}
}