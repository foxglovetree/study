#pragma once
#include "OgreMovableObject.h"
// marker interface
using namespace Ogre;
class Pickable
{
public:
    virtual bool afterPick(MovableObject *) = 0;
};
