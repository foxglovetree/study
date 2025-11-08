#pragma once

#include "State.h"
#include "PathFollow2.h"
#include "util/CellUtil.h"
#include "util/CostMap.h"
#include "ActorState.h"
using namespace Ogre;

class WorldState : public State
{
    ActorState *actorState;

public:
    WorldState(State *parent) : State(parent)
    {
        actorState = new ActorState(this);
    }

    ActorState *getActorState()
    {
        return actorState;
    }
};