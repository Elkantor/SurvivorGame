#pragma once

#include "../../../includes.c"

typedef struct Enemy
{
    vec2u32 m_cell;
    Timer m_animTimer;
    vec2u32 m_targetCell;
    Vector3 m_scale;
    Model m_model;
} Enemy;


static const f32 k_enemyMoveDuration = 0.5f;

void EnemyInit(Enemy* _enemy, const Model _model, const Grid _grid, const Vector3 _pos)
{
    _enemy->m_cell = GridIndexFromWorldPos(_grid, _pos);
    _enemy->m_model = _model;

    const Vector3 scale = { 1.f, 1.f, 1.f };
    const Vector3 rotation = { 0.f, 0.f, 0.f };
    _enemy->m_model.transform = Utils3DCreateTransform(_pos, rotation, scale);

    _enemy->m_targetCell = _enemy->m_cell;
    _enemy->m_animTimer = TimerInit(0.f);
}

void EnemyMoveTo(Enemy* _enemy, const Dir _targetDir, const Grid _grid)
{
    if (TimerIsStarted(&_enemy->m_animTimer))
        return;

    _enemy->m_scale = Utils3DGetScale(_enemy->m_model.transform);
    _enemy->m_targetCell = _enemy->m_cell; // Reset target cell just in case

    if (_targetDir == DIR_NONE)
    {
        return;
    }

    // Init timer for movement animation
    _enemy->m_animTimer = TimerInit(k_enemyMoveDuration);
    TimerStart(&_enemy->m_animTimer);

    const vec2u32 additionalCell[4] =
    {
        [DIR_UP] = { 0, -1 },
        [DIR_DOWN] = { 0, 1 },
        [DIR_LEFT] = { -1, 0 },
        [DIR_RIGHT] = { 1, 0 },
    };
    
    const vec2u32 targetCell =
    {
        .m_x = _enemy->m_cell.m_x + additionalCell[_targetDir].m_x,
        .m_y = _enemy->m_cell.m_y + additionalCell[_targetDir].m_y,
    };

    // Check if can move on the target cell, or is it OOB
    if (GridBoundsCheck(_grid, targetCell) == false)
    {
        return;
    }

    _enemy->m_targetCell = targetCell;
}

void EnemyUpdateMoveTo(Enemy* _enemy, const Grid _grid, const f32 _dt)
{
    if (TimerIsStarted(&_enemy->m_animTimer) == false)
        return;
    
    if (TimerIsFinished(&_enemy->m_animTimer))
    {
        _enemy->m_cell = _enemy->m_targetCell;
        const Timer resetedTimer = TimerInit(0.f);
        _enemy->m_animTimer = resetedTimer;
        return;
    }

    TimerUpdate(&_enemy->m_animTimer, _dt);

    const Vector3 enemyPos = Utils3DGetPosition(_enemy->m_model.transform);
    Vector3 fromPos = GridWorldPosFromIndex(_grid, _enemy->m_cell);
    fromPos.y = enemyPos.y;
    Vector3 toPos = GridWorldPosFromIndex(_grid, _enemy->m_targetCell);
    toPos.y = enemyPos.y;

    const f32 percent = 1.f - (_enemy->m_animTimer.m_time / k_enemyMoveDuration);
    const Vector3 currentPos = Utils3DVector3Lerp(fromPos, toPos, EaseOutBounce(percent));

    const Matrix transform = _enemy->m_model.transform;
    const Vector3 rotation = Utils3DGetRotation(transform);
    const Vector3 scale = Utils3DGetScale(transform);

    const Matrix newTransform = Utils3DCreateTransform(currentPos, rotation, scale);
    _enemy->m_model.transform = newTransform;
}

void EnemyUpdate(Enemy* _enemy, const Grid _grid, const f32 _dt)
{
    EnemyUpdateMoveTo(_enemy, _grid, _dt);
}