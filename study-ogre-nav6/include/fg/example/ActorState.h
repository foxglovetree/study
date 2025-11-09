#pragma once

#include <Ogre.h>
#include "fg/PathFollow2.h"
#include "fg/util/CellUtil.h"
#include "fg/util/CostMap.h"
#include "fg/State.h"
#include "PathState.h"
#include "fg/Pickable.h"

using namespace Ogre;
class ActorState : public State, public Pickable, public Ogre::FrameListener
{
protected:
    bool active = false;

    PathFollow2 *pathFolow = nullptr;
    Ogre::Entity *entity;
    CostMap *costMap;
    PathState *pathState;

    SceneNode *node;
    AnimationState *aniState;

public:
    ActorState(State *parent, CostMap *costMap, Core *core) : State(parent)
    {
        this->costMap = costMap;
        pathState = new PathState(this, costMap, core);
    }

    void setEntity(Ogre::Entity *entity)
    {
        this->entity = entity;
    }
    Ogre::Entity *getEntity()
    {
        return this->entity;
    }

    SceneNode *getNode()
    {
        return this->node;
    }

    void setNode(SceneNode *node)
    {
        this->node = node;
    }

    void setActive(bool active)
    {
        this->active = active;
    }

    bool isActive()
    {
        return this->active;
    }
    PathFollow2 *getPath()
    {
        return this->pathFolow;
    }
    void setPath(PathFollow2 *path)
    {
        this->pathFolow = path;
    }
    bool afterPick(MovableObject *actorMo) override
    {
        // cout << "ActorState::afterPick" << endl;

        SceneNode *node = actorMo->getParentSceneNode();
        const Vector3 &pos = node->getPosition();
        cout << "actor.pos:" << pos << "" << endl;
        CellKey cKey;
        bool hitCell = CellUtil::findCellByPoint(costMap, pos, cKey);
        ActorState *actor = this;
        if (hitCell)
        {
            bool active = this->isActive();
            if (!active)
            {
                actor->setActive(true);
                // markStateControls[MarkType::ACTIVE]->mark(cKey, true);
            }
            else
            {
                actor->setActive(false);
                actor->setPath(nullptr);
                CellKey start;
                if (this->pathState->getStart(start))
                {
                    //    markStateControls[MarkType::ACTIVE]->mark(start, false);
                    this->pathState->clearPath();
                }
            }
        }
        return true;
    }

    bool setTargetByCell(MovableObject *mo, CellKey cKey2) override
    {
        if (!this->isActive())
        {
            return false;
        }
        Vector3 aPos3 = mo->getParentNode()->getPosition();
        Vector2 aPos2(aPos3.x, aPos3.z);
        CellKey aCellKey;
        bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
        if (hitCell)
        {
            std::vector<Vector2> pathByKey = costMap->findPath(aCellKey, cKey2);
            std::vector<Vector2> pathByPosition(pathByKey.size());
            CellUtil::translatePathToCellCenter(pathByKey, pathByPosition);
            PathFollow2 *pathFollow = new PathFollow2(aPos2, pathByPosition);
            this->setPath(pathFollow);
            pathState->setPath(pathByKey, aCellKey, cKey2);
            enableAnimation("RunBase");
        }
        return true;
    }

    bool enableAnimation(std::string name)
    {
        aniState = entity->getAnimationState(name);
        if (aniState)
        {

            aniState->setEnabled(true);
            aniState->setLoop(true);   // 循环播放
            aniState->setWeight(1.0f); // 混合权重（用于多动画混合）
            return true;
        }
        return false;
    }

    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        if (this->isActive())
        {
            PathFollow2 *pathFollow = this->getPath();
            if (pathFollow != nullptr)
            {
                Vector2 pos;
                if (pathFollow->move(evt.timeSinceLastFrame, pos))
                {
                    Vector3 pos0 = node->getPosition();
                    node->translate(pos.x - pos0.x, 0, pos.y - pos0.z); // new position
                    // animation
                    if (aniState)
                    {
                        aniState->addTime(evt.timeSinceLastFrame);
                    }
                }
                else
                {
                    setPath(nullptr);
                }
            }
        }
        else
        {
        }
        return true;
    }
};