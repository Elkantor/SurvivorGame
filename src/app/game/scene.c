#pragma once

#include "../../includes.c"

typedef struct Scene
{
    Enemy m_enemies[10];
    u32 m_enemiesSize;

    Building m_towers[10];
    u32 m_towersSize;

    RoadCell m_roadCells[50];
    u32 m_roadCellsSize;

    Model m_modelProjectile;

    RadialMenu m_menuBuildings;
} Scene;

void SceneInit(Scene* _scene, const Grid _grid)
{
    // NOTE(Elkantor): TMP code to handle basic shape for enemy
    {
        const Mesh cylinder = GenMeshCylinder(0.25f, 1.f, 16.f);
        _scene->m_enemies[0].m_cell = (vec2u32){ (u32)(_grid.m_columns / 2.f) - 3, (u32)(_grid.m_lines / 2.f) };
        const Vector3 scale = { 1.f, 1.f, 1.f };
        const Vector3 rotation = { 0.f, 0.f, 0.f };
        Vector3 position = GridWorldPosFromIndex(_grid, _scene->m_enemies[0].m_cell);
        position.y += 0.25f;
        _scene->m_enemies[0].m_model = LoadModelFromMesh(cylinder);
        _scene->m_enemies[0].m_model.transform = Utils3DCreateTransform(position, rotation, scale);
        _scene->m_enemiesSize += 1;
    }

    BuildingInit(
        &_scene->m_towers[0],
        "resources/models/buildings/towers/elves/ArcherTowerLvl1/ArcherTowerLvl1.obj",
        (Vector3) { 0.5f, 0.f, 0.5f },
        (Vector3) { 0.f, 0.f, 0.f },
        (Vector3) { 0.2f, 0.2f, 0.2f }
    );
    _scene->m_towersSize += 1;

    BuildingInit(
        &_scene->m_towers[1],
        "resources/models/buildings/towers/elves/WizardTowerLvl1/WizardTowerLvl1.obj",
        (Vector3) { 2.5f, 0.f, 0.5f },
        (Vector3) { 0.f, 0.f, 0.f },
        (Vector3) { 0.2f, 0.2f, 0.2f }
    );
    _scene->m_towersSize += 1;

    const Mesh modelSphere = GenMeshSphere(0.1f, 10, 10);
    _scene->m_modelProjectile = LoadModelFromMesh(modelSphere);

    RadialMenuInit(&_scene->m_menuBuildings);
}

void SceneUpdate(Scene* _scene, const Camera _gameCam, const Grid _grid, const f32 _dt)
{
    RadialMenuUpdate(&_scene->m_menuBuildings, _dt);

    /*if (IsKeyPressed(KEY_DOWN))
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
    else*/ if (IsKeyPressed(KEY_ENTER))
    {
        const Vector3 enemyPos = Utils3DGetPosition(_scene->m_enemies[0].m_model.transform);
        const Vector3 targetPos = enemyPos;
        BuildingShootTo(&_scene->m_towers[0], _grid, targetPos, _scene->m_modelProjectile);
    }

    const KeyboardKey keyPressed = GetKeyPressed();
    const vec2u32 cellOvered = GridSelect(_grid, GetMousePosition(), _gameCam);

    if (keyPressed == KEY_UP || keyPressed == KEY_DOWN || keyPressed == KEY_RIGHT || keyPressed == KEY_LEFT)
    {
        u32 foundIndex = IndexInvalid;

        for (u32 i = 0; i < _scene->m_roadCellsSize; ++i)
        {
            if (RoadCellExistsAt(&_scene->m_roadCells[i], _grid, cellOvered))
            {
                foundIndex = i;
                break;
            }
        }

        const u32 capacity = sizeof(_scene->m_roadCells) / sizeof(_scene->m_roadCells[0]);

        if (foundIndex != IndexInvalid)
        {
            RoadCellUpdate(&_scene->m_roadCells[foundIndex], keyPressed);
        }
        else if (_scene->m_roadCellsSize < capacity)
        {
            RoadCellInit(&_scene->m_roadCells[_scene->m_roadCellsSize]);
            RoadCellUpdate(&_scene->m_roadCells[_scene->m_roadCellsSize], keyPressed);
            _scene->m_roadCells[_scene->m_roadCellsSize].m_cell = cellOvered;
            _scene->m_roadCellsSize += 1;

        }
    }
    else if (keyPressed == KEY_DELETE)
    {
        u32 foundIndex = IndexInvalid;

        for (u32 i = 0; i < _scene->m_roadCellsSize; ++i)
        {
            if (RoadCellExistsAt(&_scene->m_roadCells[i], _grid, cellOvered))
            {
                foundIndex = i;
                break;
            }
        }

        if (foundIndex != IndexInvalid)
        {
            _scene->m_roadCells[foundIndex] = _scene->m_roadCells[_scene->m_roadCellsSize - 1];
            _scene->m_roadCellsSize -= 1;
        }
    }

    for (u32 i = 0; i < _scene->m_enemiesSize; ++i)
    {
        EnemyUpdate(&_scene->m_enemies[i], _grid, _dt);
    }

    for (u32 i = 0; i < _scene->m_towersSize; ++i)
    {
        BuildingUpdate(&_scene->m_towers[i], _dt);
    }
}

void SceneRender(Scene* _scene, ShaderOutline* _shaderOutline, const Camera _gameCam, const Grid _grid, const vec2u32 _cellOvered)
{
    const Vector3 cellOveredToWorld = GridWorldPosFromIndex(_grid, _cellOvered);
    ShaderOutlineUpdate(_shaderOutline, _gameCam, cellOveredToWorld);
    f32 pickingColor[3] = { 1.f, 1.f, 1.f };

    for (u32 i = 0; i < _scene->m_towersSize; ++i)
    {
        const Vector3 position = Utils3DGetPosition(_scene->m_towers[i].m_model.transform);
        const vec2u32 cellPos = GridIndexFromWorldPos(_grid, position);

        if (cellPos.m_x == _cellOvered.m_x && cellPos.m_y == _cellOvered.m_y)
        {
            convertUint16ToFloat3Array((u16)i, pickingColor);
            pickingColor[0] /= 255.f;
            pickingColor[1] /= 255.f;
            pickingColor[2] /= 255.f;
        }

        BuildingRender(&_scene->m_towers[i]);
    }
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locColorPicker, &pickingColor, SHADER_UNIFORM_VEC3);

    for (u32 i = 0; i < _scene->m_enemiesSize; ++i)
    {
        DrawModel(_scene->m_enemies[i].m_model, (Vector3) { 0.f, 0.f, 0.f }, 1.f, WHITE);
    }

    for (u32 i = 0; i < _scene->m_roadCellsSize; ++i)
    {
        RoadCellRender(&_scene->m_roadCells[i], _grid);   
    }
}

void SceneRenderUI(Scene* _scene)
{
    RadialMenuRender(&_scene->m_menuBuildings);
}

void SceneRenderWithShader(const Scene* _scene, const Shader _shader)
{
    for (u16 i = 0; i < _scene->m_towersSize; ++i)
    {
        const Shader tmp = _scene->m_towers[i].m_model.materials[0].shader;
        _scene->m_towers[i].m_model.materials[0].shader = _shader;
        DrawModel(_scene->m_towers[i].m_model, (Vector3){ 0.f, 0.f, 0.f }, 1.f, WHITE);
        _scene->m_towers[i].m_model.materials[0].shader = tmp;
    }
}

void SceneRenderFlat(Scene* _scene, ShaderFlatColor* _shader)
{
    for (u16 i = 0; i < _scene->m_towersSize; ++i)
    {
        const u16 id = i;
        ShaderFlatColorUpdate(_shader, id);
        const Shader tmp = _scene->m_towers[i].m_model.materials[0].shader;

        _scene->m_towers[i].m_model.materials[0].shader = _shader->m_shader;

        const Matrix matScale = MatrixScale(1.f, 1.f, 1.f);
        const Matrix matRotation = MatrixRotateXYZ((Vector3) { 0.f, 0.f, 0.f });
        const Matrix matTranslation = MatrixTranslate(0.f, 0.f, 0.f);
        const Matrix transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

        DrawModelEx(_scene->m_towers[i].m_model, Utils3DGetPosition(transform), Utils3DGetRotation(transform), 0.f, Utils3DGetScale(transform), WHITE);
        _scene->m_towers[i].m_model.materials[0].shader = tmp;
    }
}