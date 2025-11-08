#pragma once

#include <Ogre.h>
#include <type_traits>
#include <functional>
#include "PathFollow2.h"
#include "util/CellUtil.h"
#include "util/CostMap.h"
using namespace Ogre;

class State
{
public:
    static void forEachState(SceneNode *node, std::function<void(MovableObject *mo, State *)> func, bool recursive = true)
    {
        int numObjs = node->numAttachedObjects();
        for (int i = 0; i < numObjs; i++)
        {
            MovableObject *mo = node->getAttachedObject(i);
            State *s = getState(mo);
            if (s == nullptr)
            {
                continue;
            }
            func(mo, s);
        }

        if(!recursive){
            return ;
        }

        auto cMap = node->getChildren();

        for (auto it = cMap.begin(); it != cMap.end(); ++it)
        {
            Node *node = *it;
            SceneNode *cNode = dynamic_cast<SceneNode *>(node);
            if (cNode)
            {
                forEachState(cNode, func, true);
            }
        }
    }

    static State *getState(MovableObject *mo)
    {
        const Any &any = mo->getUserAny();
        if (any.isEmpty())
        {
            return nullptr;
        }

        State *state = Ogre::any_cast<State *>(any);
        return state;
    }

private:
    State *parent;

public:
    State(State *parent)
    {
        this->parent = parent;
    }

    virtual Ogre::Root *getRoot()
    {
        if (this->parent)
        {
            return this->parent->getRoot();
        }
        return nullptr;
    }
    virtual bool afterPick(MovableObject *mo)
    {
        return false;
    };
    virtual bool setTargetByCell(MovableObject *mo, CellKey cKey2) {
        return false;
    }

public:
};