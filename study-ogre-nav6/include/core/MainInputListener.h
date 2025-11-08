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
#include "InputState.h"
#include <OgreRenderWindow.h>
#include <iostream>
#include "util/CellMark.h"
#include "util/CellUtil.h"
#include "IWorld.h"

using namespace OgreBites;
using namespace Ogre;
using namespace std;
// === Custom hash function ===
//
// === Input handler for closing application ===
class MainInputListener : public OgreBites::InputListener, public Component
{
private:
    RenderWindow *window;
    Viewport *viewport;
    Camera *camera;
    InputState *inputState;
    IWorld * wsc;
public:
    MainInputListener(IWorld *wsc,
                      RenderWindow *window, Viewport *viewport,
                      Camera *camera) : window(window),
                                        viewport(viewport), camera(camera)
    {
        this->wsc = wsc;
    };
    void init(Component::InitContext &ctx) override
    {
        this->inputState = parent->find<InputState>();
    }

    void pickByMouse(int mx, int my)
    {
        // normalized (0,1)
        float ndcX = mx / (float)viewport->getActualWidth();
        float ndcY = my / (float)viewport->getActualHeight();
        Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

        wsc->pickActorByRay(ray);
    }

    bool mousePressed(const MouseButtonEvent &evt) override
    {
        if (evt.button == ButtonType::BUTTON_LEFT)
        {
            pickByMouse(evt.x, evt.y);
            // markByMouse(MarkType::START, evt.x, evt.y);
        }
        if (evt.button == ButtonType::BUTTON_RIGHT)
        {

            setTargetByMouse(evt.x, evt.y);
        }

        return true;
    }

    void setTargetByMouse(int mx, int my)
    {
        // normalized (0,1)
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
            bool hitCell = CellUtil::findCellByPoint(costMap, Vector2(pos.x, pos.z), cKey);
            if (hitCell)
            {
                //
                wsc->setTargetByCell(cKey);
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

    bool mouseMoved(const MouseMotionEvent &evt) override
    {
        int width = window->getWidth();
        int height = window->getHeight();
        // 定义边缘区域（例如：10 像素）
        int edgeSize = 10;
        int x = evt.x;
        int y = evt.y;
        inputState->left = (x <= edgeSize);
        inputState->right = (x >= width - edgeSize);
        inputState->front = (y <= edgeSize);
        inputState->back = (y >= height - edgeSize);
        if (inputState->isMoving())
        {
            cout << "(" << x << "," << y << "),(" << width << "," << height << ")" << endl;
            // try pick.
        }
        else
        {
        }
        return true;
    }
    bool mouseWheelRolled(const MouseWheelEvent &evt) override
    {
        Ogre::SceneNode *node = camera->getParentSceneNode();
        float speed = 20.0f;
        node->translate(Ogre::Vector3::UNIT_Y * evt.y * speed);

        return true;
    }
    bool keyPressed(const OgreBites::KeyboardEvent &evt) override
    {
        if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
        {
            Ogre::Root::getSingleton().queueEndRendering();
        }
        if (evt.keysym.sym == OgreBites::SDLK_LEFT)
        {
            inputState->left = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
        {
            inputState->right = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_UP)
        {
            inputState->front = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_DOWN)
        {
            inputState->back = true;
        }
        return true;
    }
    bool keyReleased(const OgreBites::KeyboardEvent &evt) override
    {

        if (evt.keysym.sym == OgreBites::SDLK_LEFT)
        {
            inputState->left = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
        {
            inputState->right = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_UP)
        {
            inputState->front = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_DOWN)
        {
            inputState->back = false;
        }
        return true;
    }
};
