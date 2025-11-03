#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include "CellRender.h"
#include "HexGridPrinter.h"
#include "InputState.h"
#include "CameraUpdater.h"
#include "MainInputListener.h"
#include "CellManager.h"
using namespace OgreBites;
using namespace Ogre;
class HexGridApp : public ApplicationContext
{
private:
    InputState inputState;
    CellManager *cells;

    CellRender *render;
    CameraUpdater *frameListener;
    MainInputListener *keyHandler;

public:
    HexGridApp() : ApplicationContext("HexagonalGridVisualizer")
    {
        this->cells = new CellManager(12, 10);
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

        render = new CellRender(sceneMgr, getRenderWindow(), cells);

        // Create frame listener for main loop
        frameListener = new CameraUpdater(render, inputState);
        root->addFrameListener(frameListener);

        // Add input listener
        keyHandler = new MainInputListener(inputState, window, cells, render->getViewport(), render->getCamera(), render);
        addInputListener(keyHandler);

        std::cout << "Starting Ogre visualization... Press ESC to exit.\n";
    }
    void findPath()
    {
        // Find path
        std::cout << "Finding path from (1,1) to (10,8):\n";
        HexGridPrinter::printCostGrid(cells->getCostMap());
        CostMap &costMap = cells->getCostMap();
        auto path = costMap.findPath(1, 1, 10, 8);
        std::cout << "Path found with " << path.size() << " hexes\n";
        if (!path.empty())
        {
            float totalCost = costMap.calculatePathCost(path);
            std::cout << "Total path cost: " << totalCost << "\n";
            std::cout << "Path: ";
            for (const auto &p : path)
            {
                std::cout << "(" << (int)p.x << "," << (int)p.y << ") ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        HexGridPrinter::printPathGrid(costMap, 1, 1, 10, 8, path);
        render->setPath(path, 1, 1, 10, 8);
    }

    void startRendering()
    {
        mRoot->startRendering();
    }
};