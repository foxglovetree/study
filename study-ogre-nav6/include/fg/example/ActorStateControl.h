#pragma once
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "fg/State.h"
#include "fg/example/ActorState.h"
#include "fg/Core.h"
#define CHAR_HEIGHT 5
#define SCALE 5.0f
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
        
        sceNode = sMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT * SCALE);
        sceNode->setScale(SCALE, SCALE, SCALE);
        sceNode->attachObject(entity);
        sceNode->translate(0, SCALE * CHAR_HEIGHT , 0);
        // todo collect auto
        this->setSceneNode(sceNode);
        
                
    }

};