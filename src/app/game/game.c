#pragma once

#include "../../includes.c"
#include "scene.c"

typedef struct Game
{
    Scene m_scene;
    Console m_console;
    MatCap m_matCap;
    GameCamera m_camera;
    Camera3D m_camLight;

    Model m_modelGround;
    Matrix m_lightView;
    Matrix m_lightProj;
    Matrix m_lightViewProj;

    RenderTexture3D m_passColorPicker;
    RenderTexture2D m_passShadows;
    Font m_globalFont;
    ShaderOutline m_shaderOutline;
    ShaderFlatColor m_shaderFlatColor;
    Shader m_shaderRadialFade;
    HUD m_hud;
    f32 m_gold;
    bool m_debugDrawShadowMap;
} Game;

const f32 k_gridWidth = 100.f;
const f32 k_gridHeight = 100.f;
const i32 k_textureActiveSlot = 10;

void GameInit(Game* _game)
{
    memset(_game, 0, sizeof(Game));

    MatCapInit(&_game->m_matCap);
    GameCameraInit(&_game->m_camera);

    _game->m_modelGround = LoadModelFromMesh(GenMeshPlane(100.f, 100.f, 1, 1));

    // Shadows
    _game->m_passShadows = Utils3DLoadShadowmapRenderTexture(2048, 2048);
    const Vector3 lightDir = Vector3Normalize((Vector3) { 0.35f, -1.0f, -0.35f });
    _game->m_camLight.position = Vector3Scale(lightDir, -15.0f);
    _game->m_camLight.target = Vector3Zero();
    _game->m_camLight.projection = CAMERA_ORTHOGRAPHIC; // Use an orthographic projection for directional lights
    _game->m_camLight.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    _game->m_camLight.fovy = 20.0f;

    RenderTexture3DInit(&_game->m_passColorPicker);

    _game->m_shaderRadialFade = LoadShader("resources/shaders/vs_radialFade.glsl", "resources/shaders/fs_radialFade.glsl");
    _game->m_globalFont = LoadFontEx("resources/fonts/aclonica-v25-latin/aclonica-v25-latin-regular.ttf", 24, NULL, 167);

    const Grid grid = { .m_lines = k_gridWidth, .m_columns = k_gridHeight };
    SceneInit(&_game->m_scene, grid, _game->m_matCap);

    ShaderFlatColorInit(&_game->m_shaderFlatColor);
    ShaderOutlineInit(&_game->m_shaderOutline);
    _game->m_gold = 0;
}

void GameUpdate(Game* _game, const f32 _dt, Arena* _frameArena)
{
    RenderTexture3DUpdate(&_game->m_passColorPicker);

    if (_game->m_console.m_drawable == false)
    {
        GameCameraUpdate(&_game->m_camera, _dt);
    }

    const Grid grid = { .m_lines = k_gridWidth, .m_columns = k_gridHeight };

    SceneUpdate(&_game->m_scene, _game->m_camera.m_cam, grid, _dt, _frameArena);
    UpdateCamera(&_game->m_camera.m_cam, _game->m_camera.m_cam.projection);

    ConsoleUpdate(&_game->m_console, _dt);
}

void GameRender(Game* _game)
{
    const Grid grid = { .m_lines = k_gridWidth, .m_columns = k_gridHeight };
    const vec2u32 cellOvered = GridSelect(grid, GetMousePosition(), _game->m_camera.m_cam);

    BeginDrawing();
    {
        // Color map
        BeginTextureMode(_game->m_passColorPicker.m_texture);
        {
            ClearBackground(WHITE);
            BeginMode3D(_game->m_camera.m_cam);
            {
                SceneRenderFlat(&_game->m_scene, &_game->m_shaderFlatColor);
            }
            EndMode3D();
        }
        EndTextureMode();

        ClearBackground(GRAY);

        // Shadows map
        BeginTextureMode(_game->m_passShadows);
        {
            ClearBackground(WHITE);
            
            const Vector3 cameraPos = _game->m_camera.m_cam.position;
            SetShaderValue(_game->m_matCap.m_shader, _game->m_matCap.m_shader.locs[SHADER_LOC_VECTOR_VIEW], &cameraPos, SHADER_UNIFORM_VEC3);

            const Vector3 lightDir = Vector3Normalize((Vector3) { 0.35f, -1.0f, -0.35f });
            SetShaderValue(_game->m_matCap.m_shader, _game->m_matCap.m_lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);
            _game->m_camLight.position = Vector3Scale(lightDir, -15.0f);

            rlEnableBackfaceCulling();
            rlSetCullFace(RL_CULL_FACE_FRONT);
            BeginMode3D(_game->m_camLight);
            {
                _game->m_lightView = rlGetMatrixModelview();
                _game->m_lightProj = rlGetMatrixProjection();
                
                SceneRenderShadows(&_game->m_scene);
            }
            EndMode3D();
            rlDisableBackfaceCulling();
        }
        EndTextureMode();
        
        _game->m_lightViewProj = MatrixMultiply(_game->m_lightView, _game->m_lightProj);

        SetShaderValueMatrix(_game->m_matCap.m_shader, _game->m_matCap.m_lightViewProjLoc, _game->m_lightViewProj);
        rlActiveTextureSlot(k_textureActiveSlot);
        rlEnableTexture(_game->m_passShadows.depth.id);
        rlSetUniform(_game->m_matCap.m_shadowMapLoc, &k_textureActiveSlot, SHADER_UNIFORM_INT, 1);
        rlSetCullFace(RL_CULL_FACE_BACK);

        // Render 3D Models
        BeginMode3D(_game->m_camera.m_cam);
        {
            _game->m_modelGround.materials[0].shader = _game->m_matCap.m_shader;
            DrawModel(_game->m_modelGround, (Vector3) { 0.f, -0.101f, 0.f }, 1.f, WHITE);

            const vec2u32 cellHighlighted = _game->m_scene.m_displayRadialMenu ? (vec2u32) { IndexInvalid, IndexInvalid } : cellOvered;
            GridRender(grid, _game->m_camera.m_cam, _game->m_shaderRadialFade, (Color) { 180, 180, 180, 255 }, cellHighlighted);
            SetShaderValueV(_game->m_matCap.m_shader, _game->m_matCap.m_viewEyeLoc, &_game->m_camera.m_cam.position, SHADER_UNIFORM_VEC3, 1);
            SceneRender(&_game->m_scene, &_game->m_shaderOutline, _game->m_camera.m_cam, grid, cellHighlighted, _game->m_matCap);
        }
        EndMode3D();

        // Outline pass
        BeginShaderMode(_game->m_shaderOutline.m_shader);
        {
            const f32 w = _game->m_passColorPicker.m_texture.texture.width;
            const f32 h = _game->m_passColorPicker.m_texture.texture.height;
            DrawTextureRec(_game->m_passColorPicker.m_texture.texture, (Rectangle) { 0.f, 0.f, w, -h }, (Vector2) { 0, 0 }, WHITE);
        }
        EndShaderMode();

        // 2D on top: UI
        {
            SceneRenderUI(&_game->m_scene, _game->m_globalFont, _game->m_camera.m_cam);

            char goldTxt[20] = { 0 };
            snprintf(goldTxt, 20, "Gold: %f", _game->m_gold);
            DrawTextEx(_game->m_globalFont, goldTxt, (Vector2) { 10.f, 10.f }, _game->m_globalFont.baseSize, 1, BLACK);

            if (IsKeyDown(KEY_F1))
            {
                const f32 w = _game->m_passColorPicker.m_texture.texture.width;
                const f32 h = _game->m_passColorPicker.m_texture.texture.height;
                DrawTextureRec(_game->m_passColorPicker.m_texture.texture, (Rectangle) { 0.f, 0.f, w, -h }, (Vector2) { 0, 0 }, WHITE);
            }
            
            if (IsKeyPressed(KEY_F3))
            {
                _game->m_debugDrawShadowMap = !_game->m_debugDrawShadowMap;
            }
            if (_game->m_debugDrawShadowMap)
            {
                const f32 w = _game->m_passShadows.depth.width;
                const f32 h = _game->m_passShadows.depth.height;
                DrawTextureRec(_game->m_passShadows.depth, (Rectangle) { 0.f, 0.f, w, -h }, (Vector2) { 0, 0 }, WHITE);
            }

            ConsoleRender(&_game->m_console, GetFrameTime());

            char cellPosTxt[50] = { 0 };
            snprintf(cellPosTxt, 50, "Cell pos: x: %d, y: %d", cellOvered.m_x, cellOvered.m_y);
            DrawTextEx(_game->m_globalFont, cellPosTxt, (Vector2) { 10.f, GetScreenHeight() - 20.f }, _game->m_globalFont.baseSize, 1, BLACK);
        }
    }
    EndDrawing();
}
