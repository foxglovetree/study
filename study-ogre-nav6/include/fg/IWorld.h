
#pragma once
#include <iostream>
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "util/CostMap.h"
#include <unordered_map>
#include "util/CellUtil.h"
#include "State.h"

using namespace Ogre;
using namespace std;
// root state & control.
class IWorld
{

public:
    virtual CostMap * getCostMap() = 0;
    
};