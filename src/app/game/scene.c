#pragma once

#include "../../includes.c"

typedef struct Scene
{
    Building m_buildings[10];

    Model m_models[10];
    u32 m_modelSize;

    ShaderFlatColor m_shaderFlatColor;
    ShaderOutline m_shaderOutline;

    RenderTexture2D renderTarget;

    RadialMenu m_menuBuildings;
} Scene;

u32 ModelGetID(const char* _stringLiteral)
{
    Scene* scenePtr = NULL;
    const u32 id = (uPtr)_stringLiteral % sizeof(scenePtr->m_models) / sizeof(scenePtr->m_models[0]);
    return id;
}

void SceneInit(Scene* _scene)
{
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

void SceneUpdate(Scene* _scene, const Camera _gameCam, const f32 _dt)
{
    RadialMenuUpdate(&_scene->m_menuBuildings, _dt);
}

void SceneRender(Scene* _scene, const Camera _gameCam, const Grid _grid, const vec2u32 _cellOvered)
{
    //const Matrix matScale = MatrixScale(0.2f, 0.2f, 0.2f);
    //const Matrix matRotation = MatrixRotateXYZ((Vector3) { 0.f, 0.f, 0.f });
    //const Matrix matTranslation = MatrixTranslate(0.5f, 0.f, 0.5f);
    //const Matrix transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
    //

    const Vector3 cellOveredToWorld = GridWorldPosFromIndex(_grid, _cellOvered);
    ShaderOutlineUpdate(&_scene->m_shaderOutline, _gameCam, cellOveredToWorld);

    for (u32 i = 0; i < _scene->m_modelSize; ++i)
    {
        const Vector3 position = Utils3DGetPosition(_scene->m_models[i].transform);
        const vec2u32 cellPos = GridIndexFromWorldPos(_grid, position);

        if (cellPos.m_x == _cellOvered.m_x && cellPos.m_y == _cellOvered.m_y)
        {

            f32 pickingColor[3] = { 0.f };
            convertUint16ToFloat3Array((u16)i, pickingColor);
            pickingColor[0] /= 255.f;
            pickingColor[1] /= 255.f;
            pickingColor[2] /= 255.f;
            SetShaderValue(_scene->m_shaderOutline.m_shader, _scene->m_shaderOutline.m_locColorPicker, &pickingColor, SHADER_UNIFORM_VEC3);
        }

        DrawModel(_scene->m_models[i], (Vector3) { 0.f, 0.f, 0.0f }, 1.f, WHITE);
    }

    //// Archer tower
    //{
    //    const Matrix matScale = MatrixScale(0.2f, 0.2f, 0.2f);
    //    const Matrix matRotation = MatrixRotateXYZ((Vector3) { 0.f, 0.f, 0.f });
    //    const Matrix matTranslation = MatrixTranslate(0.5f, 0.f, 0.5f);
    //    const Matrix transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

    //    /*for (int i = 0; i < _scene->m_archerTowerLvl1.meshCount; i++) 
    //    {
    //        DrawMeshInstanced(_scene->m_archerTowerLvl1.meshes[i], _scene->m_archerTowerLvl1.materials[_scene->m_archerTowerLvl1.meshMaterial[i]], &transform, 1);
    //    }*/

    //    const vec2u32 hightlightedCell = GridIndexFromWorldPos(&grid, Utils3DGetPosition(transform));
    //    DrawModelEx(_scene->m_archerTowerLvl1, Utils3DGetPosition(transform), Utils3DGetRotation(transform), 0.f, Utils3DGetScale(transform), WHITE);
    //}

    //DrawModel(_scene->m_wizardTowerLvl1, (Vector3) { 2.5f, 0.f, 0.5f }, 0.2f, WHITE);
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