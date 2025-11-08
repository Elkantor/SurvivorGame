#pragma once

#include "../../../includes.c"

typedef struct Building
{
    Projectile m_projectiles[2];
    u32 m_projectilesSize;

    Model m_model;
    Timer m_animTimerShoot;
    vec2u32 m_cell;
} Building;

void BuildingInit(Building* _building, const char* _modelPath, const Vector3 _pos, const Vector3 _rotation, const Vector3 _scale)
{
    _building->m_model = LoadModel(_modelPath);
    _building->m_model.transform = Utils3DCreateTransform(_pos, _rotation, _scale);
}

static const f32 k_buildingShootDuration = 0.5f;

void BuildingShootTo(Building* _building, const Grid _grid, const vec2u32 _targetPos)
{
    if (TimerIsStarted(&_building->m_animTimerShoot))
        return;

    if (GridBoundsCheck(_grid, _targetPos) == false)
        return;

    // TODO: Create projectile

    _building->m_targetCell = _building->m_cell;
}

void BuildingUpdateShoot(Building* _building, const f32 _dt)
{
    if (TimerIsStarted(&_building->m_animTimerShoot) == false)
        return;

    if (TimerIsFinished(&_building->m_animTimerShoot))
    {
        _enemy->m_cell = _enemy->m_targetCell;
        const Timer resetedTimer = TimerInit(0.f);
        _enemy->m_animTimer = resetedTimer;
        return;
    }

    TimerUpdate(&_enemy->m_animTimer, _dt);
}

void BuildingUpdate(Building* _building, const f32 _dt)
{
    BuildingUpdateShoot(_building, _dt);
}

void BuildingRender(const Building* _building)
{
    DrawModel(_building->m_model, (Vector3) { 0.f, 0.f, 0.0f }, 1.f, WHITE);
}