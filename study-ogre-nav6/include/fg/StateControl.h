
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
#include "fg/util/StateUtil.h"

using namespace Ogre;
// Base class for model data and control.
//, typename = std::enable_if_t<std::is_base_of_v<State, T>>
template <typename T>
class StateControl : public Component
{
    // static_assert(std::is_base_of_v<State, T>, "T must be derived from State!");

protected:
    T *state;

public:
    // Draw a single hexagon to a specific object

    static void drawHexagonTo(Ogre::ManualObject *obj,
                              const std::vector<Ogre::Vector2> &vertices,
                              const Ogre::ColourValue &color1)
    {
        StateUtil::drawHexagonTo(obj, vertices, color1, color1);
    }

    static void drawHexagonTo(Ogre::ManualObject *obj,
                              const std::vector<Ogre::Vector2> &vertices,
                              const Ogre::ColourValue &color1, ColourValue color2)
    {
        StateUtil::drawHexagonTo(obj, vertices, color1, color2);
    } //
    StateControl()
    {
        this->state = nullptr;
    }

    StateControl(T *state)
    {
        this->state = state;
    }

    void init(InitContext &ctx) override
    {
        Component::init(ctx);
    }
};

class CostMapControl : public StateControl<CostMap>
{

public:
    CostMapControl(CostMap *costMap)
    {
        for (int i = 3; i < 8; i++)
        {
            costMap->setCost(i, 4, 2);
        }

        // Water: cost 3
        for (int i = 2; i < 6; i++)
        {
            costMap->setCost(6, i, 3);
        }

        // Low cost path (like road)
        for (int i = 2; i < 10; i++)
        {
            costMap->setCost(i, 7, 1);
        }

        // Obstacles
        costMap->setCost(4, 3, CostMap::OBSTACLE);
        costMap->setCost(7, 5, CostMap::OBSTACLE);
    }
};
