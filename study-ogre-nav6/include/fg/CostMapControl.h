
#pragma once
#include <type_traits>
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/defines.h"
#include "fg/util/CostMap.h"
#include "fg/util/Component.h"
#include "fg/State.h"
#include "fg/MaterialNames.h"
#include "fg/util/DrawerUtil.h"

using namespace Ogre;
class CostMapControl : public CostMap
{

public:
    CostMapControl(int w, int h) : CostMap(w, h)
    {
        for (int i = 3; i < 8; i++)
        {
            this->setCost(i, 4, 2);
        }

        // Water: cost 3
        for (int i = 2; i < 6; i++)
        {
            this->setCost(6, i, 3);
        }

        // Low cost path (like road)
        for (int i = 2; i < 10; i++)
        {
            this->setCost(i, 7, 1);
        }

        // Obstacles
        this->setCost(4, 3, CostMap::OBSTACLE);
        this->setCost(7, 5, CostMap::OBSTACLE);
    }
};
