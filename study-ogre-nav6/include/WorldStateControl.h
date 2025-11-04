
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "CostMap.h"
#include "CellStateControl.h"
#include "InputState.h"
using namespace Ogre;

// root state & control.
class WorldStateControl : StateControl
{
protected:
    InputState *inputState;
    CellStateControl *cells;
    Ogre::SceneManager *sceneMgr;

    CostMap *costMap;
    CostMapControl *costMapControl;
    Ogre::Root *root;
    CameraStateControl *frameListener;
    CellFocusStateControl *cellFocusStateControl;

public:
    WorldStateControl(Ogre::Root *root, CostMap *costMap, Ogre::SceneManager *sceneMgr, Camera *camera) : costMap(costMap)
    {
        this->root = root;
        this->inputState = new InputState();
        this->sceneMgr = sceneMgr;

        this->costMapControl = new CostMapControl(costMap);
        int width = costMap->getWidth();
        int height = costMap->getHeight();

        cells = new CellStateControl(costMap, sceneMgr);

        // Create frame listener for main loop
        frameListener = new CameraStateControl(camera, inputState);
        root->addFrameListener(frameListener);
        cellFocusStateControl = new CellFocusStateControl(costMap, sceneMgr);
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
        cellFocusStateControl->select(cx, cy);
    }
};