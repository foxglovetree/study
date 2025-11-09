
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
#include "fg/core/MainInputListener.h"
#include "ActorStateControl.h"
#include "CellStateControl.h"
#include "CellMarkStateControl.h"
#include "fg/core/WorldState.h"
#include "fg/State.h"
#include "fg/CostMapControl.h"
#include "fg/Core.h"
#include "fg/core/CameraStateControl.h"

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

    InputState *inputState;

    Core *core;

public:
    WorldStateControl(CostMap *costMap, Core *core) : costMap(costMap), core(core), WorldState(nullptr)
    {
        Ogre::Root *root = core->getRoot();

        // Create frame listener for main loop
        this->cells = new CellStateControl(costMap, core);
        this->inputState = new InputState();
        CameraStateControl *frameListener = new CameraStateControl(this, costMap, core->getCamera(), inputState);

        root->addFrameListener(frameListener);
        markStateControls[MarkType::ACTIVE] = new CellMarkStateControl(costMap, core, MarkType::ACTIVE);

        ActorStateControl *actor = new ActorStateControl(this, costMap, core);

        root->addFrameListener(actor);
        MainInputListener *keyHandler = new MainInputListener(this, core, this->inputState);
        core->getAppContext()->addInputListener(keyHandler);
    }

    CostMap *getCostMap()
    {
        return costMap;
    }

    void setTargetByCell(CellKey cKey) override
    {
        SceneManager * sMgr = core->getSceneManager();
        SceneNode *rNode = sMgr->getRootSceneNode();
        State::forEachState(rNode, [cKey](MovableObject *mo, State *s)
                            { s->setTargetByCell(mo, cKey); });
    }

    void pickActorByRay(Ray &ray) override
    {
        // 创建射线查询对象
        SceneManager * sMgr = core->getSceneManager();
        Ogre::RaySceneQuery *rayQuery = sMgr->createRayQuery(ray);
        rayQuery->setSortByDistance(true);  // 按距离排序（最近的优先）
        rayQuery->setQueryMask(0x00000001); // 与 Entity 的查询掩码匹配

        // 执行查询
        Ogre::RaySceneQueryResult &result = rayQuery->execute();

        State *actor = nullptr;
        MovableObject *actorMo = nullptr;
        // 遍历结果
        for (auto &it : result)
        {
            const Any &any = it.movable->getUserAny();
            if (any.isEmpty())
            {
                continue;
            }

            State *state = Ogre::any_cast<State *>(any);
            if (ActorState *actorPtr = dynamic_cast<ActorState *>(state))
            {
                actor = actorPtr;
                actorMo = it.movable;
            }
            break;
        }
        sMgr->destroyQuery(rayQuery);
        if (actor == nullptr)
        {
            return;
        }
        //
        actor->afterPick(actorMo);
        // high light the cell in which the actor stand.
    }
};