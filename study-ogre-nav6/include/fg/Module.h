#pragma once
#include <Ogre.h>
#include <OgreInput.h>
#include "Core.h"

using namespace Ogre;
using namespace OgreBites;
class Module
{
protected:
public:
    virtual void active(Core *core)
    {
    }
};
