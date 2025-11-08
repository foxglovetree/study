#pragma once

#include <Ogre.h>
#include "PathFollow2.h"
#include "util/CellUtil.h"
#include "util/CostMap.h"
using namespace Ogre;
#include "State.h"
class ActorState : public State
{
    Ogre::Entity *entity;
public:
    ActorState(State *parent) : State(parent)
    {
    }

    void setEntity(Ogre::Entity *entity){
        this->entity = entity;
    }
    Ogre::Entity *getEntity(){
        return this->entity;
    }


};