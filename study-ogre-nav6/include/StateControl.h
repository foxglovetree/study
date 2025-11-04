
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "CostMap.h"
using namespace Ogre;

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

class CellStateControl : StateControl
{
public:
    static inline const std::string materialNameSelected = "SelectedMaterial";

private:
    int x;
    int y;
    bool selected;
    Ogre::ManualObject *hexGridObject;
    Ogre::SceneNode *gridNode;
    Ogre::ManualObject *selectedObject;
    Ogre::SceneNode *selectedNode;
    CostMap *costMap;

public:
    CellStateControl(CostMap *costMap, int x, int y, Ogre::SceneManager *sceneMgr) : x(x), y(y), costMap(costMap)
    {
        selectedObject = sceneMgr->createManualObject();
        selectedNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        selectedNode->attachObject(selectedObject);

        // Create hexagonal grid object
        hexGridObject = sceneMgr->createManualObject();
        gridNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        gridNode->attachObject(hexGridObject);
        //
        rebuildCellMesh();
    }
    void setSelected(bool selected)
    {
        if (this->selected == selected)
        {
            return;
        }
        this->selected = selected;
        this->rebuildSelected();
    }

    void rebuildCellMesh()
    {

        hexGridObject->clear();

        // Begin the manual object
        hexGridObject->begin(StateControl::materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        int cost = costMap->getCost(x, y);
        Ogre::ColourValue color = getCostColor(cost);
        auto vertices = CostMap::calculateVerticesForXZ(x, y, CostMap::hexSize);
        StateControl::drawHexagonTo(hexGridObject, vertices, color);

        // End the manual object
        hexGridObject->end();
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

    void rebuildSelected()
    {
        selectedObject->clear();
        selectedObject->begin(materialNameSelected, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        auto verticesInner = CostMap::calculateVerticesForXZ(x, y, CostMap::hexSize, 0.75f);
        auto verticesOuter = CostMap::calculateVerticesForXZ(x, y, CostMap::hexSize, 0.95f);

        drawHexagonRing(selectedObject, verticesInner, verticesOuter, ColourValue(1.0f, 1.0f, 0.8f, 0.0f), ColourValue(1.0f, 1.0f, 0.8f, 0.6f));

        selectedObject->end();
    }

    void drawHexagonRing(Ogre::ManualObject *obj,
                         const std::vector<Ogre::Vector2> &verticesInner,
                         const std::vector<Ogre::Vector2> &verticesOuter,
                         const Ogre::ColourValue &colorInner,
                         Ogre::ColourValue &colorOuter)
    {
        const float nomX = 0;
        const float nomY = 1;
        const float nomZ = 0;
        int baseIndex = obj->getCurrentVertexCount();
        for (int i = 0; i < 6; i++)
        {

            obj->position(verticesInner[i].x, 0, verticesInner[i].y);
            obj->normal(nomX, nomY, nomZ);
            obj->colour(colorInner);

            obj->position(verticesOuter[i].x, 0, verticesOuter[i].y);
            obj->normal(nomX, nomY, nomZ);
            obj->colour(colorOuter);
        }

        // Triangles
        for (int i = 0; i < 6; ++i)
        {
            int p1 = baseIndex + i * 2;
            int p2 = p1 + 1;
            int p3 = baseIndex + ((i + 1) % 6) * 2 + 1;
            int p4 = p3 - 1;

            obj->triangle(p1, p2, p3);
            obj->triangle(p1, p3, p4);
        }
    }

    bool getSelected()
    {
        return this->selected;
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
class WorldStateControl : StateControl
{
protected:
    std::vector<std::vector<CellStateControl *>> cells;
    Ogre::SceneManager *sceneMgr;
    
    CostMap *costMap;
    CostMapControl *costMapControl;

public:
    WorldStateControl(CostMap *costMap, Ogre::SceneManager *sceneMgr) : costMap(costMap)
    {
        this->sceneMgr = sceneMgr;
        this->costMapControl = new CostMapControl(costMap);
        int width = costMap->getWidth();
        int height = costMap->getHeight();
        cells.resize(costMap->getHeight(), std::vector<CellStateControl *>(costMap->getWidth(), nullptr));
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                cells[j][i] = new CellStateControl(costMap, i, j, sceneMgr);
            }
        }
    }
    CostMap* getCostMap(){
        return costMap;
    }
    void pickupCell(int cx, int cy)
    {
        cells[cy][cx]->setSelected(true);
    }

    bool isSelected(int cx, int cy)
    {
        return cells[cy][cx]->getSelected();
    }
};