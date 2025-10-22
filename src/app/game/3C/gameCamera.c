#pragma once

#include "../../../includes.c"

typedef struct GameCamera
{
    Camera3D m_cam;
    f32 m_moveSpeed;
    f32 m_rotationSpeed;
    f32 m_zoomSpeed;
    f32 m_zoonMin;
    f32 m_zoomMax;
    f32 m_zoomLevel;
} GameCamera;

void GameCameraInit(GameCamera* _gameCamera)
{
    _gameCamera->m_cam.position = (Vector3){ 0.0f, 15.0f, 12.0f };  // Camera position
    _gameCamera->m_cam.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    _gameCamera->m_cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    _gameCamera->m_cam.fovy = 45.0f;                                // Camera field-of-view Y
    _gameCamera->m_cam.projection = CAMERA_PERSPECTIVE;

    _gameCamera->m_moveSpeed = 0.1f;
    _gameCamera->m_rotationSpeed = 0.001f;
    _gameCamera->m_zoomSpeed = 1.5f;
    _gameCamera->m_zoonMin = 5.f;
    _gameCamera->m_zoomMax = 100.f;
    _gameCamera->m_zoomLevel = 0.f;
}

void GameCameraUpdate(GameCamera* _gameCamera, const f32 _dt)
{
    // Update Zoom Level (zoom distance)
    {
        Vector3 direction = Vector3Subtract(_gameCamera->m_cam.position, _gameCamera->m_cam.target);
        f32 distance = Vector3Length(direction);
        _gameCamera->m_zoomLevel = distance;

        // Zoom / Dezoom
        {
            const f32 mouseWheel = GetMouseWheelMove();

            if (mouseWheel != 0.f)
            {
                const f32 zoomSpeed = _gameCamera->m_zoomSpeed;
                const f32 zoomMinDist = _gameCamera->m_zoonMin;
                const f32 zoomMaxDist = _gameCamera->m_zoomMax;

                distance -= mouseWheel * zoomSpeed;

                if (distance < zoomMinDist) distance = zoomMinDist;
                if (distance > zoomMaxDist) distance = zoomMaxDist;

                direction = Vector3Normalize(direction);
                direction = Vector3Scale(direction, distance);

                _gameCamera->m_cam.position = Vector3Add(_gameCamera->m_cam.target, direction);
            }
        }
    }

    const f32 moveSpeedFactor = _gameCamera->m_moveSpeed * (_gameCamera->m_zoomLevel / 30.f * PI);

    // Move camera
    {
        Vector3 moveDirection = { 0.0f, 0.0f, 0.0f };
        Vector3 forward = Vector3Subtract(_gameCamera->m_cam.target, _gameCamera->m_cam.position);
        forward.y = 0.0f; // ignore Y, as we only move on X/Z
        forward = Vector3Normalize(forward);

        const Vector3 right = { -forward.z, 0.0f, forward.x };

        if (IsKeyDown(KEY_W)) moveDirection = Vector3Add(moveDirection, forward);
        if (IsKeyDown(KEY_S)) moveDirection = Vector3Subtract(moveDirection, forward);
        if (IsKeyDown(KEY_D)) moveDirection = Vector3Add(moveDirection, right);
        if (IsKeyDown(KEY_A)) moveDirection = Vector3Subtract(moveDirection, right);

        if (Vector3Length(moveDirection) > 0)
        {
            moveDirection = Vector3Scale(Vector3Normalize(moveDirection), moveSpeedFactor);
            _gameCamera->m_cam.position = Vector3Add(_gameCamera->m_cam.position, moveDirection);
            _gameCamera->m_cam.target = Vector3Add(_gameCamera->m_cam.target, moveDirection);
        }
    }

    // Rotate camera
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        const Vector2 mouseDelta = GetMouseDelta();

        const f32 rotationAngle = mouseDelta.x * _gameCamera->m_rotationSpeed;
        const Vector3 direction = Vector3Subtract(_gameCamera->m_cam.position, _gameCamera->m_cam.target);

        const f32 newX = direction.x * cosf(rotationAngle) - direction.z * sinf(rotationAngle);
        const f32 newZ = direction.x * sinf(rotationAngle) + direction.z * cosf(rotationAngle);

        _gameCamera->m_cam.position.x = _gameCamera->m_cam.target.x + newX;
        _gameCamera->m_cam.position.z = _gameCamera->m_cam.target.z + newZ;
    }
}