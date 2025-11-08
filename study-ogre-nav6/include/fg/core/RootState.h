#pragma once

#include <Ogre.h>
#include "fg/PathFollow2.h"
#include "fg/util/CellUtil.h"
#include "fg/util/CostMap.h"
using namespace Ogre;
#include "fg/State.h"
#include "WorldState.h"
class RootState : public State
{
    Ogre::Root *root;
    OgreBites::ApplicationContext *appCtx;
    WorldState * world;
public:
    RootState() : State(nullptr)
    {
        this->world = new WorldState(this);
    }
    WorldState * getWorld()
    {
        return this->world;
    }
    void set(OgreBites::ApplicationContext *appCtx)
    {
        this->appCtx = appCtx;
        this->root = appCtx->getRoot();
    }
    OgreBites::ApplicationContext *getAppContext()
    {
        return this->appCtx;
    }
    Ogre::Root *getRoot() override
    {
        return this->root;
    }
};