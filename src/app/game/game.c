#pragma once

#include "../../includes.c"
#include "scene.c"

typedef struct Game
{
    Scene m_scene;
    Font m_globalFont;
    RenderTexture3D m_passColorPicker;
    RenderTexture3D m_passLighted;
    RenderTexture3D m_ssaoFront;
    RenderTexture3D m_ssaoBack;
    RenderTexture2D m_passShadowMap;
    GBuffer m_gbuffer;
    GameCamera m_camera;
    Shader m_shaderRadialFade;
    ShaderFlatColor m_shaderFlatColor;
    ShaderOutline m_shaderOutline;
    ShaderShadow m_shaderShadow;
    ShaderLighting m_shaderLighting;
    ShaderBlur m_shaderBlur;
    ShaderSSAO m_shaderSSAO;
    ShaderBasicColor m_shaderBasicColor;
    ShadowLight m_shadowLight;
    HUD m_hud;
    Console m_console;
    f32 m_gold;
} Game;

const f32 k_gridWidth = 100.f;
const f32 k_gridHeight = 100.f;

void GameInit(Game* _game)
{
    memset(_game, 0, sizeof(Game));

    const Grid grid = { .m_lines = k_gridWidth, .m_columns = k_gridHeight };
    SceneInit(&_game->m_scene, grid);

    _game->m_globalFont = LoadFontEx("resources/fonts/aclonica-v25-latin/aclonica-v25-latin-regular.ttf", 24, NULL, 167);
    RenderTexture3DInit(&_game->m_passColorPicker);
    RenderTexture3DInit(&_game->m_passLighted);
    RenderTexture3DInit(&_game->m_ssaoFront);
    RenderTexture3DInit(&_game->m_ssaoBack);
    _game->m_passShadowMap = LoadShadowMap(2048, 2048);
    _game->m_gbuffer = LoadGBuffer(GetScreenWidth(), GetScreenHeight());
    GameCameraInit(&_game->m_camera);

    _game->m_shaderRadialFade = LoadShader("resources/shaders/vs_radialFade.glsl", "resources/shaders/fs_radialFade.glsl");
    ShaderFlatColorInit(&_game->m_shaderFlatColor);
    ShaderOutlineInit(&_game->m_shaderOutline);
    ShaderShadowInit(&_game->m_shaderShadow);
    ShaderLightingInit(&_game->m_shaderLighting);
    ShaderBlurInit(&_game->m_shaderBlur);
    ShaderSSAOInit(&_game->m_shaderSSAO);
    ShaderBasicColorInit(&_game->m_shaderBasicColor);

    ShadowLightInit(&_game->m_shadowLight);

    _game->m_gold = 0;

    //rlSetClipPlanes(0.01f, 50.0f);
}

void GameUpdate(Game* _game, const f32 _dt)
{
    RenderTexture3DUpdate(&_game->m_passColorPicker);
    RenderTexture3DUpdate(&_game->m_passLighted);
    RenderTexture3DUpdate(&_game->m_ssaoFront);
    RenderTexture3DUpdate(&_game->m_ssaoBack);

    if (_game->m_console.m_drawable == false)
    {
        GameCameraUpdate(&_game->m_camera, _dt);
    }

    const Grid grid = { .m_lines = k_gridWidth, .m_columns = k_gridHeight };

    SceneUpdate(&_game->m_scene, _game->m_camera.m_cam, grid, _dt);
    UpdateCamera(&_game->m_camera.m_cam, _game->m_camera.m_cam.projection);

    ConsoleUpdate(&_game->m_console, _dt);
}

void GameRender(Game* _game)
{
    const Grid grid = { .m_lines = k_gridWidth, .m_columns = k_gridHeight };
    const vec2u32 cellOvered = GridSelect(grid, GetMousePosition(), _game->m_camera.m_cam);
    //rlDisableColorBlend();

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

        /*const f32 lightClipNear = rlGetCullDistanceNear();
        const f32 lightClipFar = rlGetCullDistanceFar();

        BeginShadowMap(_game->m_passShadowMap, _game->m_shadowLight);
        {
            ShaderShadowUpdate(&_game->m_shaderShadow, lightClipNear, lightClipFar);
            SceneRenderWithShader(&_game->m_scene, _game->m_shaderShadow.m_shader);
        }
        EndShadowMap();

        const f32 camClipNear = rlGetCullDistanceNear();
        const f32 camClipFar = rlGetCullDistanceFar();

        BeginGBuffer(_game->m_gbuffer, _game->m_camera.m_cam);
        {
            const f32 specularity = 0.9f;
            const f32 glossiness = 2.0f;

            SetShaderValue(_game->m_shaderBasicColor.m_shader, _game->m_shaderBasicColor.m_specularity, &specularity, SHADER_UNIFORM_FLOAT);
            SetShaderValue(_game->m_shaderBasicColor.m_shader, _game->m_shaderBasicColor.m_glossiness, &glossiness, SHADER_UNIFORM_FLOAT);
            SetShaderValue(_game->m_shaderBasicColor.m_shader, _game->m_shaderBasicColor.m_camClipNear, &camClipNear, SHADER_UNIFORM_FLOAT);
            SetShaderValue(_game->m_shaderBasicColor.m_shader, _game->m_shaderBasicColor.m_camClipFar, &camClipFar, SHADER_UNIFORM_FLOAT);

            SceneRenderWithShader(&_game->m_scene, _game->m_shaderBasicColor.m_shader);
        }
        EndGBuffer(GetScreenWidth(), GetScreenHeight());

        Matrix camView = rlGetMatrixModelview();
        Matrix camProj = rlGetMatrixProjection();
        Matrix lightViewProj = MatrixMultiply(rlGetMatrixModelview(), rlGetMatrixProjection());
        Matrix camInvProj = MatrixInvert(camProj);
        Matrix camInvViewProj = MatrixInvert(MatrixMultiply(camView, camProj));
        const Vector3 lightDir = Vector3Normalize((Vector3){ 0.35f, -1.0f, -0.35f });

        BeginTextureMode(_game->m_ssaoFront.m_texture);
        {
            BeginShaderMode(_game->m_shaderSSAO.m_shader);
            {
                const Vector2 shadowInvResolution = (Vector2){ 1.0f / _game->m_passShadowMap.texture.width, 1.0f / _game->m_passShadowMap.texture.height };

                SetShaderValueTexture(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_gBufferNormal, _game->m_gbuffer.m_normal);
                SetShaderValueTexture(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_gBufferDepth, _game->m_gbuffer.m_depth);
                SetShaderValueMatrix(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_camView, camView);
                SetShaderValueMatrix(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_camProj, camProj);
                SetShaderValueMatrix(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_camInvProj, camInvProj);
                SetShaderValueMatrix(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_camInvViewProj, camInvViewProj);
                SetShaderValueMatrix(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_lightViewProj, lightViewProj);
                SetShaderValueShadowMap(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_shadowMap, _game->m_passShadowMap);
                SetShaderValue(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_shadowInvResolution, &shadowInvResolution, SHADER_UNIFORM_VEC2);
                SetShaderValue(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_camClipNear, &camClipNear, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_camClipFar, &camClipFar, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_lightClipNear, &lightClipNear, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_lightClipFar, &lightClipFar, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderSSAO.m_shader, _game->m_shaderSSAO.m_lightDir, &lightDir, SHADER_UNIFORM_VEC3);

                ClearBackground(WHITE);
                DrawTextureRec(
                    _game->m_ssaoFront.m_texture.texture,
                    (Rectangle){ 0, 0, _game->m_ssaoFront.m_texture.texture.width, _game->m_ssaoFront.m_texture.texture.height },
                    (Vector2){ 0, 0 },
                    WHITE);
            }
            EndShaderMode();
        }
        EndTextureMode();

        BeginTextureMode(_game->m_ssaoBack.m_texture);
        {
            BeginShaderMode(_game->m_shaderBlur.m_shader);
            {
                const Vector2 blurDirection = (Vector2){ 1.0f, 0.0f };
                const Vector2 blurInvTextureResolution = (Vector2){ 1.0f / _game->m_ssaoFront.m_texture.texture.width, 1.0f / _game->m_ssaoFront.m_texture.texture.height };

                SetShaderValueTexture(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_gBufferNormal, _game->m_gbuffer.m_normal);
                SetShaderValueTexture(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_gBufferDepth, _game->m_gbuffer.m_depth);
                SetShaderValueTexture(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_inputTexture, _game->m_ssaoFront.m_texture.texture);
                SetShaderValueMatrix(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_camInvProj, camInvProj);
                SetShaderValue(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_camClipNear, &camClipNear, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_camClipFar, &camClipFar, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_invTextureResolution, &blurInvTextureResolution, SHADER_UNIFORM_VEC2);
                SetShaderValue(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_blurDirection, &blurDirection, SHADER_UNIFORM_VEC2);

                DrawTextureRec(
                    _game->m_ssaoBack.m_texture.texture,
                    (Rectangle){ 0, 0, _game->m_ssaoBack.m_texture.texture.width, -_game->m_ssaoBack.m_texture.texture.height },
                    (Vector2){ 0, 0 },
                    WHITE);
            }
            EndShaderMode();
        }
        EndTextureMode();

        BeginTextureMode(_game->m_ssaoFront.m_texture);
        {
            BeginShaderMode(_game->m_shaderBlur.m_shader);
            {
                const Vector2 blurDirection = (Vector2){ 0.0f, 1.0f };
                SetShaderValueTexture(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_inputTexture, _game->m_ssaoBack.m_texture.texture);
                SetShaderValue(_game->m_shaderBlur.m_shader, _game->m_shaderBlur.m_blurDirection, &blurDirection, SHADER_UNIFORM_VEC2);

                DrawTextureRec(
                    _game->m_ssaoFront.m_texture.texture,
                    (Rectangle){ 0, 0, _game->m_ssaoFront.m_texture.texture.width, -_game->m_ssaoFront.m_texture.texture.height },
                    (Vector2){ 0, 0 },
                    WHITE);
            }
            EndShaderMode();
        }
        EndTextureMode();

        BeginTextureMode(_game->m_passLighted.m_texture);
        {
            BeginShaderMode(_game->m_shaderLighting.m_shader);
            {
                const Vector3 sunColor = (Vector3){ 253.0f / 255.0f, 255.0f / 255.0f, 232.0f / 255.0f };
                const f32 sunStrength = 0.25f;
                const Vector3 skyColor = (Vector3){ 174.0f / 255.0f, 183.0f / 255.0f, 190.0f / 255.0f };
                const f32 skyStrength = 0.15f;
                const f32 groundStrength = 0.1f;
                const f32 ambientStrength = 1.0f;
                const f32 exposure = 0.9f;

                SetShaderValueTexture(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_gBufferColor, _game->m_gbuffer.m_color);
                SetShaderValueTexture(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_gBufferNormal, _game->m_gbuffer.m_normal);
                SetShaderValueTexture(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_gBufferDepth, _game->m_gbuffer.m_depth);
                SetShaderValueTexture(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_ssao, _game->m_ssaoFront.m_texture.texture);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_camPos, &_game->m_camera.m_cam.position, SHADER_UNIFORM_VEC3);
                SetShaderValueMatrix(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_camInvViewProj, camInvViewProj);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_lightDir, &lightDir, SHADER_UNIFORM_VEC3);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_sunColor, &sunColor, SHADER_UNIFORM_VEC3);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_sunStrength, &sunStrength, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_skyColor, &skyColor, SHADER_UNIFORM_VEC3);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_skyStrength, &skyStrength, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_groundStrength, &groundStrength, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_ambientStrength, &ambientStrength, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_exposure, &exposure, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_camClipNear, &camClipNear, SHADER_UNIFORM_FLOAT);
                SetShaderValue(_game->m_shaderLighting.m_shader, _game->m_shaderLighting.m_camClipFar, &camClipFar, SHADER_UNIFORM_FLOAT);

                ClearBackground(RAYWHITE);

                DrawTextureRec(
                    _game->m_gbuffer.m_color,
                    (Rectangle){ 0, 0, _game->m_gbuffer.m_color.width, -_game->m_gbuffer.m_color.height },
                    (Vector2){ 0, 0 },
                    WHITE);
                    }
                    EndShaderMode();
        }
        EndTextureMode();*/

        // // TODO(Elkantor): Need a FXAA pass maybe? For now just raw display the final lighted texture
        // DrawTextureRec(
        //     _game->m_passLighted.m_texture.texture,
        //     (Rectangle){ 0, 0, _game->m_passLighted.m_texture.texture.width, -_game->m_passLighted.m_texture.texture.height },
        //     (Vector2){ 0, 0 },
        //     WHITE);

        //rlEnableColorBlend();

        // Render 3D Models
        BeginMode3D(_game->m_camera.m_cam);
        {
            SceneRender(&_game->m_scene, &_game->m_shaderOutline, _game->m_camera.m_cam, grid, cellOvered);
            GridRender(grid, _game->m_camera.m_cam, _game->m_shaderRadialFade, (Color) { 180, 180, 180, 255 });
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
