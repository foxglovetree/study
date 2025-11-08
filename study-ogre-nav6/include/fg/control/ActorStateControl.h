#pragma once
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "fg/State.h"
#include "fg/ActorState.h"
#include "fg/StateControl.h"
#define CHAR_HEIGHT 5
#define SCALE 2.0f
using namespace Ogre;

class ActorStateControl : public Ogre::FrameListener, public StateControl<ActorState>
{
    Entity *obj;
    CostMap *costMap;
    
    SceneNode *node;
    
public:
    ActorStateControl(ActorState * actor):StateControl(actor)
    {
    }
    void init(InitContext &ctx) override
    {
        CostMap *costMap = this->find<CostMap>();
        SceneManager *sceneMgr = this->find<Ogre::SceneManager>();
        obj = sceneMgr->createEntity("Sinbad.mesh");
        obj->setQueryFlags(0x00000001);

        obj->setUserAny(Any((State*)state));
        this->state->setEntity(obj);
        
        node = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT * SCALE);
        node->setScale(SCALE, SCALE, SCALE);
        node->attachObject(obj);
    }
   

    SceneNode *getNode()
    {
        return node;
    }
    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        if (this->state->isActive())
        {
            PathFollow2 *pathFollow = this->state->getPath();
            if (pathFollow != nullptr)
            {
                Vector2 pos;
                if (pathFollow->move(evt.timeSinceLastFrame, pos))
                {

                    node->setPosition(pos.x, 0, pos.y);
                }
                else
                {
                    state->setPath(nullptr);
                }
            }
        }
        else
        {
        }
        return true;
    }
};