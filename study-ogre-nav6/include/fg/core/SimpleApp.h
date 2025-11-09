#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include "fg/core/MaterialFactory.h"
#include "fg/util/HexGridPrinter.h"
#include "CameraStateControl.h"
#include "fg/CostMapControl.h"
#include "fg/Module.h"
#include "fg/Application.h"
#include "fg/core/SimpleCore.h"

using namespace OgreBites;
using namespace Ogre;
class SimpleApp : public Application
{
private:
    Core *core;
    std::vector<Module *> list;

public:
    SimpleApp()
    {
        this->core = new SimpleCore();
    }

    void add(Module *mod) override
    {
        list.push_back(mod);
    }

    void setup() override
    {
        for (auto it = list.begin(); it != list.end(); it++)
        {
            Module *mod = *it;
            mod->active(this->core);
        }
    }

    void startRendering() override
    {

        Ogre::Root *root = this->core->getRoot();
        root->startRendering();
    }

    void close() override
    {
        std::cout << "Closing application.\n";
        core->getAppContext()->closeApp();
    }
};