#pragma once

#include "../../includes.c"
#include "scene.c"

typedef struct Game
{
    Font m_globalFont;
    Scene m_scene;
    RenderTexture3D m_renderTexture;
    GameCamera m_camera;
    HUD m_hud;
    f32 m_gold;
} Game;


void GameInit(Game* _game)
{
    _game->m_globalFont = LoadFontEx("resources/fonts/aclonica-v25-latin/aclonica-v25-latin-regular.ttf", 24, NULL, 167);

    RenderTexture3DInit(&_game->m_renderTexture);
    GameCameraInit(&_game->m_camera);
    SceneInit(&_game->m_scene);

    _game->m_gold = 0;
}

void GameUpdate(Game* _game, const f32 _dt)
{
    GameCameraUpdate(&_game->m_camera, _dt);
    SceneUpdate(&_game->m_scene, _dt);
    UpdateCamera(&_game->m_camera.m_cam, _game->m_camera.m_cam.projection);
}

void GameRender(Game* _game)
{
    // Color map
    /*BeginTextureMode(_game->m_renderTexture.m_texture);
    {
        ClearBackground(RAYWHITE);
        BeginMode3D(_game->m_camera.m_cam);
        {
            SceneRenderFlat(&_game->m_scene);
        }
        EndMode3D();
    }
    EndTextureMode();*/

    BeginDrawing();
    {
        ClearBackground(GRAY);

        BeginMode3D(_game->m_camera.m_cam);
        {
            SceneRender(&_game->m_scene);
            //DrawTexture(_game->m_renderTexture.m_texture.texture, 0, 0, WHITE);
            //DrawGrid(20, 1.0f);
            const f32 lines = 100.f;
            const f32 columns = 100.f;

            for (f32 i = 0; i < lines + 1; ++i)
            {
                const Vector3 startPos = { .x = -columns / 2.f, .y = 0.f, .z = (-lines / 2.f) + i };
                const Vector3 endPos = { .x = columns / 2.f, .y = 0.f, .z = (-lines / 2.f) + i };
                DrawLine3D(startPos, endPos, WHITE);
            }
            
            for (f32 i = 0; i < columns + 1; ++i)
            {
                const Vector3 startPos = { .x = (-columns / 2.f) + i, .y = 0.f, .z = -lines / 2.f };
                const Vector3 endPos = { .x = (-columns / 2.f) + i, .y = 0.f, .z = lines / 2.f };
                DrawLine3D(startPos, endPos, WHITE);
            }
        }
        EndMode3D();

        // 2D on top: UI
        {
            SceneRenderUI(&_game->m_scene);

            char goldTxt[20] = { 0 };
            snprintf(goldTxt, 20, "Gold: %f", _game->m_gold);
            DrawTextEx(_game->m_globalFont, goldTxt, (Vector2) { 10.f, 10.f }, _game->m_globalFont.baseSize, 1, BLACK);
        }
    }
    EndDrawing();
}