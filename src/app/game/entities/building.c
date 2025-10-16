#pragma once

#include "../../../includes.c"

typedef struct Building
{
    Matrix m_transform;
    u32 m_uid;
    char* m_modelID;
} Building;

void BuildingInit(Building* _building)
{

}

void BuildingUpdate(Building* _building, const f32 _dt)
{

}

void BuildingRender(Building* _building)
{

}