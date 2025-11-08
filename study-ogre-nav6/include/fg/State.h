#pragma once

#include <Ogre.h>
#include "PathFollow2.h"
#include "util/CellUtil.h"
#include "util/CostMap.h"
using namespace Ogre;

class State
{
private:
    bool active = false;

    PathFollow2 *pathFolow = nullptr;
    State *parent;

public:
    State(State *parent)
    {
        this->parent = parent;
    }

    virtual Ogre::Root *getRoot()
    {
        if (this->parent)
        {
            return this->parent->getRoot();
        }
        return nullptr;
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
};