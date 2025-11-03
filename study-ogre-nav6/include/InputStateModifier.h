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
using namespace OgreBites;
using namespace Ogre;
using namespace std;
// === Custom hash function ===
//
// === Input handler for closing application ===
class InputStateModifier : public OgreBites::InputListener
{
private:
    InputState &inputState;
    RenderWindow *window;

public:
    InputStateModifier(InputState &inputState, RenderWindow *window) : inputState(inputState), window(window) {};

    bool mouseMoved(const MouseMotionEvent &evt) override
    {
        int width = window->getWidth();
        int height = window->getHeight();
        // 定义边缘区域（例如：10 像素）
        int edgeSize = 10;
        int x = evt.x;
        int y =evt.y;
        inputState.left = (x <= edgeSize);
        inputState.right = (x >= width - edgeSize);
        inputState.up = (y <= edgeSize);
        inputState.down = (y >= height - edgeSize);
        cout << "(" << x << "," << y << "),(" << width << "," << height << ")" << endl;

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
