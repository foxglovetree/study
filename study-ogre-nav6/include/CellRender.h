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
#include "CellManager.h"
// === Custom hash function ===
//
// === Hexagonal Map Visualizer class ===
using namespace Ogre;
using namespace OgreBites;

class CellRender
{
private:
    Ogre::SceneManager *sceneMgr;
    Ogre::RenderWindow *window;
    Ogre::Camera *camera;
    Ogre::SceneNode *cameraNode;
    Ogre::ManualObject *hexGridObject;
    Ogre::ManualObject *pathObject;
    Ogre::ManualObject *selectedObject;

    Ogre::SceneNode *gridNode;
    Ogre::SceneNode *pathNode;
    Ogre::SceneNode *selectedNode;

    float hexSize;

    std::vector<Ogre::Vector2> currentPath;
    int startx, starty, endx, endy;
    bool gridDirty;
    bool pathDirty;

    std::string materialNameToCreate = "ABC";
    std::string materialNameInUse = "ABC";
    std::string materialNameSelected = "SelectedMaterial";
    CellManager *cells;
    Ogre::Viewport *vp;

public:
    CellRender(Ogre::SceneManager *mgr, Ogre::RenderWindow *win, CellManager *cells, float size = 30.0f)
        : sceneMgr(mgr), window(win), hexSize(size),
          cells(cells), startx(-1), starty(-1), endx(-1), endy(-1),
          gridDirty(true), pathDirty(false)
    {

        // 假设你已经有 sceneMgr 和 camera
        Ogre::Light *light = sceneMgr->createLight("MyPointLight");
        light->setType(Ogre::Light::LT_POINT);
        light->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));  // 白色漫反射
        light->setSpecularColour(Ogre::ColourValue(1.0, 1.0, 1.0)); // 白色镜面光

        Ogre::SceneNode *lightNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->setPosition(0, 500, 0);
        lightNode->attachObject(light);
        // Create camera
        camera = sceneMgr->createCamera("HexMapCamera");
        camera->setNearClipDistance(0.1f);
        camera->setFarClipDistance(1000.0f);
        camera->setAutoAspectRatio(true);

        // Create camera node and set position and direction
        cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        cameraNode->setPosition(0, 500, 0); //
        cameraNode->attachObject(camera);
        cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_PARENT);

        // Create viewport
        vp = window->addViewport(camera);
        vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

        // Create hexagonal grid object
        hexGridObject = sceneMgr->createManualObject("HexGridObject");
        gridNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        gridNode->attachObject(hexGridObject);
        //

        // Create path object
        pathObject = sceneMgr->createManualObject("PathObject");
        pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        pathNode->attachObject(pathObject);
        //
        // Create hexagonal grid object
        selectedObject = sceneMgr->createManualObject("SelectedCellObject");
        selectedNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        selectedNode->attachObject(selectedObject);
        //
        createVertexColourMaterial();
        createVertexColourMaterialForSelected(); // for selected
    }
    // 在你的 HexMapVisualizer 构造函数或初始化函数中调用

    Ogre::MaterialPtr createVertexColourMaterialForSelected()
    {
        using namespace Ogre;

        // 创建材质，名称和资源组
        MaterialPtr mat = MaterialManager::getSingleton().create(materialNameSelected, "General");

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
        pass->setDepthCheckEnabled(true);
        pass->setLightingEnabled(false);
        pass->setSelfIllumination(1, 1, 0.8);
        pass->setDepthBias(0, -1);
        return mat;
    }
    Ogre::MaterialPtr createVertexColourMaterial()
    {
        using namespace Ogre;

        // 创建材质，名称和资源组
        MaterialPtr mat = MaterialManager::getSingleton().create(materialNameToCreate, "General");

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
    Ogre::Viewport *getViewport()
    {
        return vp;
    }
    Ogre::Camera *getCamera()
    {
        return this->camera;
    }

    void setPath(const std::vector<Ogre::Vector2> &path, int sx, int sy, int ex, int ey)
    {
        currentPath = path;
        startx = sx;
        starty = sy;
        endx = ex;
        endy = ey;
        pathDirty = true;
    }

    void update()
    {

        if (gridDirty)
        {
            drawHexGrid();
            drawSelected();
            // gridDirty = false;
        }

        if (pathDirty)
        {
            drawPath();
            pathDirty = false;
        }
    }

private:
    void drawHexagonRing(Ogre::ManualObject *obj,
                         const std::vector<Ogre::Vector2> &verticesInner,
                         const std::vector<Ogre::Vector2> &verticesOuter,
                         const Ogre::ColourValue &colorInner,
                         ColourValue &colorOuter)
    {
        const float nomX = 0;
        const float nomY = 1;
        const float nomZ = 0;

        for (int i = 0; i < 7; i++)
        {
            int idx = i % 6;
            obj->position(verticesInner[idx].x, 0, verticesInner[idx].y);
            obj->normal(nomX, nomY, nomZ);
            obj->colour(colorInner);

            obj->position(verticesOuter[idx].x, 0, verticesOuter[idx].y);
            obj->normal(nomX, nomY, nomZ);
            obj->colour(colorOuter);
        }
    }

    void drawSelected()
    {
        selectedObject->clear();
        int width = cells->getWidth();
        int height = cells->getHeight();
        CostMap &costMap = cells->getCostMap();
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (cells->getSelected(x, y))
                {
                    selectedObject->begin(materialNameSelected, Ogre::RenderOperation::OT_TRIANGLE_STRIP);

                    auto verticesInner = costMap.getHexagonVerticesForXZ(x, y, hexSize , 0.75f);
                    auto verticesOuter = costMap.getHexagonVerticesForXZ(x, y, hexSize , 0.95f);
                    drawHexagonRing(selectedObject, verticesInner, verticesOuter, ColourValue(1.0f, 1.0f, 0.8f, 0.0f), ColourValue(1.0f, 1.0f, 0.8f, 0.6f));
                    selectedObject->end();
                }
            }
        }
    }

    void drawHexGrid()
    {

        hexGridObject->clear();

        int width = cells->getWidth();
        int height = cells->getHeight();
        CostMap &costMap = cells->getCostMap();

        // Begin the manual object
        hexGridObject->begin(materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int cost = costMap.getCost(x, y);
                Ogre::ColourValue color = getCostColor(cost);
                auto vertices = costMap.getHexagonVerticesForXZ(x, y, hexSize);
                drawHexagonTo(hexGridObject, vertices, color);
            }
        }
        // End the manual object
        hexGridObject->end();
    }

    void drawPath()
    {
        std::cout << "Drawing path ..." << std::endl;
        if (currentPath.empty())
        {
            pathObject->clear();
            return;
        }

        pathObject->clear();

        // Create path points set for quick lookup
        std::unordered_set<std::pair<int, int>, PairHash> pathSet;
        for (const auto &p : currentPath)
        {
            pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
        }

        // Begin the manual object
        pathObject->begin(materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        int width = cells->getWidth();
        int height = cells->getHeight();
        CostMap &costMap = cells->getCostMap();

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                auto vertices = costMap.getHexagonVerticesForXZ(x, y, hexSize);

                if (x == startx && y == starty)
                {
                    // Start point in green
                    drawHexagonTo(pathObject, vertices, Ogre::ColourValue::Green);
                }
                else if (x == endx && y == endy)
                {
                    // End point in blue
                    drawHexagonTo(pathObject, vertices, Ogre::ColourValue::Blue);
                }
                else if (pathSet.find({x, y}) != pathSet.end())
                {
                    // Path in yellow
                    drawHexagonTo(pathObject, vertices, Ogre::ColourValue(1.0f, 1.0f, 0.0f)); // Yellow
                }
            }
        }

        // End the manual object
        pathObject->end();
        std::cout << "End of drawing path." << std::endl;
    }

    // Get color based on cost
    Ogre::ColourValue getCostColor(int cost) const
    {
        switch (cost)
        {
        case CostMap::OBSTACLE:
            return Ogre::ColourValue::Red;
        case CostMap::DEFAULT_COST:
            return Ogre::ColourValue(0.8f, 0.6f, 0.2f); // light Sand color
        case 2:
            return Ogre::ColourValue(0.6f, 0.4f, 0.1f); // Dark Sand color
        case 3:
            return Ogre::ColourValue(0.2f, 0.4f, 0.8f); // Water color
        default:
            return Ogre::ColourValue(0.7f, 0.7f, 0.7f); // light gray
        }
    }

    // Draw a single hexagon to a specific object
    void drawHexagonTo(Ogre::ManualObject *obj,
                       const std::vector<Ogre::Vector2> &vertices,
                       const Ogre::ColourValue &color1)
    {
        drawHexagonTo(obj, vertices, color1, color1);
    }

    void drawHexagonTo(Ogre::ManualObject *obj,
                       const std::vector<Ogre::Vector2> &vertices,
                       const Ogre::ColourValue &color1, ColourValue color2)
    {
        const float nomX = 0;
        const float nomY = 1;
        const float nomZ = 0;
        // Compute center
        Ogre::Vector2 center(0, 0);
        for (auto &v : vertices)
            center += v;
        center *= (1.0f / 6.0f);

        size_t baseIndex = obj->getCurrentVertexCount();

        // Center
        obj->position(center.x, 0, center.y);
        obj->normal(nomX, nomY, nomZ);
        obj->colour(color1);

        // Corners
        for (int i = 0; i < 6; ++i)
        {
            obj->position(vertices[i].x, 0, vertices[i].y);
            obj->normal(nomX, nomY, nomZ);
            obj->colour(color2);
        }

        // Triangles
        for (int i = 0; i < 6; ++i)
        {
            int p1 = baseIndex + i + 1;
            int p2 = baseIndex + (i + 1) % 6 + 1;
            obj->triangle(baseIndex, p1, p2);
        }
    }

    bool isPointInCell(float px, float py, int cx, int cy)
    {
        CostMap &costMap = cells->getCostMap();
        auto corners = costMap.getHexagonVerticesForXZ(cx, cy, hexSize);

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

public:
    bool findCellByPoint(float px, float py, int &cx, int &cy)
    {
        int width = cells->getWidth();
        int height = cells->getHeight();
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
};
