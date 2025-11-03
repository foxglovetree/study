#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include "HexMapVisualizer.h"
#include "HexGridPrinter.h"
#include "InputState.h"
#include "CameraUpdater.h"
#include "InputStateModifier.h"
using namespace OgreBites;
using namespace Ogre;
class HexGridApp : public ApplicationContext
{
private:
    InputState inputState;
    GridManager navGrid{12, 10};
    HexMapVisualizer *visualizer;
    CameraUpdater *frameListener;
    InputStateModifier *keyHandler;

public:
    HexGridApp() : ApplicationContext("HexagonalGridVisualizer")
    {
    }
    void initApp()
    {
        ApplicationContextBase::initApp();
        // log level
        LogManager *lm = LogManager::getSingletonPtr();
        Log *log = lm->getDefaultLog();
        log->setDebugOutputEnabled(false);
        log->setLogDetail(Ogre::LL_LOW);
        //
        InputListener *ls = getImGuiInputListener();

        RenderWindow *window = this->getRenderWindow();

        Ogre::Root *root = getRoot();
        Ogre::SceneManager *sceneMgr = root->createSceneManager();

        // Register our scene with the RTSS (Required for proper lighting/shaders)
        Ogre::RTShader::ShaderGenerator *shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(sceneMgr);

        // Ogre::RTShader::RenderState* renderState = shadergen->getRenderState(Ogre::RTShader::RS_DEFAULT);
        // std::string techName = "VertexColourTech";
        // Ogre::Pass *pass=nullptr;

        // Create visualizer

        // Create navigation grid and set up example terrain

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
        navGrid.setCost(4, 3, GridManager::OBSTACLE);
        navGrid.setCost(7, 5, GridManager::OBSTACLE);

        visualizer = new HexMapVisualizer(sceneMgr, getRenderWindow());

        // Set data to visualizer (don't draw directly!)
        visualizer->setGrid(navGrid);

        // Create frame listener for main loop
        frameListener = new CameraUpdater(visualizer, inputState);
        root->addFrameListener(frameListener);

        // Add input listener
        keyHandler = new InputStateModifier(inputState, window);
        addInputListener(keyHandler);

        std::cout << "Starting Ogre visualization... Press ESC to exit.\n";
    }
    void findPath()
    {
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
        visualizer->setPath(path, 1, 1, 10, 8);
    }

    void startRendering()
    {
        mRoot->startRendering();
    }
};