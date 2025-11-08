
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/util/CostMap.h"
#include "fg/StateControl.h"
#include "fg/util/HexGridPrinter.h"
#include "fg/util/CellMark.h"

using namespace Ogre;

class PathStateControl : public StateControl<State>
{
    Ogre::ManualObject *pathObject;
    Ogre::SceneNode *pathNode;

    std::vector<Ogre::Vector2> currentPath;

    CostMap *costMap;
    CellKey start = CellKey(-1, -1);
    CellKey end = CellKey(-1, -1);

public:
    PathStateControl() 
    {
        // Create path object
    }
    void init(InitContext &ctx) override
    {
        costMap = this->parent->find<CostMap>();
        Ogre::SceneManager *sceneMgr = parent->find<Ogre::SceneManager>();
        pathObject = sceneMgr->createManualObject("PathObject");
        pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        pathNode->attachObject(pathObject);
    }

    void clearPath()
    {
        this->setPath({}, CellKey(-1, -1), CellKey(-1, -1));
    }

    bool getStart(CellKey &start)
    {
        if (this->start.first == -1)
        {
            return false;
        }
        start = this->start;
        return true;
    }

    void setPath(const std::vector<Ogre::Vector2> &path, CellKey ck1, CellKey ck2)
    {
        currentPath = path;
        start = ck1;
        end = ck2;
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
        pathObject->begin(MaterialNames::materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        int width = costMap->getWidth();
        int height = costMap->getHeight();

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                auto vertices = CostMap::calculateVerticesForXZ(x, y, CostMap::hexSize);

                if (x == start.first && y == start.second)
                {
                    // Start point in green
                    drawHexagonTo(pathObject, vertices, Ogre::ColourValue::Green);
                }
                else if (x == end.first && y == end.second)
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
