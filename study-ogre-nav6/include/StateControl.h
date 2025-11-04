
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "CostMap.h"
using namespace Ogre;

//Base class for model data and control.
class StateControl
{

public:
    // Draw a single hexagon to a specific object
    static inline const std::string materialNameToCreate = "ABC";
    static inline const std::string materialNameInUse = "ABC";
    static void drawHexagonTo(Ogre::ManualObject *obj,
                              const std::vector<Ogre::Vector2> &vertices,
                              const Ogre::ColourValue &color1)
    {
        drawHexagonTo(obj, vertices, color1, color1);
    }

    static void drawHexagonTo(Ogre::ManualObject *obj,
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
};


class PathStateControl : StateControl
{
    Ogre::ManualObject *pathObject;
    Ogre::SceneNode *pathNode;

    std::vector<Ogre::Vector2> currentPath;
    int startx = -1;
    int starty = -1;
    int endx = -1;
    int endy = -1;
    int width;
    int height;

public:
    PathStateControl(int width, int height, Ogre::SceneManager *sceneMgr) : width(width), height(height)
    {
        // Create path object
        pathObject = sceneMgr->createManualObject("PathObject");
        pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        pathNode->attachObject(pathObject);
    }

    void findPath(CostMap *costMap)
    {
        // Find path

        auto path = costMap->findPath(1, 1, 10, 8);
        std::cout << "Path found with " << path.size() << " hexes\n";
        if (!path.empty())
        {
            float totalCost = costMap->calculatePathCost(path);
            std::cout << "Total path cost: " << totalCost << "\n";
            std::cout << "Path: ";
            for (const auto &p : path)
            {
                std::cout << "(" << (int)p.x << "," << (int)p.y << ") ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        HexGridPrinter::printPathGrid(costMap, 1, 1, 10, 8, path);
        setPath(path, 1, 1, 10, 8);
    }

    void setPath(const std::vector<Ogre::Vector2> &path, int sx, int sy, int ex, int ey)
    {
        currentPath = path;
        startx = sx;
        starty = sy;
        endx = ex;
        endy = ey;
        this->rebuild();
    }

    void rebuild()
    {

        pathObject->clear();

        // Create path points set for quick lookup
        std::unordered_set<std::pair<int, int>, PairHash> pathSet;
        for (const auto &p : currentPath)
        {
            pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
        }

        // Begin the manual object
        pathObject->begin(materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                auto vertices = CostMap::calculateVerticesForXZ(x, y, CostMap::hexSize);

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
};
class CostMapControl : StateControl
{

public:
    CostMapControl(CostMap *costMap)
    {
        for (int i = 3; i < 8; i++)
        {
            costMap->setCost(i, 4, 2);
        }

        // Water: cost 3
        for (int i = 2; i < 6; i++)
        {
            costMap->setCost(6, i, 3);
        }

        // Low cost path (like road)
        for (int i = 2; i < 10; i++)
        {
            costMap->setCost(i, 7, 1);
        }

        // Obstacles
        costMap->setCost(4, 3, CostMap::OBSTACLE);
        costMap->setCost(7, 5, CostMap::OBSTACLE);
    }
};
