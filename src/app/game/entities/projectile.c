#pragma once

#include "../../../includes.c"

typedef struct Projectile
{
	Model m_model;
	Vector3 m_startPos;
	Vector3 m_targetPos;
	Timer m_animTimer;
} Projectile;

static f32 k_projectileAnimDuration = 0.5f;

void ProjectileInit(Projectile* _projectile, const Model _model, const Vector3 _pos, const Vector3 _rotation, const Vector3 _scale)
{
	if (_projectile->m_model.meshCount == 0)
	{
		_projectile->m_model = _model;
	}

	_projectile->m_startPos = _pos;
	_projectile->m_targetPos = _pos;
	_projectile->m_model.transform = Utils3DCreateTransform(_pos, _rotation, _scale);
}

void ProjectileLaunch(Projectile* _projectile, const Vector3 _targetPos)
{
	if (TimerIsStarted(&_projectile->m_animTimer))
		return;

	_projectile->m_targetPos = _targetPos;
	_projectile->m_animTimer = TimerInit(k_projectileAnimDuration);
	TimerStart(&_projectile->m_animTimer);
}

void ProjectileUpdate(Projectile* _projectile, const f32 _dt)
{
	if (TimerIsStarted(&_projectile->m_animTimer) == false)
		return;

	if (TimerIsFinished(&_projectile->m_animTimer))
	{
		const Timer resetedTimer = TimerInit(0.f);
		_projectile->m_animTimer = resetedTimer;
		return;
	}

	TimerUpdate(&_projectile->m_animTimer, _dt);
	const f32 t = 1.f - (_projectile->m_animTimer.m_time / k_projectileAnimDuration);

	const Vector3 newPos = Utils3DVector3Lerp(_projectile->m_startPos, _projectile->m_targetPos, EaseOutQuart(t));
	const Vector3 rotation = Utils3DGetRotation(_projectile->m_model.transform);
	const Vector3 scale = Utils3DGetScale(_projectile->m_model.transform);

	_projectile->m_model.transform = Utils3DCreateTransform(newPos, rotation, scale);
}

void ProjectileRender(const Projectile* _projectile, MatCap _matCap)
{
	const Shader tmp = _projectile->m_model.materials[0].shader;
	_projectile->m_model.materials[0].shader = _matCap.m_shader;

	MatCapUpdate(&_matCap, 0.1f, 0.99);
	DrawModel(_projectile->m_model, (Vector3) { 0.f, 0.f, 0.f }, 1.f, BLACK);
	_projectile->m_model.materials[0].shader = tmp;

}