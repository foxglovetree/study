#pragma once
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "fg/State.h"
#include "fg/example/ActorState.h"
#include "fg/Core.h"
#define CHAR_HEIGHT 5
#define SCALE 2.0f
using namespace Ogre;

class ActorStateControl : public Ogre::FrameListener, public ActorState
{
    Entity *obj;
    CostMap *costMap;
    
    SceneNode *node;
    
    
    public:
    ActorStateControl(State *parent, CostMap *costMap, Core* core):ActorState(parent, costMap, core)
    {       
        
        SceneManager * sMgr = core->getSceneManager();
        obj = sMgr->createEntity("Sinbad.mesh");
        obj->setQueryFlags(0x00000001);

        obj->setUserAny(Any((State*)this));
        this->setEntity(obj);
        
        node = sMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT * SCALE);
        node->setScale(SCALE, SCALE, SCALE);
        node->attachObject(obj);
    }
   

    SceneNode *getNode()
    {
        return node;
    }
    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        if (this->isActive())
        {
            PathFollow2 *pathFollow = this->getPath();
            if (pathFollow != nullptr)
            {
                Vector2 pos;
                if (pathFollow->move(evt.timeSinceLastFrame, pos))
                {

                    node->setPosition(pos.x, 0, pos.y);
                }
                else
                {
                    setPath(nullptr);
                }
            }
        }
        else
        {
        }
        return true;
    }
};