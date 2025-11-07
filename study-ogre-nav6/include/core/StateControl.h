
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "util/CostMap.h"
#include "util/Component.h"
using namespace Ogre;

// Base class for model data and control.
class StateControl : public Component
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
    StateControl(){
        
    }
    void init() override
    {
        Component::init();
    }
};

class SimpleStateControl : StateControl
{
protected:
    ManualObject *obj;
    SceneNode *node;

public:
    SimpleStateControl(Ogre::SceneManager *sceneMgr)
    {
        obj = sceneMgr->createManualObject();
        node = sceneMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(obj);
        buildMesh();
    }

    void buildMesh()
    {
        obj->clear();

        // Begin the manual object
        obj->begin(StateControl::materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        obj->end();
    }
    virtual void buildMeshInternal(ManualObject *obj) = 0;
};

class CostMapControl : public StateControl
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
