
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "StateControl.h"
using namespace Ogre;

//
class CellFocusStateControl : StateControl
{
public:
    static inline const std::string materialNameSelected = "SelectedMaterial";

private:
    Ogre::ManualObject *hexGridObject;
    Ogre::SceneNode *gridNode;
    Ogre::ManualObject *selectedObject;
    Ogre::SceneNode *selectedNode;
    CostMap *costMap;

public:
    CellFocusStateControl(CostMap *costMap, Ogre::SceneManager *sceneMgr) : costMap(costMap)
    {
        selectedObject = sceneMgr->createManualObject();
        selectedNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        selectedNode->attachObject(selectedObject);
        this->rebuildSelected();
        selectedObject->setVisible(false);
    }

    void select(int x, int y)
    {
        selectedObject->setVisible(true);
        Ogre::Vector3 center = CostMap::calculateCenterForXZ(x, y);
        selectedNode->setPosition(center);
    }

    void rebuildSelected()
    {
        selectedObject->clear();
        selectedObject->begin(materialNameSelected, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        auto verticesInner = CostMap::calculateVerticesForXZ(CostMap::hexSize, 0.75f);
        auto verticesOuter = CostMap::calculateVerticesForXZ(CostMap::hexSize, 0.95f);

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
};
