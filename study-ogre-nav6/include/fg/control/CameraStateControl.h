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
#include "fg/util/CostMap.h"

#include "fg/StateControl.h"
#include "fg/InputState.h"
#include "fg/util/CameraUtil.h"
#include "fg/util/Polygon2.h"

using namespace Ogre;

// === Frame Listener class for main loop ===
class CameraStateControl : public Ogre::FrameListener, public StateControl<State>
{
private:
    bool quit;

    InputState *inputState;
    Ogre::Camera *camera;
    ViewPoint *viewport;
    CostMap *costMap;

public:
    CameraStateControl() : quit(false)
    {
    }
    void init(InitContext&ctx) override
    {

        //
        this->costMap = this->parent->find<CostMap>();
        this->camera = this->parent->find<Ogre::Camera>();
        this->inputState = this->parent->find<InputState>();
    }

    bool checkViewportInBorderOfGround()
    {

        // todo cache border on ground
        int width = costMap->getWidth();
        int height = costMap->getHeight();
        float rX = width * CostMap::hexSize * 2;
        float rZ = height * CostMap::hexSize * 2;
        Polygon2 borderOnGround(Vector2(0, 0), Vector2(0, rZ), Vector2(rX, rZ), Vector2(rX, 0));
        //

        Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0);
        Ray ray = camera->getCameraToViewportRay(0.5f, 0.5f);
        auto hitGrd = ray.intersects(ground);
        if (!hitGrd.first)
        {
            return false;
        }
        Vector3 viewCenterOnGround = ray.getPoint(hitGrd.second);
        return borderOnGround.isPointInPolygon(viewCenterOnGround.x, viewCenterOnGround.z);
    }

    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        // std::cout << "Frame started!\n";

        // Move camera
        Ogre::SceneNode *node = camera->getParentSceneNode();
        // 获取当前朝向（四元数）
        // Ogre::Quaternion orientation = node->getOrientation();

        // 计算右向量（X轴）
        Ogre::Vector3 right = Ogre::Vector3::UNIT_X;
        Ogre::Vector3 back = Ogre::Vector3::UNIT_Z;

        float speed = 1000.0f;
        Vector3 position = node->getPosition();
        if (inputState->front)
        {
            node->translate(-back * speed * evt.timeSinceLastFrame);
        }
        if (inputState->back)
        {
            node->translate(back * speed * evt.timeSinceLastFrame);
        }
        if (inputState->left)
        {
            node->translate(-right * speed * evt.timeSinceLastFrame);
        }
        if (inputState->right)
        {
            node->translate(right * speed * evt.timeSinceLastFrame);
        }

        if (!checkViewportInBorderOfGround())
        {
            // move back.
            node->setPosition(position);
        }

        return true; // Continue rendering
    }
};
