#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include "fg/example/MaterialFactory.h"
#include "fg/util/HexGridPrinter.h"
#include "fg/CostMapControl.h"
#include "fg/Module.h"
#include "fg/App.h"
#include "fg/core/SimpleCore.h"

using namespace OgreBites;
using namespace Ogre;
class SimpleApp : public App
{
private:
    Core *core;
    std::vector<Module *> list;
    std::unordered_map<std::string, Module *> map;

public:
    SimpleApp()
    {
        this->core = new SimpleCore();
    }

    void add(Module *mod) override
    {

        std::string name = mod->getName();
        if (map.find(name) != map.end())
        {
            throw std::runtime_error("Module already exists:" + name);
        }
        map[name] = mod;
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