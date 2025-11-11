
#pragma once
#include <iostream>
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/util/CostMap.h"
#include "fg/InputState.h"
#include <unordered_map>
#include "fg/util/CellUtil.h"
#include "fg/State.h"
#include "fg/IWorld.h"
#include "fg/example/MainInputListener.h"
#include "ActorStateControl.h"
#include "CellStateControl.h"
#include "CellMarkStateControl.h"
#include "fg/State.h"
#include "fg/CostMapControl.h"
#include "fg/Core.h"
#include "fg/core/CameraState.h"
#include "fg/WorldState.h"
#include "fg/core/SimpleInputState.h"
#include "fg/core/MouseClickPicker.h"
using namespace Ogre;
using namespace std;
// root state & control.
class WorldStateControl : public WorldState, public IWorld
{
protected:
    CellStateControl *cells;

    CostMap *costMap;
    CostMapControl *costMapControl;

    std::unordered_map<MarkType, CellMarkStateControl *> markStateControls;

    SimpleInputState *inputState;
    Core *core;

public:
    WorldStateControl(CostMap *costMap, Ground *ground, Core *core) : costMap(costMap), core(core), WorldState(ground)
    {

        Ogre::Root *root = core->getRoot();

        // Create frame listener for main loop
        this->cells = new CellStateControl(costMap, core);

        this->inputState = new SimpleInputState(core->getCamera(), core->getWindow());

        CameraState *cameraState = new CameraState(core->getCamera(), inputState);
        cameraState->setGround(this->ground); //
        root->addFrameListener(cameraState);

        markStateControls[MarkType::ACTIVE] = new CellMarkStateControl(costMap, core, MarkType::ACTIVE);
        ActorStateControl *actor = new ActorStateControl( costMap, core);
        this->addChild(actor);
        root->addFrameListener(actor);
        MainInputListener *keyHandler = new MainInputListener(this, core);
        core->getAppContext()->addInputListener(keyHandler);
        core->getAppContext()->addInputListener(inputState);
        core->getAppContext()->addInputListener(new MouseClickPicker(core->getCamera(), core->getSceneManager(), core->getViewport()));
    }

    CostMap *getCostMap()
    {
        return costMap;
    }
};