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
#include "fg/InputState.h"

using namespace OgreBites;
using namespace Ogre;
using namespace std;
// === Custom hash function ===
//
// === Input handler for closing application ===
class SimpleInputState : public OgreBites::InputListener, public InputState
{
private:
    bool left = false;
    bool right = false;
    bool front = false;
    bool back = false;

    Camera *camera;
    RenderWindow *window;

public:
    SimpleInputState(Camera *cam, RenderWindow *window)
    {
        this->camera = cam;
        this->window = window;
    }

    bool mouseMoved(const MouseMotionEvent &evt) override
    {

        int width = window->getWidth();
        int height = window->getHeight();
        // 定义边缘区域（例如：10 像素）
        int edgeSize = 10;
        int x = evt.x;
        int y = evt.y;
        this->left = (x <= edgeSize);
        this->right = (x >= width - edgeSize);
        this->front = (y <= edgeSize);
        this->back = (y >= height - edgeSize);
        if (this->isMoving())
        {
            cout << "(" << x << "," << y << "),(" << width << "," << height << ")" << endl;
            // try pick.
        }
        else
        {
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
            this->left = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
        {
            this->right = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_UP)
        {
            this->front = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_DOWN)
        {
            this->back = true;
        }
        return true;
    }
    bool keyReleased(const OgreBites::KeyboardEvent &evt) override
    {

        if (evt.keysym.sym == OgreBites::SDLK_LEFT)
        {
            this->left = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
        {
            this->right = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_UP)
        {
            this->front = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_DOWN)
        {
            this->back = false;
        }
        return true;
    }

    bool isLeft() override
    {
        return this->left;
    }
    bool isRight() override
    {
        return this->right;
    }
    bool isFront() override
    {
        return this->front;
    }
    bool isBack() override
    {
        return this->back;
    }
};
