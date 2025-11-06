#pragma once

#include "../../includes.c"
#include "scene.c"

typedef struct Game
{
    Scene m_scene;
    Font m_globalFont;
    RenderTexture3D m_passColorPicker;
    RenderTexture2D m_passShadowMap;
    GameCamera m_camera;
    
    Shader m_shaderRadialFade;

    HUD m_hud;
    f32 m_gold;

    Console m_console;
} Game;

const f32 k_gridWidth = 100.f;
const f32 k_gridHeight = 100.f;

void GameInit(Game* _game)
{
    memset(_game, 0, sizeof(Game));

    SceneInit(&_game->m_scene);
    _game->m_globalFont = LoadFontEx("resources/fonts/aclonica-v25-latin/aclonica-v25-latin-regular.ttf", 24, NULL, 167);
    RenderTexture3DInit(&_game->m_passColorPicker);
    _game->m_passShadowMap = LoadShadowMap(2048, 2048);
    GameCameraInit(&_game->m_camera);

    _game->m_shaderRadialFade = LoadShader("resources/shaders/vs_radialFade.glsl", "resources/shaders/fs_radialFade.glsl");
    _game->m_gold = 0;
}

void GameUpdate(Game* _game, const f32 _dt)
{
    RenderTexture3DUpdate(&_game->m_passColorPicker);

    if (_game->m_console.m_drawable == false)
    {
        GameCameraUpdate(&_game->m_camera, _dt);
    }

    SceneUpdate(&_game->m_scene, _game->m_camera.m_cam, _dt);
    UpdateCamera(&_game->m_camera.m_cam, _game->m_camera.m_cam.projection);

    ConsoleUpdate(&_game->m_console, _dt);
}

void GameRender(Game* _game)
{
    const Grid grid = { .m_lines = k_gridWidth, .m_columns = k_gridHeight };
    const vec2u32 cellOvered = GridSelect(grid, GetMousePosition(), _game->m_camera.m_cam);

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

        BeginTextureMode(_game->m_passShadowMap);
        {
            ClearBackground(WHITE);
            rlClearScreenBuffers(); // Force clearing depth

            const Vector3 LightPos = { -1.0f, 5.0f, -1.0f };
            const Matrix MatProj = MatrixOrtho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f);  // Ajustez bounds au size de scène
            const Matrix MatView = MatrixLookAt(LightPos, (Vector3) { 0.0f, 0.0f, 0.0f }, (Vector3) { 0.0f, 1.0f, 0.0f });

            BeginMode3D(_game->m_camera.m_cam);
            {
                rlSetMatrixProjection(MatProj);
                rlSetMatrixModelview(MatView);
                SceneRender(&_game->m_scene, _game->m_camera.m_cam, grid, cellOvered);
            }
            EndMode3D();
        }
        EndTextureMode();

        // Render 3D Models
        BeginMode3D(_game->m_camera.m_cam);
        {
            SceneRender(&_game->m_scene, _game->m_camera.m_cam, grid, cellOvered);
            GridRender(grid, _game->m_camera.m_cam, _game->m_shaderRadialFade, WHITE);
        }
        EndMode3D();

        // Outline pass
        BeginShaderMode(_game->m_scene.m_shaderOutline.m_shader);
        {
            const f32 w = _game->m_passColorPicker.m_texture.texture.width;
            const f32 h = _game->m_passColorPicker.m_texture.texture.height;
            DrawTextureRec(_game->m_passColorPicker.m_texture.texture, (Rectangle) { 0.f, 0.f, w, -h }, (Vector2) { 0, 0 }, WHITE);
        }
        EndShaderMode();

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
            else if (IsKeyDown(KEY_F3))
            {
                const f32 w = _game->m_passShadowMap.texture.width;
                const f32 h = _game->m_passShadowMap.texture.height;
                DrawTextureRec(_game->m_passShadowMap.texture, (Rectangle) { 0.f, 0.f, w, -h }, (Vector2) { 0, 0 }, WHITE);
            }

            ConsoleRender(&_game->m_console, GetFrameTime());

            char cellPosTxt[50] = { 0 };
            snprintf(cellPosTxt, 50, "Cell pos: x: %d, y: %d", cellOvered.m_x, cellOvered.m_y);
            DrawTextEx(_game->m_globalFont, cellPosTxt, (Vector2) { 10.f, GetScreenHeight() - 20.f }, _game->m_globalFont.baseSize, 1, BLACK);
        }
    }
    EndDrawing();
}