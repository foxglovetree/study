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
#include "CostMap.h"
#include "InputState.h"

// === Custom hash function ===

// === Frame Listener class for main loop ===
class CameraUpdater : public Ogre::FrameListener
{
private:
    GridRender *render;
    bool quit;

    InputState &inputState;

public:
    CameraUpdater(GridRender *viz, InputState &inputState) : render(viz), quit(false), inputState(inputState) {}

    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        // std::cout << "Frame started!\n";

        // Move camera
        Ogre::Camera *camera = render->getCamera();
        Ogre::SceneNode *node = camera->getParentSceneNode();
        // 获取当前朝向（四元数）
        // Ogre::Quaternion orientation = node->getOrientation();

        // 计算右向量（X轴）
        Ogre::Vector3 right = Ogre::Vector3::UNIT_X;
        Ogre::Vector3 back = Ogre::Vector3::UNIT_Z;

        float speed = 1000.0f;

        if (inputState.front)
        {
            node->translate(-back * speed * evt.timeSinceLastFrame);
        }
        if (inputState.back)
        {
            node->translate(back * speed * evt.timeSinceLastFrame);
        }
        if (inputState.left)
        {
            node->translate(-right * speed * evt.timeSinceLastFrame);
        }
        if (inputState.right)
        {
            node->translate(right * speed * evt.timeSinceLastFrame);
        }
       

        // Update visualization
        if (render)
        {
            render->update();
        }

        return true; // Continue rendering
    }
};
