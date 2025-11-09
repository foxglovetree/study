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
#include "fg/InputState.h"
#include "fg/util/CameraUtil.h"
#include "fg/util/Polygon2.h"
#include "fg/State.h"

#include "fg/util/BoolFunctionList.h"
using namespace Ogre;

// === Frame Listener class for main loop ===
class CameraState : public Ogre::FrameListener, public State
{
private:
    bool quit;

    InputState *inputState;
    Ogre::Camera *camera;
    ViewPoint *viewport;
    Polygon2 *borderOnGround;

    BoolFunctionList<CameraState *> validator;

public:
    CameraState(Ogre::Camera *camera, InputState *inputState) : quit(false)
    {

        this->camera = camera;
        this->inputState = inputState;
        this->borderOnGround = nullptr;
    }
    void setBorderOnGround(Polygon2 *bog)
    {
        this->borderOnGround = bog;
    }

    bool isViewportInBorderOfGround(Vector3 &position, Quaternion &orientation, Polygon2 *borderOnGround)
    {

        Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0);
        // Ray ray = state->camera->getCameraToViewportRay(0.5f, 0.5f);

        Ogre::Vector3 direction = orientation * Ogre::Vector3::NEGATIVE_UNIT_Z;
        Ogre::Ray ray(position, direction);
        //
        auto hitGrd = ray.intersects(ground);
        if (!hitGrd.first)
        {
            return false;
        }
        Vector3 viewCenterOnGround = ray.getPoint(hitGrd.second);

        return this->borderOnGround->isPointInPolygon(viewCenterOnGround.x, viewCenterOnGround.z);
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
        Vector3 step = Ogre::Vector3::ZERO;
        if (inputState->front)
        {
            // node->translate(-back * speed * evt.timeSinceLastFrame);
            step += -back * speed * evt.timeSinceLastFrame;
        }
        if (inputState->back)
        {
            step += (back * speed * evt.timeSinceLastFrame);
        }
        if (inputState->left)
        {
            step += (-right * speed * evt.timeSinceLastFrame);
        }
        if (inputState->right)
        {
            step += (right * speed * evt.timeSinceLastFrame);
        }

        Vector3 position2 = position + step;
        bool validTranlate = true;
        if (this->borderOnGround)
        {

            Quaternion orientation = node->getOrientation();

            validTranlate = this->isViewportInBorderOfGround(position2, orientation, this->borderOnGround);
        }
        if (validTranlate)
        {
            node->translate(step);
        }

        return true; // Continue rendering
    }
};
