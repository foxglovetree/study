
#pragma once
#include <iostream>
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "CostMap.h"
#include "CellStateControl.h"
#include "InputState.h"
#include "PathStateControl.h"
#include "CellMarkStateControl.h"
#include <unordered_map>
#include "ActorStateControl.h"
#include "CellUtil.h"
#include "State.h"

using namespace Ogre;
using namespace std;
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
    ActorStateControl *actorStateControl;

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
        frameListener = new CameraStateControl(costMap, camera, inputState);
        root->addFrameListener(frameListener);

        markStateControls[MarkType::ACTIVE] = new CellMarkStateControl(costMap, sceneMgr, MarkType::ACTIVE);

        this->pathStateControl = new PathStateControl(costMap, sceneMgr);
        this->actorStateControl = new ActorStateControl(costMap, sceneMgr);
        root->addFrameListener(actorStateControl);
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

    void setTargetByCell(CellKey cKey)
    {
        State *actor = this->actorStateControl->getState();

        if (!actor->isActive())
        {
            return;
        }
        Vector3 aPos3 = this->actorStateControl->getNode()->getPosition();
        Vector2 aPos2(aPos3.x, aPos3.z);
        CellKey aCellKey;
        bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
        if (hitCell)
        {
            std::vector<Vector2> pathByKey = costMap->findPath(aCellKey, cKey);
            std::vector<Vector2> pathByPosition(pathByKey.size());
            CellUtil::translatePathToCellCenter(pathByKey, pathByPosition);
            PathFollow2 *pathFollow = new PathFollow2(aPos2, pathByPosition);
            actor->setPath(pathFollow);
            pathStateControl->setPath(pathByKey, aCellKey, cKey);
        }
    }

    void pickActorByRay(Ray &ray)
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
            if (state->isType(State::Type::ACTOR))
            {
                actor = state;
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

        // high light the cell in which the actor stand.

        SceneNode *node = actorMo->getParentSceneNode();
        const Vector3 &pos = node->getPosition();
        cout << "actor.pos:" << pos << "" << endl;
        CellKey cKey;
        bool hitCell = CellUtil::findCellByPoint(costMap, pos, cKey);

        if (hitCell)
        {
            bool active = actor->isActive();
            if (!active)
            {
                actor->setActive(true);
                markStateControls[MarkType::ACTIVE]->mark(cKey, true);
            }
            else
            {
                actor->setActive(false);
                actor->setPath(nullptr);
                CellKey start; 
                if(this->pathStateControl->getStart(start)){
                    markStateControls[MarkType::ACTIVE]->mark(start, false);
                    this->pathStateControl->clearPath();
                }

            }
        }
    }
};