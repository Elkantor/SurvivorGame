#pragma once

#include "../../includes.c"

#define ENEMY_COUNT 20

typedef struct Scene
{
    Enemy m_enemies[ENEMY_COUNT];
    u32 m_enemiesAnimFrames[ENEMY_COUNT];
    u32 m_enemiesSize;

    Building m_towers[10];
    u32 m_towersSize;

    RoadCell m_roadCells[50];
    u32 m_roadCellsSize;

    Model m_modelEnemy;
    Model m_modelTowerArcher1;
    Model m_modelTowerWizard1;
    Model m_modelProjectile;
    Model m_modelSkeletonWarrior;
    Model m_modelPath;
    Model m_modelGrass2;

    ModelAnimation* m_modelAnimSkeletonWarrior;
    i32 m_modelAnimSkeletonWarriorCount;

    Texture m_iconSimpleArrow;
    Texture m_iconColdArrow;
    Texture m_iconFireArrow;
    Texture m_iconToxicArrow;
    Texture m_iconSniperArrow;
    Texture m_iconElectricArrow;

    RadialMenu m_menuBuildings;
    bool m_displayRadialMenu;
} Scene;

void SceneSpawnEnemy(Scene* _scene, const Grid _grid, const vec2u32 _spawnCell)
{
    const u32 capacity = ENEMY_COUNT;

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

void SceneSave(const Scene* _scene, Arena* _frameArena)
{
    const i32 buffSize = 204800;
    u8* buff = ArenaPush(_frameArena, buffSize);
    binn obj = { 0 };

    bool success = true;

    if (binn_create(&obj, BINN_OBJECT, buffSize, buff))
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

    ArenaPop(_frameArena, buff);
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
    // Skeleton Warrior
    {
        const Texture textureSkeleton = LoadTexture("resources/textures/enemies/skeleton_texture.png");
        _scene->m_modelEnemy = LoadModel("resources/models/enemies/SkeletonWarrior.glb");
        const Vector3 pos = { 2.5f, 0.f, 1.5f };
        const Vector3 rotation = { 0.f, 0.f, 0.f };
        const Vector3 scale = { 0.35f, 0.35f, 0.35f };
        _scene->m_modelEnemy.transform = Utils3DCreateTransform(pos, rotation, scale);
        _scene->m_modelEnemy.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = textureSkeleton;
        _scene->m_modelEnemy.materials[1].maps[MATERIAL_MAP_ALBEDO].texture = textureSkeleton;
        _scene->m_modelEnemy.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture;
        _scene->m_modelEnemy.materials[1].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture;
    }

    _scene->m_modelTowerArcher1 = LoadModel("resources/models/buildings/towers/elves/ArcherTowerLvl1/ArcherTowerLvl1.obj");
    _scene->m_modelTowerArcher1.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture;
    BuildingInit(
        &_scene->m_towers[0],
        _scene->m_modelTowerArcher1,
        (Vector3) { 0.5f, 0.01f, 0.5f },
        (Vector3) { 0.f, 0.f, 0.f },
        (Vector3) { 0.25f, 0.25f, 0.25f }
    );
    _scene->m_towersSize += 1;

    _scene->m_modelTowerWizard1 = LoadModel("resources/models/buildings/towers/elves/WizardTowerLvl1/WizardTowerLvl1.obj");
    _scene->m_modelTowerWizard1.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture;
    BuildingInit(
        &_scene->m_towers[1],
        _scene->m_modelTowerWizard1,
        (Vector3) { 2.5f, 0.01f, 0.5f },
        (Vector3) { 0.f, 0.f, 0.f },
        (Vector3) { 0.25f, 0.25f, 0.25f }
    );
    _scene->m_towersSize += 1;

    const Mesh modelSphere = GenMeshSphere(0.1f, 10, 10);
    _scene->m_modelProjectile = LoadModelFromMesh(modelSphere);
    _scene->m_modelProjectile.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture2;

    const Texture textureElven = LoadTexture("resources/textures/alts/PolygonElven_Texture_02_A.png");

    // Path
    {
        _scene->m_modelPath = LoadModel("resources/models/props/Path.obj");
        const Matrix transform = _scene->m_modelPath.transform;
        const vec2u32 posGrid = { 53, 53 };
        const Vector3 pos = GridWorldPosFromIndex(_grid, posGrid);
        const Vector3 rotation = Utils3DGetRotation(transform);
        const Vector3 scale = { 0.4f, 0.4f, 0.4f };
        _scene->m_modelPath.transform = Utils3DCreateTransform(pos, rotation, scale);
        _scene->m_modelPath.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = textureElven;
        _scene->m_modelPath.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture;
    }

    // Grass2
    {
        _scene->m_modelGrass2 = LoadModel("resources/models/props/Grass02.obj");
        const Matrix transform = _scene->m_modelGrass2.transform;
        const vec2u32 posGrid = { 53, 53 };
        const Vector3 pos = GridWorldPosFromIndex(_grid, posGrid);
        const Vector3 rotation = Utils3DGetRotation(transform);
        const Vector3 scale = { 0.4f, 0.4f, 0.4f };
        _scene->m_modelGrass2.transform = Utils3DCreateTransform(pos, rotation, scale);
        _scene->m_modelGrass2.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = textureElven;
        _scene->m_modelGrass2.materials[0].maps[MATERIAL_MAP_METALNESS].texture = _matCap.m_texture;
    }

    // Model anims
    _scene->m_modelAnimSkeletonWarrior = LoadModelAnimations("resources/models/enemies/Rig_Medium_MovementBasic2.glb", &_scene->m_modelAnimSkeletonWarriorCount);

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

void SceneUpdate(Scene* _scene, const Camera _gameCam, const Grid _grid, const f32 _dt, Arena* _frameArena)
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
        SceneSave(_scene, _frameArena);
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
        EnemyUpdate(&_scene->m_enemies[i], _grid, _dt, _scene->m_roadCells, _scene->m_roadCellsSize);

        // Update ennemy model anims
        const f32 updateRate = 1000.f / (_dt * 1000.f);
        const f32 realFrameRate = updateRate / 24.f;
        const ModelAnimation anim = _scene->m_modelAnimSkeletonWarrior[6];
        const f32 currentFrame = (_scene->m_enemiesAnimFrames[i] + realFrameRate);
        _scene->m_enemiesAnimFrames[i] = ((i32)currentFrame) % anim.frameCount;
        UpdateModelAnimation(_scene->m_enemies[i].m_model, anim, _scene->m_enemiesAnimFrames[i]);
    }

    for (u32 i = 0; i < _scene->m_towersSize; ++i)
    {
        BuildingUpdate(&_scene->m_towers[i], _dt);
    }

    RadialMenuUpdate(&_scene->m_menuBuildings, _dt, &_scene->m_displayRadialMenu, GetMousePosition());
}

void SceneRenderShadows(Scene* _scene)
{
    for (u32 i = 0; i < _scene->m_towersSize; ++i)
    {
        BuildingRenderSimple(&_scene->m_towers[i]);
    }
    for (u32 i = 0; i < _scene->m_enemiesSize; ++i)
    {
        EnemyRender(&_scene->m_enemies[i]);
    }
}

void SceneRender(Scene* _scene, ShaderOutline* _shaderOutline, const Camera _gameCam, const Grid _grid, const vec2u32 _cellOvered, MatCap _matCap)
{
    const Vector3 cellOveredToWorld = GridWorldPosFromIndex(_grid, _cellOvered);
    ShaderOutlineUpdate(_shaderOutline, _gameCam, cellOveredToWorld);
    f32 pickingColor[3] = { 1.f, 1.f, 1.f };

    MatCapUpdate(&_matCap, 0.5f, 1.0f, 5.f, 1.f, ORANGE);
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

        const Shader tmp = _scene->m_towers[i].m_model.materials[0].shader;
        _scene->m_towers[i].m_model.materials[0].shader = _matCap.m_shader;
        BuildingRender(&_scene->m_towers[i], _matCap);
        _scene->m_towers[i].m_model.materials[0].shader = tmp;
    }
    SetShaderValue(_shaderOutline->m_shader, _shaderOutline->m_locColorPicker, &pickingColor, SHADER_UNIFORM_VEC3);
    
    MatCapUpdate(&_matCap, 0.6f, 1.f, 20.f, 10.f, ORANGE);
    for (u32 i = 0; i < _scene->m_enemiesSize; ++i)
    {
        const Shader tmp = _scene->m_enemies[i].m_model.materials[0].shader;
        _scene->m_enemies[i].m_model.materials[0].shader = _matCap.m_shader;
        _scene->m_enemies[i].m_model.materials[1].shader = _matCap.m_shader;
        EnemyRender(&_scene->m_enemies[i]);
        _scene->m_enemies[i].m_model.materials[0].shader = tmp;
        _scene->m_enemies[i].m_model.materials[1].shader = tmp;
    }

    _scene->m_modelPath.materials[0].shader = _matCap.m_shader;
    MatCapUpdate(&_matCap, 0.5f, 1.f, 10.f, 2.f, ORANGE);
    for (u32 i = 0; i < _scene->m_roadCellsSize; ++i)
    {
        RoadCellRender(&_scene->m_roadCells[i], _grid);
        const Vector3 worldPos = GridWorldPosFromIndex(_grid, _scene->m_roadCells[i].m_cell);
        {
            const Matrix transform = _scene->m_modelPath.transform;
            const Vector3 rotation = Utils3DGetRotation(transform);
            const Vector3 scale = Utils3DGetScale(transform);
            _scene->m_modelPath.transform = Utils3DCreateTransform(worldPos, rotation, scale);
        }
        DrawModel(_scene->m_modelPath, (Vector3) { -0.5f, 0.f, 0.5f }, 1.f, WHITE);
    }

}

void SceneRenderUI(Scene* _scene, const Font _font, const Camera _camera)
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

    for (u32 i = 0; i < _scene->m_enemiesSize; ++i)
    {
        EnemyRenderUI(&_scene->m_enemies[i], _camera, _scene->m_modelAnimSkeletonWarrior, _scene->m_enemiesAnimFrames[i]);
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
