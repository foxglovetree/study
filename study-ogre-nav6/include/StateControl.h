
#pragma once
#include <vector>
#include <Ogre.h>
#include "CostMap.h"

class StateControl
{
};

class CellStateControl : StateControl
{
public:
    static std::string materialNameSelected = "SelectedMaterial";

private:
    int x;
    int y;
    bool selected;
    Ogre::ManualObject *selectedObject;
    Ogre::SceneNode *selectedNode;

public:
    CellStateControl(int x, int y, Ogre::SceneManager *sceneMgr):x(x),y(y)
    {
        selectedObject = sceneMgr->createManualObject("SelectedCellObject");
        selectedNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        selectedNode->attachObject(selectedObject);
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

    void rebuildSelected()
    {
        selectedObject->clear();
        selectedObject->begin(materialNameSelected, Ogre::RenderOperation::OT_TRIANGLE_LIST);
       


        auto verticesInner = CostMap::calculateVerticesForXZ(x, y, hexSize, 0.75f);
        auto verticesOuter = CostMap::calculateVerticesForXZ(x, y, hexSize, 0.95f);

        drawHexagonRing(selectedObject, verticesInner, verticesOuter, ColourValue(1.0f, 1.0f, 0.8f, 0.0f), ColourValue(1.0f, 1.0f, 0.8f, 0.6f));

        
        selectedObject->end();
    }

    bool getSelected()
    {
        return this->selected;
    }
};

class WorldStateControl : StateControl
{
protected:
    std::vector<std::vector<CellStateControl *>> cells;
    Ogre::SceneManager *sceneMgr;
    int width;
    int height;

public:
    WorldStateControl(Ogre::SceneManager *sceneMgr, int width, int height) : width(width), height(height)
    {
        cells.resize(height, std::vector<CellStateControl *>(width, nullptr));
        this->sceneMgr = sceneMgr;
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                cells[j][i] = new CellStateControl(sceneMgr);
            }
        }
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