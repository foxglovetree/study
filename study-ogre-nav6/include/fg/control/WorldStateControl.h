
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
#include "PathStateControl.h"
#include "CellMarkStateControl.h"
#include "fg/core/WorldState.h"
#include "fg/core/RootState.h"
using namespace Ogre;
using namespace std;
// root state & control.
class WorldStateControl : public StateControl<WorldState>, public IWorld
{
protected:
    CellStateControl *cells;
    Ogre::SceneManager *sceneMgr;

    CostMap *costMap;
    CostMapControl *costMapControl;
    Ogre::Root *root;
    CameraStateControl *frameListener;

    std::unordered_map<MarkType, CellMarkStateControl *> markStateControls;

    PathStateControl *pathStateControl;
    ActorStateControl *actorStateControl;
    Camera *camera;
    ApplicationContext *app;
    RenderWindow *window;
    Viewport *vp;

public:
    WorldStateControl(WorldState *state) : StateControl(state)
    {
    }

    void init(InitContext &ctx) override
    {
        this->app = this->find<ApplicationContext>();
        this->sceneMgr = this->find<Ogre::SceneManager>();
        vp = this->find<Viewport>();
        camera = this->find<Camera>();
        costMap = this->find<CostMap>();
        root = this->app->getRoot();
        window = this->app->getRenderWindow();

        this->addObject<Ogre::SceneManager>(this->sceneMgr);
        this->addObject<Camera>(this->camera);
        int width = costMap->getWidth();
        int height = costMap->getHeight();
        this->costMapControl = new CostMapControl(costMap);

        // Create frame listener for main loop
        this->addComponent<CellStateControl>(new CellStateControl());
        this->addObject<InputState>(new InputState());
        frameListener = this->addComponent<CameraStateControl>(new CameraStateControl());

        root->addFrameListener(frameListener);
        markStateControls[MarkType::ACTIVE] = new CellMarkStateControl(costMap, sceneMgr, MarkType::ACTIVE);

        pathStateControl = this->addComponent<PathStateControl>(new PathStateControl());
        ActorState * actor = new ActorState(this->state, costMap);
        this->actorStateControl = this->addComponent<ActorStateControl>(new ActorStateControl(actor));

        root->addFrameListener(actorStateControl);
        MainInputListener *keyHandler = new MainInputListener(this, window, vp, camera);
        app->addInputListener(keyHandler);
        this->addComponent<MainInputListener>(keyHandler);
        StateControl::init(ctx);
    }

    CostMap *getCostMap()
    {
        return costMap;
    }

    void setTargetByCell(CellKey cKey) override
    {
        SceneNode *root = this->sceneMgr->getRootSceneNode();
        State::visitState(root, [cKey](MovableObject * mo,State* s){
            s->setTargetByCell(mo, cKey);
        });
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