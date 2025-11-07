
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "StateControl.h"
using namespace Ogre;

//
class CellStateControl : public StateControl
{
public:
private:
    Ogre::ManualObject *obj;
    Ogre::SceneNode *node;
    CostMap *costMap;

public:
    CellStateControl(CostMap *costMap) : costMap(costMap)
    {
        // Create hexagonal grid object         

        /*
        obj = sceneMgr->createManualObject();
        node = sceneMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(obj);
        //
        buildCellMesh();
        */
    }
    void init() override
    {
        Ogre::SceneManager *sceneMgr = parent->find<Ogre::SceneManager>();
        
        obj = sceneMgr->createManualObject();
        node = sceneMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(obj);
        //
        buildCellMesh();
        StateControl::init();
    }

    void buildCellMesh()
    {

        obj->clear();

        // Begin the manual object
        obj->begin(StateControl::materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);
        int width = costMap->getWidth();
        int height = costMap->getHeight();
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                int cost = costMap->getCost(x, y);
                Ogre::ColourValue color = getCostColor(cost);
                auto vertices = CostMap::calculateVerticesForXZ(x, y, CostMap::hexSize);
                StateControl::drawHexagonTo(obj, vertices, color);
            }
        }

        // End the manual object
        obj->end();
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
};
