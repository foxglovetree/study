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
    virtual std::string getName() = 0;
    virtual void active(Core *core) = 0;
};
