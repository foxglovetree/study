#pragma once

#include <Ogre.h>
#include "PathFollow2.h"
#include "util/CellUtil.h"
#include "util/CostMap.h"
using namespace Ogre;

class State
{
public:
    enum Type
    {
        ACTOR
    };

private:
    Type type;
    bool active = false;

    PathFollow2 *pathFolow = nullptr;

public:
    State(Type type)
    {
        this->type = type;
    }

    void setActive(bool active)
    {
        this->active = active;
    }

    bool isActive()
    {
        return this->active;
    }
    PathFollow2 *getPath()
    {
        return this->pathFolow;
    }
    void setPath(PathFollow2 *path)
    {
        this->pathFolow = path;
    }

    bool isType(Type type)
    {
        return this->type == type;
    }
};