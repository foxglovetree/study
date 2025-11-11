// main.cpp - Complete Ogre A* Hex Grid Visualization System
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

// === Include OgreBites for modern initialization ===
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>
#include <OgreRenderWindow.h>
#include <iostream>
#include "fg/util/CellMark.h"
#include "fg/util/CellUtil.h"
#include "fg/IWorld.h"

using namespace OgreBites;
using namespace Ogre;
using namespace std;
// === Custom hash function ===
//
// === Input handler for closing application ===
class MainInputListener : public OgreBites::InputListener
{
private:
    IWorld *wsc;

    Core *core;

public:
    MainInputListener(IWorld *wsc,
                      Core *core)
    {
        this->core = core;
        this->wsc = wsc;
    }

    bool mousePressed(const MouseButtonEvent &evt) override
    {

        if (evt.button == ButtonType::BUTTON_RIGHT)
        {

            setTargetByMouse(evt.x, evt.y);
        }

        return true;
    }

    void setTargetByMouse(int mx, int my)
    {
        // normalized (0,1)
        Viewport *viewport = core->getViewport();
        Camera *camera = core->getCamera();

        float ndcX = mx / (float)viewport->getActualWidth();
        float ndcY = my / (float)viewport->getActualHeight();

        Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

        Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0
        auto hitGrd = ray.intersects(ground);
        cout << "ndc:(" << ndcX << "," << ndcY << ")" << "hit:" << hitGrd.first << endl;
        if (hitGrd.first)
        {
            Ogre::Vector3 pos = ray.getPoint(hitGrd.second);

            CellKey cKey;
            CostMap *costMap = this->wsc->getCostMap();

            // bool hitCell = CellUtil::findCellByPoint(costMap, Vector2(pos.x, pos.z), cKey);
            bool hitCell = CellUtil::findCellByPoint(costMap, Ground::Transfer::to2D(pos), cKey);

            if (hitCell)
            {

                core->getRootState()->forEachChild(
                    [&cKey](State *s)
                    {
                        Movable *mvb = s->getMovable();
                        if (mvb)
                        {
                            mvb->setTargetCell(cKey);
                        }
                    });
                //
            }
            // cout << "worldPoint(" << pickX << ",0," << pickZ << "),cellIdx:[" << cx << "," << cy << "]" << endl;
        }
    }

    bool mouseReleased(const MouseButtonEvent &evt) override
    {
        if (evt.button == ButtonType::BUTTON_LEFT)
        {
        }
        return true;
    }

    bool mouseWheelRolled(const MouseWheelEvent &evt) override
    {
        Ogre::SceneNode *node = core->getCamera()->getParentSceneNode();
        float speed = 20.0f;
        node->translate(Ogre::Vector3::UNIT_Y * evt.y * speed);

        return true;
    }
};
