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
// === Custom hash function ===
//
// === Hexagonal Map Visualizer class ===
class HexMapVisualizer
{
private:
    Ogre::SceneManager *sceneMgr;
    Ogre::RenderWindow *window;
    Ogre::Camera *camera;
    Ogre::SceneNode *cameraNode;
    Ogre::ManualObject *hexGridObject;
    Ogre::ManualObject *pathObject;
    Ogre::SceneNode *gridNode;
    Ogre::SceneNode *pathNode;
    float hexSize;

    // Current state
    const GridManager *currentGrid;
    std::vector<Ogre::Vector2> currentPath;
    int startx, starty, endx, endy;
    bool gridDirty;
    bool pathDirty;

    std::string materialNameToCreate="ABC";
    std::string materialNameInUse="ABC";
public:
    HexMapVisualizer(Ogre::SceneManager *mgr, Ogre::RenderWindow *win, float size = 30.0f)
        : sceneMgr(mgr), window(win), hexSize(size),
          currentGrid(nullptr), startx(-1), starty(-1), endx(-1), endy(-1),
          gridDirty(false), pathDirty(false)
    {

        // 假设你已经有 sceneMgr 和 camera
        Ogre::Light *light = sceneMgr->createLight("MyPointLight");
        light->setType(Ogre::Light::LT_POINT);
        light->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));  // 白色漫反射
        light->setSpecularColour(Ogre::ColourValue(1.0, 1.0, 1.0)); // 白色镜面光

        Ogre::SceneNode *lightNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->setPosition(0, 0, 500);
        lightNode->attachObject(light);
        // Create camera
        camera = sceneMgr->createCamera("HexMapCamera");
        camera->setNearClipDistance(0.1f);
        camera->setFarClipDistance(1000.0f);
        camera->setAutoAspectRatio(true);

        // Create camera node and set position and direction
        cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        cameraNode->setPosition(0, -500, 500);
        cameraNode->attachObject(camera);
        cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_PARENT);

        // Create viewport
        Ogre::Viewport *vp = window->addViewport(camera);
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
        createVertexColourMaterial();
    }
    // 在你的 HexMapVisualizer 构造函数或初始化函数中调用
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
        pass->setVertexColourTracking(TrackVertexColourEnum::TVC_DIFFUSE);//漫反射
        // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_AMBIENT);//环境光
        //pass->setVertexColourTracking(TrackVertexColourEnum::TVC_EMISSIVE);//自发光
        // pass->setVertexColourTracking(TrackVertexColourEnum::TVC_SPECULAR);//镜面反射
        return mat;
    }

    Ogre::Camera *getCamera()
    {
        return this->camera;
    }

    void setGrid(const GridManager &grid)
    {
        currentGrid = &grid;
        gridDirty = true;
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

        if (!currentGrid)
            return;

        if (gridDirty)
        {
            drawHexGrid();
            gridDirty = false;
        }

        if (pathDirty)
        {
            drawPath();
            pathDirty = false;
        }
    }

private:
    void drawHexGrid()
    {
        std::cout << "Drawing hex grid... width=" << currentGrid->getWidth()
                  << ", height=" << currentGrid->getHeight() << std::endl;
        if (!currentGrid)
            return;

        hexGridObject->clear();

        int width = currentGrid->getWidth();
        int height = currentGrid->getHeight();

        // Begin the manual object
        hexGridObject->begin(materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int cost = currentGrid->getCost(x, y);
                Ogre::ColourValue color = getCostColor(cost);

                auto vertices = currentGrid->getHexagonVertices(x, y, hexSize);

                // Draw hexagon (triangle fan)
                if (cost == GridManager::OBSTACLE)
                {
                    // Obstacles in red
                    drawHexagonTo(hexGridObject, vertices, color);
                }
                else
                {
                    // Normal terrain with corresponding cost color
                    drawHexagonTo(hexGridObject, vertices, color);
                }
            }
        }

        // End the manual object
        hexGridObject->end();
        std::cout << "End of drawing hex grid" << std::endl;
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

        int width = currentGrid->getWidth();
        int height = currentGrid->getHeight();

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                auto vertices = currentGrid->getHexagonVertices(x, y, hexSize);

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
        case GridManager::OBSTACLE:
            return Ogre::ColourValue::Red;
        case GridManager::DEFAULT_COST:
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
                       const std::vector<Ogre::Vector3> &vertices,
                       const Ogre::ColourValue &color)
    {
        // Compute center
        Ogre::Vector3 center(0, 0, 0);
        for (auto &v : vertices)
            center += v;
        center *= (1.0f / 6.0f);

        size_t baseIndex = obj->getCurrentVertexCount();

        // Center
        obj->position(center);
        obj->normal(0, 0, 1);
        obj->colour(color * 0.7f);

        // Corners
        for (int i = 0; i < 6; ++i)
        {
            obj->position(vertices[i]);
            obj->normal(0, 0, 1);
            obj->colour(color);
        }

        // Triangles
        for (int i = 0; i < 6; ++i)
        {
            int next = (i + 1) % 6;
            obj->triangle(baseIndex, baseIndex + i + 1, baseIndex + next + 1);
        }
    }
};

