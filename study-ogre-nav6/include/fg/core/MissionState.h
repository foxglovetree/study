#pragma
#include <Ogre.h>
#include <OgreFrameListener.h>
#include <OgreAnimationState.h>
#include <vector>
#include "fg/State.h"
#include "fg/PathFollow2.h"

class MissionState : public State
{
public:
    class Listener
    {
    public:
        virtual void missionDone(MissionState *mission) = 0;
    };

protected:
    bool done = false;
    std::vector<MissionState::Listener *> listeners;

public:
    MissionState() 
    {
    }

    bool isDone()
    {
        return this->done;
    }
    void add(MissionState::Listener *l)
    {
        listeners.push_back(l);
    }

    void setDone(bool done)
    {
        this->done = done;
        if (this->done)
        {
            for (MissionState::Listener *l : listeners)
            {
                l->missionDone(this);
            }
        }
    }
};
