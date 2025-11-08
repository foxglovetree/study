// main.cpp - Complete Ogre A* Hex Grid Visualization System
#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

// === Include OgreBites for modern initialization ===
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>
#include "util/CostMap.h"
#include "util/HexGridPrinter.h"
#include "core/HexGridApp.h"
// === Custom hash function ===
//



// === Main function ===
int main()
{
    std::cout << "OGRE Version: "
              << OGRE_VERSION_MAJOR << "."
              << OGRE_VERSION_MINOR << "."
              << OGRE_VERSION_PATCH << std::endl;
    try
    {
        std::cout << "Weighted Hexagonal Grid Navigation System\n";
        std::cout << "=========================================\n\n";

        // Initialize Ogre application context
        auto app = std::make_unique<HexGridApp>();
        Component::InitContext ctx;
        app->init(ctx);                
        app->startRendering();
        app->close();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
