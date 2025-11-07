
#pragma once
#include <iostream>
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "util/CostMap.h"
#include "CellStateControl.h"
#include "InputState.h"
#include "PathStateControl.h"
#include "CellMarkStateControl.h"
#include <unordered_map>
#include "ActorStateControl.h"
#include "util/CellUtil.h"
#include "State.h"

using namespace Ogre;
using namespace std;
// root state & control.
class IWorld
{

public:
    virtual CostMap * getCostMap() = 0;
    virtual void setTargetByCell(CellKey cKey) = 0;
    virtual void pickActorByRay(Ray &ray) = 0;
};