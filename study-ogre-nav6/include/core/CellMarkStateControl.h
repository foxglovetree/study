
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "StateControl.h"
#include "util/CellMark.h"
using namespace Ogre;

//
class CellMarkStateControl : StateControl
{
public:
    static inline const std::string materialNameSelected = "SelectedMaterial";

private:
    Ogre::ManualObject *obj;
    Ogre::SceneNode *node;
    CostMap *costMap;
    MarkType markType;
    std::unordered_set<CellKey, PairHash> marks;

public:
    CellMarkStateControl(CostMap *costMap, Ogre::SceneManager *sceneMgr,
                         MarkType type) : costMap(costMap), markType(type)
    {
        obj = sceneMgr->createManualObject();
        node = sceneMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(obj);
    }

    void mark(CellKey key, bool mark)
    {
        bool rt = false;
        
        if(mark){
            marks.insert(key);
        } else {
            rt = marks.erase(key);
        }

        rebuildMarkMesh();
        
    }

    bool isMarked(CellKey key, MarkType mtyp)
    {
        return marks.find(key) != marks.end();
    }

    void rebuildMarkMesh()
    {
        obj->clear();
        obj->begin(materialNameSelected, Ogre::RenderOperation::OT_TRIANGLE_LIST);
        for (const auto &key : marks)
        {
            int cx = key.first;
            int cy = key.second;
            auto verticesInner = CostMap::calculateVerticesForXZ(cx, cy, CostMap::hexSize, 0.75f);
            auto verticesOuter = CostMap::calculateVerticesForXZ(cx, cy, CostMap::hexSize, 0.95f);
            drawHexagonRing(obj, verticesInner, verticesOuter, ColourValue(1.0f, 1.0f, 0.8f, 0.0f), ColourValue(1.0f, 1.0f, 0.8f, 0.6f));
        }

        obj->end();
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
