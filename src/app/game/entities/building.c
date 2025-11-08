#pragma once

#include "../../../includes.c"

typedef struct Building
{
    Projectile m_projectiles[3];
    u8 m_projectilesSize;

    Model m_model;
    Timer m_animTimerShoot;
    vec2u32 m_cell;
} Building;

void BuildingInit(Building* _building, const char* _modelPath, const Vector3 _pos, const Vector3 _rotation, const Vector3 _scale)
{
    _building->m_model = LoadModel(_modelPath);
    _building->m_model.transform = Utils3DCreateTransform(_pos, _rotation, _scale);

    _building->m_projectilesSize = 0;
}

static const f32 k_buildingShootDuration = 0.5f;

void BuildingShootTo(Building* _building, const Grid _grid, const Vector3 _targetPos, const Model _projectileModel)
{
    if (TimerIsStarted(&_building->m_animTimerShoot))
        return;

    // Create Projectile only if it can
    if (_building->m_projectilesSize >= sizeof(_building->m_projectiles) / sizeof(_building->m_projectiles[0]))
        return;

    Vector3 pos = Utils3DGetPosition(_building->m_model.transform);
    pos.y += 2.f;

    const Vector3 rotation = (Vector3){ 0.f, 0.f, 0.f };
    const Vector3 scale = (Vector3){ 1.f, 1.f, 1.f };

    ProjectileInit(&_building->m_projectiles[_building->m_projectilesSize], _projectileModel, pos, rotation, scale);
    ProjectileLaunch(&_building->m_projectiles[_building->m_projectilesSize], _targetPos);

    _building->m_projectilesSize += 1;
}

void BuildingUpdateShoot(Building* _building, const f32 _dt)
{
    if (TimerIsStarted(&_building->m_animTimerShoot) == false)
        return;

    if (TimerIsFinished(&_building->m_animTimerShoot))
    {
        const Timer resetedTimer = TimerInit(0.f);
        _building->m_animTimerShoot = resetedTimer;
        return;
    }

    TimerUpdate(&_building->m_animTimerShoot, _dt);
}

void BuildingUpdate(Building* _building, const f32 _dt)
{
    BuildingUpdateShoot(_building, _dt);

    for (u8 i = 0; i < _building->m_projectilesSize; ++i)
    {
        ProjectileUpdate(&_building->m_projectiles[i], _dt);
    }
}

void BuildingRender(const Building* _building)
{
    DrawModel(_building->m_model, (Vector3) { 0.f, 0.f, 0.0f }, 1.f, WHITE);

    for (u8 i = 0; i < _building->m_projectilesSize; ++i)
    {
        ProjectileRender(&_building->m_projectiles[i]);
    }
}