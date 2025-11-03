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
#include "GridManager.h"
#include "HexMapVisualizer.h"
#include "InputState.h"

// === Custom hash function ===


// === Frame Listener class for main loop ===
class CameraUpdater : public Ogre::FrameListener
{
private:
    HexMapVisualizer *visualizer;
    bool quit;

    InputState& inputState;

public:
    CameraUpdater(HexMapVisualizer *viz, InputState& inputState) : visualizer(viz), quit(false),inputState(inputState) {}

    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        // std::cout << "Frame started!\n";

        // Move camera
        Ogre::Camera *camera = visualizer->getCamera();
        Ogre::SceneNode *node = camera->getParentSceneNode();
        // 获取当前朝向（四元数）
        //Ogre::Quaternion orientation = node->getOrientation();

        // 计算右向量（X轴）
        Ogre::Vector3 right = Ogre::Vector3::UNIT_X;
        Ogre::Vector3 up = Ogre::Vector3::UNIT_Y;
        float speed = 1000.0f;

        if (inputState.up)
        {
            node->translate(up * speed * evt.timeSinceLastFrame);
        }
        if (inputState.down)
        {
            node->translate(-up * speed * evt.timeSinceLastFrame);
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
        if (visualizer)
        {
            visualizer->update();
        }

        return true; // Continue rendering
    }
};
