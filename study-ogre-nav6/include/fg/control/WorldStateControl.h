
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
using namespace Ogre;
using namespace std;
// root state & control.
class WorldStateControl : public WorldState, public IWorld
{
protected:
    CellStateControl *cells;
    Ogre::SceneManager *sceneMgr;

    CostMap *costMap;
    CostMapControl *costMapControl;
    Ogre::Root *root;

    std::unordered_map<MarkType, CellMarkStateControl *> markStateControls;

    Camera *camera;
    ApplicationContext *app;
    RenderWindow *window;
    Viewport *vp;
    InputState *inputState;

public:
    WorldStateControl(CostMap *costMap, ApplicationContext *app, SceneManager *sMgr, Viewport *vp, Camera *camera) : costMap(costMap), WorldState(nullptr)
    {

        this->app = app;
        this->sceneMgr = sMgr;
        vp = vp;
        camera = camera;

        root = this->app->getRoot();
        window = this->app->getRenderWindow();

        // Create frame listener for main loop
        this->cells = new CellStateControl(costMap, sMgr);
        this->inputState = new InputState();
        CameraStateControl *frameListener = new CameraStateControl(this, costMap, camera, inputState);

        root->addFrameListener(frameListener);
        markStateControls[MarkType::ACTIVE] = new CellMarkStateControl(costMap, sceneMgr, MarkType::ACTIVE);

        ActorStateControl *actor = new ActorStateControl(this, costMap, sceneMgr);

        root->addFrameListener(actor);
        MainInputListener *keyHandler = new MainInputListener(this, window, vp, camera, this->inputState);
        app->addInputListener(keyHandler);
    }

    CostMap *getCostMap()
    {
        return costMap;
    }

    void setTargetByCell(CellKey cKey) override
    {
        SceneNode *root = this->sceneMgr->getRootSceneNode();
        State::forEachState(root, [cKey](MovableObject *mo, State *s)
                            { s->setTargetByCell(mo, cKey); });
    }

    void pickActorByRay(Ray &ray) override
    {
        // 创建射线查询对象
        Ogre::RaySceneQuery *rayQuery = sceneMgr->createRayQuery(ray);
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
        sceneMgr->destroyQuery(rayQuery);
        if (actor == nullptr)
        {
            return;
        }
        //
        actor->afterPick(actorMo);
        // high light the cell in which the actor stand.
    }
};