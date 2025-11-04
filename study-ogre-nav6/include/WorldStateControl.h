
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "CostMap.h"
#include "CellStateControl.h"
#include "InputState.h"
#include "PathStateControl.h"
#include "CellMarkStateControl.h"
#include <unordered_map>
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

    std::unordered_map<MarkType, CellMarkStateControl *> markStateControls;

    PathStateControl *pathStateControl;

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

        markStateControls[MarkType::START] = new CellMarkStateControl(costMap, sceneMgr, MarkType::START);
        markStateControls[MarkType::END] = new CellMarkStateControl(costMap, sceneMgr, MarkType::END);

        this->pathStateControl = new PathStateControl(costMap, sceneMgr);
    }
    PathStateControl *getPathStateControl()
    {
        return this->pathStateControl;
    }
    InputState *getInputState()
    {
        return this->inputState;
    }
    CostMap *getCostMap()
    {
        return costMap;
    }
    void markCell(int cx, int cy, MarkType mType)
    {
        bool marked = markStateControls[mType]->mark(cx, cy);

        //todo raise a event.
        if(!marked){
            return ;
        }
        if (mType == MarkType::START || mType == MarkType::END)
        {
            pathStateControl->findPath(cx,cy);
        }
    }
};