// main.cpp - Complete Ogre A* Hex Grid Visualization System

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
#include "HexNavigationGrid.h"
#include "HexMapVisualizer.h"
#include "HexApp.h"
#include "KeyHandler.h"
#include "HexGridPrinter.h"
// === Custom hash function ===
//
struct InputState inputState;


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
        auto appCtx = std::make_unique<OgreBites::ApplicationContext>("HexagonalGridVisualizer");
        appCtx->initApp();

        Ogre::Root *root = appCtx->getRoot();
        Ogre::SceneManager *sceneMgr = root->createSceneManager();

        // Register our scene with the RTSS (Required for proper lighting/shaders)
        Ogre::RTShader::ShaderGenerator *shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(sceneMgr);

        // Ogre::RTShader::RenderState* renderState = shadergen->getRenderState(Ogre::RTShader::RS_DEFAULT);
        // std::string techName = "VertexColourTech";
        // Ogre::Pass *pass=nullptr;

        // Create visualizer
        HexMapVisualizer visualizer(sceneMgr, appCtx->getRenderWindow());

        // Create navigation grid and set up example terrain
        HexNavigationGrid navGrid(12, 10);

        // Sand: cost 2
        for (int i = 3; i < 8; i++)
        {
            navGrid.setCost(i, 4, 2);
        }

        // Water: cost 3
        for (int i = 2; i < 6; i++)
        {
            navGrid.setCost(6, i, 3);
        }

        // Low cost path (like road)
        for (int i = 2; i < 10; i++)
        {
            navGrid.setCost(i, 7, 1);
        }

        // Obstacles
        navGrid.setCost(4, 3, HexNavigationGrid::OBSTACLE);
        navGrid.setCost(7, 5, HexNavigationGrid::OBSTACLE);

        // Find path
        std::cout << "Finding path from (1,1) to (10,8):\n";
        HexGridPrinter::printCostGrid(navGrid);
        auto path = navGrid.findPath(1, 1, 10, 8);
        std::cout << "Path found with " << path.size() << " hexes\n";
        if (!path.empty())
        {
            float totalCost = navGrid.calculatePathCost(path);
            std::cout << "Total path cost: " << totalCost << "\n";
            std::cout << "Path: ";
            for (const auto &p : path)
            {
                std::cout << "(" << (int)p.x << "," << (int)p.y << ") ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        HexGridPrinter::printPathGrid(navGrid, 1, 1, 10, 8, path);

        // Set data to visualizer (don't draw directly!)
        visualizer.setGrid(navGrid);
        visualizer.setPath(path, 1, 1, 10, 8);

        // Create frame listener for main loop
        HexApp frameListener(&visualizer, inputState);
        root->addFrameListener(&frameListener);

        // Add input listener
        KeyHandler keyHandler(inputState);
        appCtx->addInputListener(&keyHandler);

        std::cout << "Starting Ogre visualization... Press ESC to exit.\n";

        // Start rendering loop - this will call frameStarted automatically
        root->startRendering();

        std::cout << "Closing application.\n";
        appCtx->closeApp();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
