
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "CostMap.h"
#include "StateControl.h"
#include "HexGridPrinter.h"
using namespace Ogre;

class PathStateControl : StateControl
{
    Ogre::ManualObject *pathObject;
    Ogre::SceneNode *pathNode;

    std::vector<Ogre::Vector2> currentPath;
    int startx = -1;
    int starty = -1;
    int endx = -1;
    int endy = -1;
    CostMap *costMap;

public:
    PathStateControl(CostMap *costMap, Ogre::SceneManager *sceneMgr) : costMap(costMap)
    {
        // Create path object
        pathObject = sceneMgr->createManualObject("PathObject");
        pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        pathNode->attachObject(pathObject);
    }

    void clearPath(){
        this->setPath(currentPath, -1,-1,-1,-1);
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

        int width = costMap->getWidth();
        int height = costMap->getHeight();

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
    }
};
