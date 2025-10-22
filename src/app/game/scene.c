#pragma once

#include "../../includes.c"

typedef struct Scene
{
    Building m_buildings[10];
    Model m_archerTowerLvl1;
    Model m_wizardTowerLvl1;

    ShaderFlatColor m_shaderFlatColor;
    ShaderOutline m_shaderOutline;

    RadialMenu m_menuBuildings;
} Scene;

void SceneInit(Scene* _scene)
{
    _scene->m_archerTowerLvl1 = LoadModel("resources/models/buildings/towers/elves/ArcherTowerLvl1/ArcherTowerLvl1.obj");
    _scene->m_wizardTowerLvl1 = LoadModel("resources/models/buildings/towers/elves/WizardTowerLvl1/WizardTowerLvl1.obj");

    ShaderFlatColorInit(&_scene->m_shaderFlatColor);
    ShaderOutlineInit(&_scene->m_shaderOutline);
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



void SceneRenderFlat(Scene* _scene)
{

    {
        const u16 id = 256;
        ShaderFlatColorUpdate(&_scene->m_shaderFlatColor, id);
        const Shader tmp = _scene->m_archerTowerLvl1.materials[0].shader;
        _scene->m_archerTowerLvl1.materials[0].shader = _scene->m_shaderFlatColor.m_shader;

        const Matrix matScale = MatrixScale(0.2f, 0.2f, 0.2f);
        const Matrix matRotation = MatrixRotateXYZ((Vector3) { 0.f, 0.f, 0.f });
        const Matrix matTranslation = MatrixTranslate(0.f, 0.f, 0.f);
        const Matrix transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

        DrawModelEx(_scene->m_archerTowerLvl1, Utils3DGetPosition(transform), Utils3DGetRotation(transform), 0.f, Utils3DGetScale(transform), WHITE);
        _scene->m_archerTowerLvl1.materials[0].shader = tmp;
    }

    {
        const u16 id = 128;
        ShaderFlatColorUpdate(&_scene->m_shaderFlatColor, id);
        const Shader tmp = _scene->m_wizardTowerLvl1.materials[0].shader;
        _scene->m_wizardTowerLvl1.materials[0].shader = _scene->m_shaderFlatColor.m_shader;
        DrawModel(_scene->m_wizardTowerLvl1, (Vector3) { 2.f, 0.f, 0.f }, 0.2f, WHITE);
        _scene->m_wizardTowerLvl1.materials[0].shader = tmp;
    }
}