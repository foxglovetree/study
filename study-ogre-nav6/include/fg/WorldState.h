#pragma once
#include "State.h"
#include "util/Polygon2.h"
#include "Ground.h"
class WorldState : public State
{
protected:
    Ground *ground;
public:
    WorldState(Ground * shape)
    {
        this->ground = shape;
    }

    Ground *getGround()
    {
        return ground;
    }
};