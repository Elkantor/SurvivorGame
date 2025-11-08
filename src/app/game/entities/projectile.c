#pragma once

#include "../../../includes.c"

typedef struct Projectile
{
	Model m_model;
	Vector3 m_targetPos;
} Projectile;

ProjectileInit(Projectile* _projectile, const Model _model, const Vector3 _pos, const Vector3 _rotation, const Vector3 _scale)
{
	_projectile->m_model = _model;
	_projectile->m_targetPos = _pos;

	_projectile->m_model.transform = Utils3DCreateTransform(_pos, _rotation, _scale);
}

ProjectileUpdate(Projectile* _projectile, const _dt, const f32 _t)
{
	// _t is the cursor between [0;1] of the projectile lerp between its model position, and the target position
}