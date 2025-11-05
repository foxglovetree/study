
#include "StateControl.h"
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "State.h"

#define CHAR_HEIGHT 5
#define SCALE 2.0f
using namespace Ogre;

class ActorStateControl : Ogre::FrameListener, StateControl
{
    Entity *obj;
    CostMap *costMap;
    State *state;
    SceneNode *node;

public:
    ActorStateControl(CostMap *costMap, SceneManager *sceneMgr)
    {
        this->costMap = costMap;
        obj = sceneMgr->createEntity("Sinbad.mesh");
        obj->setQueryFlags(0x00000001);
        this->state = new State(State::Type::ACTOR);
        obj->setUserAny(Any(state));

        node = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT * SCALE);
        node->setScale(SCALE, SCALE, SCALE);
        node->attachObject(obj);
    }
    State *getState()
    {
        return this->state;
    }

    SceneNode *getNode()
    {
        return node;
    }
    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        if (this->state->isActive())
        {
            
        }
        else
        {
        }
        return true;
    }
};