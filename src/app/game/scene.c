#pragma once

#include "../../includes.c"

typedef struct Scene
{
    Building m_buildings[10];
    Model m_archerTowerLvl1;
    Model m_wizardTowerLvl1;

    Material m_materialFlatColor;
    u32 m_shaderFlatColorLoc;

    RadialMenu m_menuBuildings;
} Scene;

void SceneInit(Scene* _scene)
{
    _scene->m_archerTowerLvl1 = LoadModel("resources/models/buildings/towers/elves/ArcherTowerLvl1/ArcherTowerLvl1.obj");
    _scene->m_wizardTowerLvl1 = LoadModel("resources/models/buildings/towers/elves/WizardTowerLvl1/WizardTowerLvl1.obj");

    _scene->m_materialFlatColor = LoadMaterialDefault();
    UnloadShader(_scene->m_materialFlatColor.shader);
    _scene->m_materialFlatColor.shader = LoadShader(NULL, "resources/shaders/flatColor.fs");
    _scene->m_shaderFlatColorLoc = GetShaderLocation(_scene->m_materialFlatColor.shader, "color");

    RadialMenuInit(&_scene->m_menuBuildings);
}

void SceneUpdate(Scene* _scene, const f32 _dt)
{
    RadialMenuUpdate(&_scene->m_menuBuildings, _dt);
}

void SceneRender(Scene* _scene)
{
    // Archer tower
    {
        const Matrix matScale = MatrixScale(0.2f, 0.2f, 0.2f);
        const Matrix matRotation = MatrixRotateXYZ((Vector3) { 0.f, 0.f, 0.f });
        const Matrix matTranslation = MatrixTranslate(0.f, 0.f, 0.f);
        const Matrix transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

        /*for (int i = 0; i < _scene->m_archerTowerLvl1.meshCount; i++) 
        {
            DrawMeshInstanced(_scene->m_archerTowerLvl1.meshes[i], _scene->m_archerTowerLvl1.materials[_scene->m_archerTowerLvl1.meshMaterial[i]], &transform, 1);
        }*/
        DrawModelEx(_scene->m_archerTowerLvl1, Utils3DGetPosition(transform), Utils3DGetRotation(transform), 0.f, Utils3DGetScale(transform), WHITE);
    }

    DrawModel(_scene->m_wizardTowerLvl1, (Vector3) { 2.f, 0.f, 0.f }, 0.2f, WHITE);
}

void SceneRenderUI(Scene* _scene)
{
    RadialMenuRender(&_scene->m_menuBuildings);
}

void convertUint16ToFloat3Array(u16 _input, f32 _output[3])
{
    _output[0] = (f32)(_input & 0xFF);
    _output[1] = (f32)((_input >> 8) & 0xFF);
    _output[2] = (f32)((_input >> 4) & 0xFF);
}

void SceneRenderFlat(Scene* _scene)
{
    //const u16 id = 256;
    //f32 color[4] = { 0.f, [3] = 1.f }; // [3] = 1.f for alpha to be 255
    //convertUint16ToFloat3Array(id, color);
    //SetShaderValue(_scene->m_materialFlatColor.shader, _scene->m_shaderFlatColorLoc, &color, SHADER_UNIFORM_VEC4);
    //const Matrix matScale = MatrixScale(0.2f, 0.2f, 0.2f);
    //const Matrix matRotation = MatrixRotateXYZ((Vector3) { 0.f, 0.f, 0.f });
    //const Matrix matTranslation = MatrixTranslate(0.f, 0.f, 0.f);
    //const Matrix transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
    //const Mesh mesh = _scene->m_archerTowerLvl1.meshes[0];
    //DrawMeshInstanced(mesh, _scene->m_materialFlatColor, &transform, 1);
}