#pragma once

#include <Ogre.h>
#include <OgreNode.h>
#include <type_traits>
#include <functional>
#include "PathFollow2.h"
#include "Pickable.h"
#include "OgreFrameListener.h"
#include "util/CellUtil.h"
#include "util/CostMap.h"
using namespace Ogre;

class State
{
public:
    static State *get(Node *node)
    {
        const Any &any = node->getUserAny();
        if (any.isEmpty())
        {
            return nullptr;
        }

        State *state = Ogre::any_cast<State *>(any);
        return state;
    }
    static void set(Node *node, State *state)
    {
        node->setUserAny(state);
    }

    static void forAllState(State *state, std::function<void(State *)> func)
    {
        func(state);
        forEachChildState(state, func, true);
    }

    static void forEachChildState(State *s, std::function<void(State *)> func, bool recursive = true)
    {
        Node *node = s->node;
        auto cMap = node->getChildren();

        for (auto it = cMap.begin(); it != cMap.end(); ++it)
        {
            Node *cNode = *it;
            State *cState = State::get(cNode);
            if (!cState)
            {
                // TODO throw exception.
                continue;
            }
            forAllState(cState, func);
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

protected:
    State *parent;
    FrameListener *frameListener = nullptr;
    Pickable *pickable = nullptr;
    Node *node = nullptr;
    SceneNode *sceNode = nullptr;

public:
    State(State *parent)
    {
        this->parent = parent;
    }

    State()
    {
        this->parent = nullptr;
    }

    SceneNode *getSceneNode()
    {
        return this->sceNode;
    }

    SceneNode *findSceneNode()
    {
        State *s = this;
        while (s)
        {
            if (s->sceNode)
            {
                return s->sceNode;
            }
            s = s->parent;
        }
        return nullptr;
    }
    void setSceneNode(SceneNode *sNode)
    {
        this->setNode(sNode);
        this->sceNode = sNode;
    }

    Node *getNode()
    {
        return this->node;
    }

    void setNode(Node *node2)
    {

        if (this->node != nullptr)
        {
            // move this node from the old node to the new one.
            // erase the state of the new node if any.
            this->node->setUserAny(Any(nullptr));
        }
        State::set(node2, this);
        this->node = node2;
    }

    void addChild(State *s)
    {
        Node *cNode = this->node->createChild();
        s->setNode(cNode);
    }

    void removeAllChildren()
    {
        std::vector<Node *> cMap = this->node->getChildren();
        for (auto it : cMap)
        {
            State *cS = State::get(it);
            delete cS;
        }
        this->node->removeAllChildren();
    }

    Pickable *getPickable()
    {
        return this->pickable;
    }

    void setPickable(Pickable *pick)
    {
        this->pickable = pick;
    }

    FrameListener *getFrameListener()
    {
        return this->frameListener;
    }

    void setFrameListener(FrameListener *listener)
    {
        this->frameListener = listener;
    }

    virtual bool setTargetByCell(CellKey cKey2)
    {
        return false;
    }

    void forEachChild(std::function<void(State *)> &func, bool recusive = true)
    {

        auto cMap = node->getChildren();

        for (auto it = cMap.begin(); it != cMap.end(); ++it)
        {
            Node *cNode = *it;
            State *s = State::get(cNode);
            func(s);
            if (recusive)
            {
                s->forEachChild(func, recusive);
            }
        }
    }

public:
};