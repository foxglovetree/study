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
#include "MaterialFactory.h"
#include "StateControl.h"

// === Custom hash function ===
//
// === Hexagonal Map Visualizer class ===
using namespace Ogre;
using namespace OgreBites;

class MaterialFactory
{
private:
    // 在你的 HexMapVisualizer 构造函数或初始化函数中调用
    static Ogre::MaterialPtr createVertexColourMaterialForSelected()
    {
        using namespace Ogre;

        // 创建材质，名称和资源组
        MaterialPtr mat = MaterialManager::getSingleton().create(CellStateControl::materialNameSelected, "General");

        // 禁用阴影接收
        mat->setReceiveShadows(false);
        mat->setDepthWriteEnabled(false);
        mat->setTransparencyCastsShadows(false);

        // 获取默认技术（Ogre 2.x 默认会自动创建一个）
        Technique *tech = mat->getTechnique(0);

        // 配置 Pass
        Pass *pass = tech->getPass(0);
        pass->setVertexColourTracking(TrackVertexColourEnum::TVC_EMISSIVE); // 自发光
        pass->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
        pass->setDepthCheckEnabled(false);
        pass->setLightingEnabled(false);
        pass->setSelfIllumination(1, 1, 0.8);
        pass->setDepthBias(1.0f, 0.0f);
        return mat;
    }
    static Ogre::MaterialPtr createVertexColourMaterial()
    {
        using namespace Ogre;

        // 创建材质，名称和资源组
        MaterialPtr mat = MaterialManager::getSingleton().create(StateControl::materialNameToCreate, "General");

        // 禁用阴影接收
        mat->setReceiveShadows(false);

        // 获取默认技术（Ogre 2.x 默认会自动创建一个）
        Technique *tech = mat->getTechnique(0);

        // 配置 Pass
        Pass *pass = tech->getPass(0);
        pass->setLightingEnabled(true);
        pass->setVertexColourTracking(TrackVertexColourEnum::TVC_DIFFUSE); // 漫反射
        // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_AMBIENT);//环境光
        // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_EMISSIVE);//自发光
        // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_SPECULAR);//镜面反射
        return mat;
    }

    void update()
    {
    }

public:
    WorldStateControl *wsc;
    static void createMaterials()
    {
        //
        // Create hexagonal grid object

        //
        createVertexColourMaterial();
        createVertexColourMaterialForSelected(); // for selected
    }
};
