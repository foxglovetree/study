#pragma once
#include "fg/core/SimpleApp.h"
#include "fg/Module.h"
#include "WorldStateControl.h"

class Example : public Module
{

    CostMap *costMap;

public:
    Example()
    {
        costMap = new CostMapControl(12, 10);
    }

    void active(Core *core) override
    {
        new WorldStateControl(costMap, core);
    }
};
