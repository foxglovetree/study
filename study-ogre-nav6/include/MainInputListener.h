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
#include "CellManager.h"

using namespace OgreBites;
using namespace Ogre;
using namespace std;
// === Custom hash function ===
//
// === Input handler for closing application ===
class MainInputListener : public OgreBites::InputListener
{
private:
    InputState &inputState;
    RenderWindow *window;
    CellManager &cells;
    Viewport *viewport;
    Camera *camera;
    CellRender *cellRender;

public:
    MainInputListener(InputState &inputState, RenderWindow *window, CellManager &cells, Viewport *viewport, Camera *camera, CellRender *cellRender) : inputState(inputState), window(window), cells(cells), viewport(viewport), camera(camera), cellRender(cellRender) {};

    bool mousePressed(const MouseButtonEvent &evt) override
    {
        if (evt.button == ButtonType::BUTTON_LEFT)
        {
            pickByMouse(evt.x, evt.y);
        }
        return true;
    }

    void pickByMouse(int mx, int my)
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
            bool hitCell = cellRender->findCellByPoint(pickX, pickZ, cx, cy);

            cout << "worldPoint(" << pickX << ",0," << pickZ << "),cellIdx:[" << cx << "," << cy << "]" << endl;
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
        inputState.left = (x <= edgeSize);
        inputState.right = (x >= width - edgeSize);
        inputState.up = (y <= edgeSize);
        inputState.down = (y >= height - edgeSize);
        if (inputState.isMoving())
        {
            cout << "(" << x << "," << y << "),(" << width << "," << height << ")" << endl;
            // try pick.
        }
        else
        {
            pickByMouse(x, y);
        }
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
            inputState.left = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
        {
            inputState.right = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_UP)
        {
            inputState.up = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_DOWN)
        {
            inputState.down = true;
        }
        return true;
    }
    bool keyReleased(const OgreBites::KeyboardEvent &evt) override
    {

        if (evt.keysym.sym == OgreBites::SDLK_LEFT)
        {
            inputState.left = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
        {
            inputState.right = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_UP)
        {
            inputState.up = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_DOWN)
        {
            inputState.down = false;
        }
        return true;
    }
};
