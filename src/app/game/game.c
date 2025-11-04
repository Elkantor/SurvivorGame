#pragma once

#include "../../includes.c"
#include "scene.c"

typedef struct Game
{
    Font m_globalFont;
    Scene m_scene;
    RenderTexture3D m_passColorPicker;
    GameCamera m_camera;
    HUD m_hud;
    f32 m_gold;

    Console m_console;
} Game;

const f32 k_gridWidth = 100.f;
const f32 k_gridHeight = 100.f;

void GameInit(Game* _game)
{
    memset(_game, 0, sizeof(Game));

    _game->m_globalFont = LoadFontEx("resources/fonts/aclonica-v25-latin/aclonica-v25-latin-regular.ttf", 24, NULL, 167);

    RenderTexture3DInit(&_game->m_passColorPicker);
    GameCameraInit(&_game->m_camera);
    SceneInit(&_game->m_scene);

    _game->m_gold = 0;
}

void GameUpdate(Game* _game, const f32 _dt)
{
    ConsoleUpdate(&_game->m_console, _dt);
    
    if (_game->m_console.m_drawable == false)
    {
        GameCameraUpdate(&_game->m_camera, _dt);
    }

    SceneUpdate(&_game->m_scene, _dt);
    UpdateCamera(&_game->m_camera.m_cam, _game->m_camera.m_cam.projection);
}

void GameRender(Game* _game)
{
    // Color map
    BeginTextureMode(_game->m_passColorPicker.m_texture);
    {
        ClearBackground(WHITE);
        BeginMode3D(_game->m_camera.m_cam);
        {
            SceneRenderFlat(&_game->m_scene);
        }
        EndMode3D();
    }
    EndTextureMode();

    BeginDrawing();
    {
        ClearBackground(GRAY);

        BeginMode3D(_game->m_camera.m_cam);
        {
            SceneRender(&_game->m_scene);

            Grid grid = { .m_lines = k_gridWidth, .m_columns = k_gridHeight };
            GridRender(&grid, _game->m_camera.m_cam, WHITE);
        }
        EndMode3D();

        // 2D on top: UI
        {
            SceneRenderUI(&_game->m_scene);

            char goldTxt[20] = { 0 };
            snprintf(goldTxt, 20, "Gold: %f", _game->m_gold);
            DrawTextEx(_game->m_globalFont, goldTxt, (Vector2) { 10.f, 10.f }, _game->m_globalFont.baseSize, 1, BLACK);

            if (IsKeyDown(KEY_F1))
            {
                const f32 w = _game->m_passColorPicker.m_texture.texture.width;
                const f32 h = _game->m_passColorPicker.m_texture.texture.height;
                DrawTextureRec(_game->m_passColorPicker.m_texture.texture, (Rectangle) { 0.f, 0.f, w, -h }, (Vector2) { 0, 0 }, WHITE);
            }

            ConsoleRender(&_game->m_console, GetFrameTime());

            Grid grid = { .m_lines = k_gridWidth, .m_columns = k_gridHeight };
            const vec2u32 cellOvered = GridSelect(&grid, GetMousePosition(), _game->m_camera.m_cam);
            char cellPosTxt[50] = { 0 };
            snprintf(cellPosTxt, 50, "Cell pos: x: %d, y: %d", cellOvered.m_x, cellOvered.m_y);
            DrawTextEx(_game->m_globalFont, cellPosTxt, (Vector2) { 10.f, GetScreenHeight() - 20.f }, _game->m_globalFont.baseSize, 1, BLACK);
        }
    }
    EndDrawing();
}