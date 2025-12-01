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

bool RoadCellSave(const RoadCell* _cell, binn* _binn)
{
	bool ret = true;

	ret &= binn_object_set_int32(_binn, "dir", _cell->m_dir);
	ret &= binn_object_set_uint32(_binn, "x", _cell->m_cell.m_x);
	ret &= binn_object_set_uint32(_binn, "y", _cell->m_cell.m_y);

	return ret;
}

void RoadCellLoad(RoadCell* _cell, const binn* _binn)
{
	_cell->m_dir = (Dir)binn_object_int32(_binn, "dir");
	_cell->m_cell.m_x = binn_object_uint32(_binn, "x");
	_cell->m_cell.m_y = binn_object_uint32(_binn, "y");
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
	Vector3 worldPos = GridWorldPosFromIndex(_grid, _cell->m_cell);
	worldPos.y += 0.1f;
	const f32 arrowScale = 0.25f;

	if (_cell->m_dir == DIR_LEFT)
	{
		const Vector3 v1 = { worldPos.x - arrowScale, worldPos.y, worldPos.z };
		const Vector3 v2 = { worldPos.x + arrowScale, worldPos.y, worldPos.z + arrowScale };
		const Vector3 v3 = { worldPos.x + arrowScale, worldPos.y, worldPos.z - arrowScale };
		DrawTriangle3D(v1, v2, v3, ORANGE);
	}
	else if (_cell->m_dir == DIR_RIGHT)
	{
		const Vector3 v1 = { worldPos.x + arrowScale, worldPos.y, worldPos.z };
		const Vector3 v2 = { worldPos.x - arrowScale, worldPos.y, worldPos.z - arrowScale };
		const Vector3 v3 = { worldPos.x - arrowScale, worldPos.y, worldPos.z + arrowScale };
		DrawTriangle3D(v1, v2, v3, ORANGE);
	}
	else if (_cell->m_dir == DIR_UP)
	{
		const Vector3 v1 = { worldPos.x - arrowScale, worldPos.y, worldPos.z + arrowScale };
		const Vector3 v2 = { worldPos.x + arrowScale, worldPos.y, worldPos.z + arrowScale };
		const Vector3 v3 = { worldPos.x, worldPos.y, worldPos.z - arrowScale };
		DrawTriangle3D(v1, v2, v3, ORANGE);
	}
	else if (_cell->m_dir == DIR_DOWN)
	{
		const Vector3 v1 = { worldPos.x - arrowScale, worldPos.y, worldPos.z - arrowScale };
		const Vector3 v2 = { worldPos.x, worldPos.y, worldPos.z + arrowScale };
		const Vector3 v3 = { worldPos.x + arrowScale, worldPos.y, worldPos.z - arrowScale };
		DrawTriangle3D(v1, v2, v3, ORANGE);
	}
}