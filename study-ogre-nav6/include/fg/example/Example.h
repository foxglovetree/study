#pragma once
#include "fg/core/SimpleApp.h"
#include "fg/Module.h"
#include "WorldStateControl.h"
class Example
{
public:
    class CostMapMod : public Module
    {

    public:
        CostMapMod()
        {
        }
        std::string getName() override
        {
            return "example.costMapMod";
        }

        void active(Core *core) override
        {
            CostMap *costMap = new CostMapControl(12, 10);
            core->setUserObject<CostMap>("costMap", costMap);
        }
    };

    class WorldStateMod : public Module
    {

    public:
        WorldStateMod()
        {
        }
        std::string getName() override
        {
            return "example.worldStateMod";
        }

        void active(Core *core) override
        {
            
            // Create materials before buding mesh?
            MaterialFactory::createMaterials(core->getMaterialManager());

            CostMap *costMap = core->getUserObject<CostMap>("costMap", true);
            new WorldStateControl(costMap, core);
        }
    };
};
