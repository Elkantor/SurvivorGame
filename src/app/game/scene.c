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

    Model m_modelEnemy;
    Model m_modelTowerArcher1;
    Model m_modelTowerWizard1;
    Model m_modelProjectile;

    Texture2D m_iconSimpleArrow;
    Texture2D m_iconColdArrow;
    Texture2D m_iconFireArrow;
    Texture2D m_iconToxicArrow;
    Texture2D m_iconSniperArrow;
    Texture2D m_iconElectricArrow;

    RadialMenu m_menuBuildings;
    bool m_displayRadialMenu;
} Scene;

void SceneSpawnEnemy(Scene* _scene, const Grid _grid, const vec2u32 _spawnCell)
{
    const u32 capacity = sizeof(_scene->m_enemies) / sizeof(_scene->m_enemies[0]);

    if (_scene->m_enemiesSize >= capacity)
        return;

    if (GridBoundsCheck(_grid, _spawnCell) == false)
        return;

    const u32 index = _scene->m_enemiesSize;
    Vector3 position = GridWorldPosFromIndex(_grid, _spawnCell);
    position.y += 0.25f;

    EnemyInit(&_scene->m_enemies[index], _scene->m_modelEnemy, _grid, position);

    _scene->m_enemiesSize += 1;
}

void SceneSave(const Scene* _scene)
{
    u8 buff[204800] = { 0 };
    binn obj = { 0 };

    bool success = true;

    if (binn_create(&obj, BINN_OBJECT, sizeof(buff), buff))
    {
        // Road Cells
        {
            u8 buffList[CHUNK_SIZE * 10] = { 0 };
            binn list = { 0 };

            if (binn_create(&list, BINN_LIST, sizeof(buffList), buffList))
            {
                for (u32 i = 0; i < _scene->m_roadCellsSize; ++i)
                {
                    binn item = { 0 };
                    u8 buffItem[CHUNK_SIZE] = { 0 };
                    if (binn_create(&item, BINN_OBJECT, sizeof(buffItem), buffItem))
                    {
                        success &= RoadCellSave(&_scene->m_roadCells[i], &item);
                    }
                    binn_list_add_object(&list, &item);
                }

                binn_object_set_list(&obj, "cells", &list);
            }
        }

        // Enemies
        {
            u8 buffList[CHUNK_SIZE * 10] = { 0 };
            binn list = { 0 };

            if (binn_create(&list, BINN_LIST, sizeof(buffList), buffList))
            {
                for (u32 i = 0; i < _scene->m_enemiesSize; ++i)
                {
                    binn item = { 0 };
                    u8 buffItem[CHUNK_SIZE] = { 0 };
                    if (binn_create(&item, BINN_OBJECT, sizeof(buffItem), buffItem))
                    {
                        success &= EnemySave(&_scene->m_enemies[i], &item);
                    }
                    binn_list_add_object(&list, &item);
                }

                binn_object_set_list(&obj, "enemies", &list);
            }
        }
    }

    if (success)
    {
        SaveFileData("./sceneSaved.dat", binn_ptr(&obj), binn_size(&obj));
    }
}

void SceneLoad(Scene* _scene, const Grid _grid)
{
    i32 dataSize = 0;
    u8* data = LoadFileData("./sceneSaved.dat", &dataSize);
    
    binn obj = { 0 };
    if (binn_load_ex(data, dataSize, &obj))
    {
        // Road Cells
        {
            binn binnList = { 0 };
            void* list = binn_object_list(&obj, "cells");
            binn_load(list, &binnList);
            const u32 count = binn_count(&binnList);

            for (u32 i = 0; i < count; ++i)
            {
                void* itemPtr = NULL;
                binn item = { 0 };
                i32 itemSize = 0;
                binn_list_get(&binnList, i+1, BINN_OBJECT, &itemPtr, &itemSize);
                binn_load(itemPtr, &item);
                RoadCellLoad(&_scene->m_roadCells[i], &item);
            }
            _scene->m_roadCellsSize = count;
        }

        // Enemies
        {
            binn binnList = { 0 };
            void* list = binn_object_list(&obj, "enemies");
            binn_load(list, &binnList);
            const u32 count = binn_count(&binnList);

            _scene->m_enemiesSize = 0;
            memset(&_scene->m_enemies, 0, sizeof(_scene->m_enemies) / sizeof(_scene->m_enemies[0]));

            for (u32 i = 0; i < count; ++i)
            {
                void* itemPtr = NULL;
                binn item = { 0 };
                i32 itemSize = 0;
                binn_list_get(&binnList, i + 1, BINN_OBJECT, &itemPtr, &itemSize);
                binn_load(itemPtr, &item);
                EnemyLoad(&_scene->m_enemies[i], &item);
                _scene->m_enemies[i].m_model = _scene->m_modelEnemy;
            }
            _scene->m_enemiesSize = count;
        }
    }
}

void SceneInit(Scene* _scene, const Grid _grid, const MatCap _matCap)
{
    const Mesh cylinder = GenMeshCylinder(0.25f, 1.f, 16.f);
    _scene->m_modelEnemy = LoadModelFromMesh(cylinder);
    _scene->m_modelEnemy.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture;

    _scene->m_modelTowerArcher1 = LoadModel("resources/models/buildings/towers/elves/ArcherTowerLvl1/ArcherTowerLvl1.obj");
    _scene->m_modelTowerArcher1.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture;
    BuildingInit(
        &_scene->m_towers[0],
        _scene->m_modelTowerArcher1,
        (Vector3) { 0.5f, 0.01f, 0.5f },
        (Vector3) { 0.f, 0.f, 0.f },
        (Vector3) { 0.2f, 0.2f, 0.2f }
    );
    _scene->m_towersSize += 1;

    _scene->m_modelTowerWizard1 = LoadModel("resources/models/buildings/towers/elves/WizardTowerLvl1/WizardTowerLvl1.obj");
    _scene->m_modelTowerWizard1.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture;
    BuildingInit(
        &_scene->m_towers[1],
        _scene->m_modelTowerWizard1,
        (Vector3) { 2.5f, 0.01f, 0.5f },
        (Vector3) { 0.f, 0.f, 0.f },
        (Vector3) { 0.2f, 0.2f, 0.2f }
    );
    _scene->m_towersSize += 1;

    const Mesh modelSphere = GenMeshSphere(0.1f, 10, 10);
    _scene->m_modelProjectile = LoadModelFromMesh(modelSphere);
    _scene->m_modelProjectile.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture2;

    Texture2D icons[] =
    {
        _scene->m_iconSimpleArrow = LoadTexture("resources/textures/icons/arrowhead.png"),
        _scene->m_iconColdArrow = LoadTexture("resources/textures/icons/frozen-arrow.png"),
        _scene->m_iconFireArrow = LoadTexture("resources/textures/icons/flaming-arrow.png"),
        _scene->m_iconToxicArrow = LoadTexture("resources/textures/icons/chemical-arrow.png"),
        _scene->m_iconSniperArrow = LoadTexture("resources/textures/icons/supersonic-arrow.png"),
        _scene->m_iconElectricArrow = LoadTexture("resources/textures/icons/charged-arrow.png"),
    };
    RadialMenuInit(&_scene->m_menuBuildings, icons, sizeof(icons) / sizeof(icons[0]));
}

void SceneUpdate(Scene* _scene, const Camera _gameCam, const Grid _grid, const f32 _dt)
{
    const vec2u32 cellOvered = GridSelect(_grid, GetMousePosition(), _gameCam);

    if (IsKeyDown(KEY_ENTER))
    {
        const Vector3 enemyPos = Utils3DGetPosition(_scene->m_enemies[0].m_model.transform);
        const Vector3 targetPos = enemyPos;
        BuildingShootTo(&_scene->m_towers[0], _grid, targetPos, _scene->m_modelProjectile);
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && _scene->m_displayRadialMenu == false)
    {
        if (GridBoundsCheck(_grid, cellOvered))
        {
            _scene->m_displayRadialMenu = true;
        }
    }
    else if (IsKeyPressed(KEY_P))
    {
        SceneSpawnEnemy(_scene, _grid, cellOvered);
    }
    else if (IsKeyPressed(KEY_F7))
    {
        SceneSave(_scene);
    }
    else if (IsKeyPressed(KEY_F8))
    {
        SceneLoad(_scene, _grid);
    }

    const KeyboardKey keyPressed = GetKeyPressed();

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

        const vec2u32 cell = _scene->m_enemies[i].m_cell;
        u32 foundIndex = IndexInvalid;

        for (u32 j = 0; j < _scene->m_roadCellsSize; ++j)
        {
            if (RoadCellExistsAt(&_scene->m_roadCells[j], _grid, cell))
            {
                foundIndex = j;
                break;
            }
        }

        if (foundIndex != IndexInvalid)
        {
            EnemyMoveTo(&_scene->m_enemies[i], _scene->m_roadCells[foundIndex].m_dir, _grid);
        }
    }

    for (u32 i = 0; i < _scene->m_towersSize; ++i)
    {
        BuildingUpdate(&_scene->m_towers[i], _dt);
    }

    RadialMenuUpdate(&_scene->m_menuBuildings, _dt, &_scene->m_displayRadialMenu, GetMousePosition());
}

void SceneRender(Scene* _scene, ShaderOutline* _shaderOutline, const Camera _gameCam, const Grid _grid, const vec2u32 _cellOvered, MatCap _matCap)
{
    const Vector3 cellOveredToWorld = GridWorldPosFromIndex(_grid, _cellOvered);
    ShaderOutlineUpdate(_shaderOutline, _gameCam, cellOveredToWorld);
    f32 pickingColor[3] = { 1.f, 1.f, 1.f };

    for (u32 i = 0; i < _scene->m_towersSize; ++i)
    {
        const Vector3 position = Utils3DGetPosition(_scene->m_towers[i].m_model.transform);
        const vec2u32 cellPos = GridIndexFromWorldPos(_grid, position);

        if (cellPos.m_x == _cellOvered.m_x && cellPos.m_y == _cellOvered.m_y && _scene->m_displayRadialMenu == false)
        {
            convertUint16ToFloat3Array((u16)i, pickingColor);
            pickingColor[0] /= 255.f;
            pickingColor[1] /= 255.f;
            pickingColor[2] /= 255.f;
        }

        BuildingRender(&_scene->m_towers[i], _matCap, _scene->m_modelProjectile);
    }
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locColorPicker, &pickingColor, SHADER_UNIFORM_VEC3);

    MatCapUpdate(&_matCap, 0.1f, 0.99f);
    for (u32 i = 0; i < _scene->m_enemiesSize; ++i)
    {
        const Shader tmp = _scene->m_enemies[i].m_model.materials[0].shader;
        _scene->m_enemies[i].m_model.materials[0].shader = _matCap.m_shader;
        DrawModel(_scene->m_enemies[i].m_model, (Vector3) { 0.f, 0.f, 0.f }, 1.f, WHITE);
        _scene->m_enemies[i].m_model.materials[0].shader = tmp;
    }

    for (u32 i = 0; i < _scene->m_roadCellsSize; ++i)
    {
        RoadCellRender(&_scene->m_roadCells[i], _grid);
    }
}

void SceneRenderUI(Scene* _scene, const Font _font)
{
    RadialMenuRender(&_scene->m_menuBuildings);

    // Draw FPS
    {
        char buff[100] = { 0 };
        snprintf(buff, 100, "%.0f FPS", 1.f / GetFrameTime());
    
        const Color darkGray = { DARKGRAY.r, DARKGRAY.g, DARKGRAY.b, 80 };
        const Color black = { BLACK.r, BLACK.g, BLACK.b, 80 };
        const Rectangle panel = { GetScreenWidth() - 130.f, 0.f, 130.f, 50.f };
        DrawRectangleGradientEx(panel, darkGray, black, black, darkGray);
        DrawRectangleLines(panel.x, panel.y, panel.width, panel.height, black);

        DrawTextEx(_font, buff, (Vector2) { GetScreenWidth() - 110.f, 10.f }, 24, 1.f, WHITE);
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
