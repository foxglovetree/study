#pragma once

#include "fg/State.h"
#include "fg/PathFollow2.h"
#include "fg/util/CellUtil.h"
#include "fg/util/CostMap.h"
#include "ActorState.h"
using namespace Ogre;

class WorldState : public State
{
    

public:
    WorldState(State *parent) : State(parent)
    {
    }

};