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
#include "Movable.h"
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
    static void set(SceneNode *node, State *state)
    {
        node->setUserAny(state);
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
    State *parent = nullptr;
    FrameListener *frameListener = nullptr;
    Pickable *pickable = nullptr;
    Movable *movable = nullptr;
    SceneNode *sceNode = nullptr;
    std::vector<State *> *children;

public:
    State()
    {
        this->children = new std::vector<State *>();
        std::cout << "new State()" << this << "" << std::endl;
    }
    virtual ~State()
    {
        std::cout << "~State()" << this << "" << std::endl;
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
        State::set(sNode, this);
        this->sceNode = sNode;
    }

    void addChild(State *s)
    {
        if (s->parent)
        {
            throw "Already has a parent state.";
        }
        this->children->push_back(s);
        s->parent = this;
    }

    void removeChild(State *cs)
    {
        this->children->erase(std::remove(this->children->begin(), this->children->end(), cs), this->children->end());
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

    Movable *getMovable()
    {
        return this->movable;
    }

    void setMovable(Movable *mvb)
    {
        this->movable = mvb;
    }

    void forEachChild(std::function<void(State *)> func, bool recusive = true)
    {
        std::vector<State *> *tmp = this->children;
        for (auto it = tmp->begin(); it != tmp->end(); ++it)
        {
            State *s = *it;
            func(s);
            if (recusive)
            {
                s->forEachChild(func, recusive);
            }
        }
    }

public:
};