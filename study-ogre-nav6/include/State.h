#pragma once
#include <Ogre.h>

using namespace Ogre;

class State
{
public:
    enum Type
    {
        ACTOR
    };

private:
    Type type;
    bool active = false;

    Vector3 target;

public:
    State(Type type)
    {
        this->type = type;
    }

    void setActive(bool active)
    {
        this->active = active;
    }

    bool isActive()
    {
        return this->active;
    }

    void setTarget(const Vector3 &target)
    {
        this->target = target;
    }

    const Vector3 &getTarget()
    {
        return this->target;
    }

    bool isType(Type type)
    {
        return this->type == type;
    }
};