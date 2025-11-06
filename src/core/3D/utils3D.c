#pragma once

#include "../raylib.h"
#include "../raymath.h"

Vector3 Utils3DGetPosition(const Matrix _transform)
{
	return (Vector3) { _transform.m12, _transform.m13, _transform.m14 };
}

Vector3 Utils3DGetScale(const Matrix _transform)
{
    const Vector3 row0 = { _transform.m0, _transform.m4, _transform.m8 };
    const Vector3 row1 = { _transform.m1, _transform.m5, _transform.m9 };
    const Vector3 row2 = { _transform.m2, _transform.m6, _transform.m10 };

    return (Vector3) 
    {
        Vector3Length(row0),
        Vector3Length(row1),
        Vector3Length(row2)
    };
}

Vector3 Utils3DGetRotation(const Matrix _transform)
{
    const Vector3 scale = Utils3DGetScale(_transform);

    if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) 
    {
        return (Vector3) { 0.0f, 0.0f, 0.0f };
    }

    const Matrix rot = 
    {
        _transform.m0 / scale.x, _transform.m4 / scale.x, _transform.m8 / scale.x, 0.0f,
        _transform.m1 / scale.y, _transform.m5 / scale.y, _transform.m9 / scale.y, 0.0f,
        _transform.m2 / scale.z, _transform.m6 / scale.z, _transform.m10 / scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    const Quaternion q = QuaternionFromMatrix(rot);
    const Vector3 eulerRad = QuaternionToEuler(q);

    // Result is in degrees
    return (Vector3) {
        eulerRad.x * RAD2DEG,
        eulerRad.y * RAD2DEG,
        eulerRad.z * RAD2DEG
    };
}

Matrix Utils3DCreateTransform(const Vector3 _pos, const Vector3 _rotation, const Vector3 _scale)
{
    const Matrix matScale = MatrixScale(_scale.x, _scale.y, _scale.z);
    const Matrix matRotation = MatrixRotateXYZ((Vector3) { _rotation.x, _rotation.y, _rotation.z });
    const Matrix matTranslation = MatrixTranslate(_pos.x, _pos.y, _pos.z);
    const Matrix transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

    return transform;
}