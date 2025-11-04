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
#include "WorldStateControl.h"
#include "CellMark.h"

using namespace OgreBites;
using namespace Ogre;
using namespace std;
// === Custom hash function ===
//
// === Input handler for closing application ===
class MainInputListener : public OgreBites::InputListener
{
private:
    RenderWindow *window;
    Viewport *viewport;
    Camera *camera;
    WorldStateControl *wsc;
    InputState *inputState;

public:
    MainInputListener(WorldStateControl *wsc,
                      RenderWindow *window, Viewport *viewport,
                      Camera *camera) : window(window),
                                        viewport(viewport), camera(camera), wsc(wsc)
    {

        this->inputState = wsc->getInputState();
    };

    bool mousePressed(const MouseButtonEvent &evt) override
    {
        if (evt.button == ButtonType::BUTTON_LEFT)
        {
            markByMouse(MarkType::START, evt.x, evt.y);
        }
        if (evt.button == ButtonType::BUTTON_RIGHT)
        {
            markByMouse(MarkType::END, evt.x, evt.y);
        }
        
        return true;
    }

    void markByMouse(MarkType mType, int mx, int my)
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
            Ogre::Vector3 worldPt = ray.getPoint(hitGrd.second);
            float pickX = worldPt.x;
            float pickZ = worldPt.z; // 因为我们把“y”方向当成了水平面内的 y
            int cx = -1;
            int cy = -1;
            bool hitCell = findCellByPoint(pickX, pickZ, cx, cy);
            if (hitCell)
            {
                wsc->markCell(cx, cy, mType);
            }
            cout << "worldPoint(" << pickX << ",0," << pickZ << "),cellIdx:[" << cx << "," << cy << "]" << endl;
        }
    }

    bool findCellByPoint(float px, float py, int &cx, int &cy)
    {
        CostMap *costMap = this->wsc->getCostMap();
        int width = costMap->getWidth();
        int height = costMap->getHeight();
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (isPointInCell(px, py, x, y))
                {
                    cx = x;
                    cy = y;
                    return true;
                }
            }
        }
        return false;
    }
    bool isPointInCell(float px, float py, int cx, int cy)
    {
        auto corners = CostMap::calculateVerticesForXZ(cx, cy, CostMap::hexSize);

        // 叉积判断是否在所有边的左侧
        for (int i = 0; i < 6; ++i)
        {
            // 逆时针
            auto p1 = corners[i];
            auto p2 = corners[(i + 1) % 6];

            // 向量 edge = p2 - p1
            // 向量 point = (mx, my) - p1
            float cross = (px - p1.x) * (p2.y - p1.y) - (py - p1.y) * (p2.x - p1.x);
            if (cross < 0)
                return false;
        }
        return true;
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
