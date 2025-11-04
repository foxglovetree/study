
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "CostMap.h"
#include "StateControl.h"
#include "InputState.h"
using namespace Ogre;

// root state & control.
class WorldStateControl : StateControl
{
protected:
    InputState *inputState;
    std::vector<std::vector<CellStateControl *>> cells;
    Ogre::SceneManager *sceneMgr;

    CostMap *costMap;
    CostMapControl *costMapControl;
    Ogre::Root *root;
    CameraStateControl *frameListener;

public:
    WorldStateControl(Ogre::Root *root, CostMap *costMap, Ogre::SceneManager *sceneMgr, Camera *camera) : costMap(costMap)
    {
        this->root = root;
        this->inputState = new InputState();
        this->sceneMgr = sceneMgr;

        this->costMapControl = new CostMapControl(costMap);
        int width = costMap->getWidth();
        int height = costMap->getHeight();
        cells.resize(costMap->getHeight(), std::vector<CellStateControl *>(costMap->getWidth(), nullptr));
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                cells[j][i] = new CellStateControl(costMap, i, j, sceneMgr);
            }
        }

        // Create frame listener for main loop
        frameListener = new CameraStateControl(camera, inputState);
        root->addFrameListener(frameListener);
    }
    InputState *getInputState()
    {
        return this->inputState;
    }
    CostMap *getCostMap()
    {
        return costMap;
    }
    void pickupCell(int cx, int cy)
    {
        cells[cy][cx]->setSelected(true);
    }

    bool isSelected(int cx, int cy)
    {
        return cells[cy][cx]->getSelected();
    }
};