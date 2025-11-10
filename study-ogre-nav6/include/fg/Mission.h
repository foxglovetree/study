#pragma
#include <Ogre.h>
#include <OgreFrameListener.h>
#include <OgreAnimationState.h>
#include <vector>
#include "PathFollow2.h"

class Mission
{
public:
    class Listener
    {
    public:
        virtual void missionDone(Mission *mission) = 0;
    };

protected:
    bool done = false;
    std::vector<Mission::Listener *> listeners;

public:
    Mission()
    {
    }

    bool isDone()
    {
        return this->done;
    }
    void add(Mission::Listener* l){
        listeners.push_back(l);
    }

    bool setDone(bool done)
    {
        this->done = done;
        if (this->done)
        {
            for (Mission::Listener *l : listeners)
            {
                l->missionDone(this);
            }
        }
    }
};

/**
 * Move a node to a destination.
 */
class PathFollow2Mission : public Mission, public FrameListener
{
    PathFollow2 *path;
    SceneNode *node;
    AnimationStateSet *aniSet;

public:
    PathFollow2Mission(SceneNode *node, PathFollow2 *path, AnimationStateSet *aniSet, std::vector<std::string> &aniNames)
    {
        this->node = node;
        this->path = path;
        this->aniSet = aniSet;
        for (std::string name : aniNames)
        {
            AnimationState* as = this->aniSet->getAnimationState(name);
            as->setEnabled(true);
            as->setLoop(true);
            as->setWeight(1.0f);
        }
    }

    PathFollow2 *getPath()
    {
        return this->path;
    }

    bool frameStarted(const Ogre::FrameEvent &evt) override
    {

        PathFollow2 *pathFollow = this->getPath();

        Vector2 pos;
        if (pathFollow->move(evt.timeSinceLastFrame, pos))
        {
            Vector3 pos0 = node->getPosition();
            node->translate(pos.x - pos0.x, 0, pos.y - pos0.z); // new position
            // animation
            AnimationStateIterator it = this->aniSet->getAnimationStateIterator();
            while (it.hasMoreElements())
            {
                AnimationState *as = it.getNext();
                as->addTime(evt.timeSinceLastFrame);
            }
            return false;
        }
        this->setDone(true);
        return true;
    }
};