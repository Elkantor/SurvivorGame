#pragma once

#include "../../includes.c"

typedef struct Scene
{
    Enemy m_enemies[10];
    u32 m_enemiesSize;

    Model m_models[10];
    u32 m_modelSize;

    ShaderFlatColor m_shaderFlatColor;
    ShaderOutline m_shaderOutline;

    RenderTexture2D renderTarget;

    RadialMenu m_menuBuildings;
} Scene;

void SceneInit(Scene* _scene, const Grid _grid)
{
    const Mesh cube = GenMeshCube(1.f, 0.5f, 1.f);
    _scene->m_enemies[0].m_cell = (vec2u32){ (u32)(_grid.m_columns / 2.f) - 3, (u32)(_grid.m_lines / 2.f) };
    const Vector3 scale = { 1.f, 1.f, 1.f };
    const Vector3 rotation = { 0.f, 0.f, 0.f };
    Vector3 position = GridWorldPosFromIndex(_grid, _scene->m_enemies[0].m_cell);
    position.y += 0.25f;
    _scene->m_enemies[0].m_model = LoadModelFromMesh(cube);
    _scene->m_enemies[0].m_model.transform = Utils3DCreateTransform(position, rotation, scale);
    _scene->m_enemiesSize = 1;

    _scene->m_models[0] = LoadModel("resources/models/buildings/towers/elves/ArcherTowerLvl1/ArcherTowerLvl1.obj");
    _scene->m_models[0].transform = Utils3DCreateTransform(
        (Vector3) { 0.5f, 0.f, 0.5f },
        (Vector3) { 0.f, 0.f, 0.f },
        (Vector3) { 0.2f, 0.2f, 0.2f }
    );

    _scene->m_models[1] = LoadModel("resources/models/buildings/towers/elves/WizardTowerLvl1/WizardTowerLvl1.obj");
    _scene->m_models[1].transform = Utils3DCreateTransform(
        (Vector3) { 2.5f, 0.f, 0.5f },
        (Vector3) { 0.f, 0.f, 0.f },
        (Vector3) { 0.2f, 0.2f, 0.2f }
    );
    _scene->m_modelSize = 2;

    ShaderFlatColorInit(&_scene->m_shaderFlatColor);
    ShaderOutlineInit(&_scene->m_shaderOutline);

    RadialMenuInit(&_scene->m_menuBuildings);
}

void SceneUpdate(Scene* _scene, const Camera _gameCam, const Grid _grid, const f32 _dt)
{
    RadialMenuUpdate(&_scene->m_menuBuildings, _dt);

    if (IsKeyPressed(KEY_DOWN))
    {
        EnemyMoveTo(&_scene->m_enemies[0], DIR_UP, _grid);
    }
    else if (IsKeyPressed(KEY_UP))
    {
        EnemyMoveTo(&_scene->m_enemies[0], DIR_DOWN, _grid);
    }
    else if (IsKeyPressed(KEY_LEFT))
    {
        EnemyMoveTo(&_scene->m_enemies[0], DIR_LEFT, _grid);
    }
    else if (IsKeyPressed(KEY_RIGHT))
    {
        EnemyMoveTo(&_scene->m_enemies[0], DIR_RIGHT, _grid);
    }

    for (u32 i = 0; i < _scene->m_enemiesSize; ++i)
    {
        EnemyUpdate(&_scene->m_enemies[i], _grid, _dt);
    }
}

void SceneRender(Scene* _scene, const Camera _gameCam, const Grid _grid, const vec2u32 _cellOvered)
{
    const Vector3 cellOveredToWorld = GridWorldPosFromIndex(_grid, _cellOvered);
    ShaderOutlineUpdate(&_scene->m_shaderOutline, _gameCam, cellOveredToWorld);
    f32 pickingColor[3] = { 1.f, 1.f, 1.f };

    for (u32 i = 0; i < _scene->m_modelSize; ++i)
    {
        const Vector3 position = Utils3DGetPosition(_scene->m_models[i].transform);
        const vec2u32 cellPos = GridIndexFromWorldPos(_grid, position);

        if (cellPos.m_x == _cellOvered.m_x && cellPos.m_y == _cellOvered.m_y)
        {
            convertUint16ToFloat3Array((u16)i, pickingColor);
            pickingColor[0] /= 255.f;
            pickingColor[1] /= 255.f;
            pickingColor[2] /= 255.f;
        }

        DrawModel(_scene->m_models[i], (Vector3) { 0.f, 0.f, 0.0f }, 1.f, WHITE);
    }
    SetShaderValue(_scene->m_shaderOutline.m_shader, _scene->m_shaderOutline.m_locColorPicker, &pickingColor, SHADER_UNIFORM_VEC3);

    for (u32 i = 0; i < _scene->m_enemiesSize; ++i)
    {
        DrawModel(_scene->m_enemies[i].m_model, (Vector3) { 0.f, 0.f, 0.f }, 1.f, RED);
    }
}

void SceneRenderUI(Scene* _scene)
{
    RadialMenuRender(&_scene->m_menuBuildings);
}

void SceneRenderFlat(Scene* _scene)
{
    for (u16 i = 0; i < _scene->m_modelSize; ++i)
    {
        const u16 id = i;
        ShaderFlatColorUpdate(&_scene->m_shaderFlatColor, id);
        const Shader tmp = _scene->m_models[i].materials[0].shader;

        _scene->m_models[i].materials[0].shader = _scene->m_shaderFlatColor.m_shader;

        const Matrix matScale = MatrixScale(1.f, 1.f, 1.f);
        const Matrix matRotation = MatrixRotateXYZ((Vector3) { 0.f, 0.f, 0.f });
        const Matrix matTranslation = MatrixTranslate(0.f, 0.f, 0.f);
        const Matrix transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

        DrawModelEx(_scene->m_models[i], Utils3DGetPosition(transform), Utils3DGetRotation(transform), 0.f, Utils3DGetScale(transform), WHITE);
        _scene->m_models[i].materials[0].shader = tmp;
    }
}