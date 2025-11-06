
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
        
        markStateControls[MarkType::START] = new CellMarkStateControl(costMap, sceneMgr, MarkType::START);
        markStateControls[MarkType::END] = new CellMarkStateControl(costMap, sceneMgr, MarkType::END);
        
        this->pathStateControl = new PathStateControl(costMap, sceneMgr);
        this->actorStateControl = new ActorStateControl(costMap, sceneMgr);
        root->addFrameListener(actorStateControl) ;
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
        // switch mark
        bool marked = markStateControls[mType]->mark(cx, cy);

        // todo raise a event.
        if (!marked) // unmarked
        {
            return;
        }
        // marked.
        if (mType == MarkType::END)
        {

            State *actor = this->actorStateControl->getState();

            if (actor->isActive())
            {

                Vector3 pos = this->actorStateControl->getNode()->getPosition();
                int cx1, cy1;
                bool hitCell = CellUtil::findCellByPoint(costMap, pos.x, pos.z, cx1, cy1);
                if (hitCell)
                {
                    std::vector<Vector2> pathByKey = costMap->findPath(cx1, cy1, cx, cy);
                    std::vector<Vector2> pathByPosition(pathByKey.size());
                    CellUtil::translatePathToCellCenter(pathByKey, pathByPosition);
                    Vector2 pos2(pos.x, pos.z);
                    PathFollow2 *pathFollow = new PathFollow2(pos2, pathByPosition);
                    actor->setPath(pathFollow);
                    pathStateControl->setPath(pathByKey, cx1, cy1, cx, cy);
                }
            }
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
        int cx;
        int cy;
        bool hitCell = CellUtil::findCellByPoint(costMap, pos.x, pos.z, cx, cy);

        if (hitCell)
        {
            markCell(cx, cy, MarkType::START);
            bool active = !actor->isActive();
            actor->setActive(active);
            if (!active)
            {
                this->pathStateControl->clearPath();
            }
        }
    }
};