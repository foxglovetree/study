#pragma once
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/example/ActorState.h"
#include "fg/Core.h"

using namespace Ogre;

class ActorStateControl : public ActorState
{
    CostMap *costMap;

public:
    ActorStateControl(State *parent, CostMap *costMap, Core *core) : ActorState(parent, costMap, core)
    {

        SceneManager *sMgr = core->getSceneManager();
        entity = sMgr->createEntity("Sinbad.mesh");
        entity->setQueryFlags(0x00000001);
        
        sceNode = sMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * ACTOR_HEIGHT);
        sceNode->setScale(ACTOR_SCALE, ACTOR_SCALE, ACTOR_SCALE);
        sceNode->attachObject(entity);
        sceNode->translate(0, ACTOR_HEIGHT , 0);
        // todo collect auto
        this->setSceneNode(sceNode);
        
                
    }

};