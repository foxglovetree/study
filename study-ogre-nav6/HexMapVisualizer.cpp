#include "HexMapVisualizer.h"
#include <iostream>
#include <cmath>

HexMapVisualizer::HexMapVisualizer(Ogre::SceneManager* mgr, Ogre::RenderWindow* win, float size)
    : sceneMgr(mgr), window(win), hexSize(size),
      currentGrid(nullptr), startx(-1), starty(-1), endx(-1), endy(-1),
      gridDirty(false), pathDirty(false),
      mainLight(nullptr), lightNode(nullptr)
{
    camera = sceneMgr->createCamera("HexMapCam");
    camera->setNearClipDistance(5.0f);
    camera->setFarClipDistance(1000.0f);
    camera->setAutoAspectRatio(true);

    cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    cameraNode->setPosition(-400, 300, 300);
    cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_WORLD);
    cameraNode->attachObject(camera);

    Ogre::Viewport* vp = window->addViewport(camera);
    vp->setBackgroundColour(Ogre::ColourValue(0.1f, 0.1f, 0.2f));

    createLights();

    hexGridObject = sceneMgr->createManualObject("HexGridObj");
    gridNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    gridNode->attachObject(hexGridObject);

    pathObject = sceneMgr->createManualObject("PathObj");
    pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    pathNode->attachObject(pathObject);
}

HexMapVisualizer::~HexMapVisualizer()
{
    if (mainLight && lightNode) {
        sceneMgr->destroyLight(mainLight);
        mainLight = nullptr;
    }
}

void HexMapVisualizer::setGrid(const HexNavigationGrid& grid)
{
    currentGrid = &grid;
    gridDirty = true;
}

void HexMapVisualizer::setPath(const std::vector<Ogre::Vector2>& path, int sx, int sy, int ex, int ey)
{
    currentPath = path;
    startx = sx; starty = sy;
    endx = ex; endy = ey;
    pathDirty = true;
}

void HexMapVisualizer::update()
{
    if (!currentGrid) return;
    if (gridDirty) { drawHexGrid(); gridDirty = false; }
    if (pathDirty) { drawPath(); pathDirty = false; }
}

void HexMapVisualizer::createLights()
{
    mainLight = sceneMgr->createLight("MainLight");
    mainLight->setType(Ogre::Light::LT_DIRECTIONAL);
    mainLight->setDiffuseColour(1.0f, 1.0f, 1.0f);
    mainLight->setSpecularColour(0.8f, 0.8f, 0.8f);

    lightNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    lightNode->attachObject(mainLight);
    lightNode->setDirection(Ogre::Vector3(-1, -1, -0.5).normalisedCopy(), Ogre::Node::TS_WORLD);

    sceneMgr->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
}

float HexMapVisualizer::getHeightForCell(int x, int y) const
{
    int cost = currentGrid->getCost(x, y);
    if (cost == HexNavigationGrid::OBSTACLE) return 25.0f;
    if (cost == 3) return 2.0f;
    if (cost == 2) return 8.0f;
    return 5.0f;
}

Ogre::ColourValue HexMapVisualizer::getCostColor(int cost) const
{
    switch (cost) {
        case HexNavigationGrid::OBSTACLE: return Ogre::ColourValue::Red;
        case 1: return Ogre::ColourValue(0.5f, 0.5f, 0.5f);   // Default
        case 2: return Ogre::ColourValue(0.8f, 0.6f, 0.2f);   // Sand
        case 3: return Ogre::ColourValue(0.2f, 0.4f, 0.8f);   // Water
        default: return Ogre::ColourValue(0.7f, 0.7f, 0.7f);
    }
}

void HexMapVisualizer::drawHexGrid()
{
    if (!currentGrid) return;
    hexGridObject->clear();
    hexGridObject->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    int w = currentGrid->getWidth(), h = currentGrid->getHeight();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            auto verts = currentGrid->getHexagonVertices(x, y, hexSize);
            int cost = currentGrid->getCost(x, y);
            Ogre::ColourValue color = getCostColor(cost);
            float height = getHeightForCell(x, y);
            drawHexPrismTo(hexGridObject, verts, color, height);
        }
    }

    hexGridObject->end();
}

void HexMapVisualizer::drawPath()
{
    if (currentPath.empty()) {
        pathObject->clear();
        return;
    }

    pathObject->clear();
    pathObject->begin("BaseWhite", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    std::unordered_set<std::pair<int, int>, PairHash> pathSet;
    for (auto& p : currentPath) {
        pathSet.insert({int(p.x), int(p.y)});
    }

    int w = currentGrid->getWidth(), h = currentGrid->getHeight();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            auto verts = currentGrid->getHexagonVertices(x, y, hexSize);
            float height = 0.0f;
            Ogre::ColourValue color;

            if (x == startx && y == starty) {
                color = Ogre::ColourValue::Green;
                height = 30.0f;
            } else if (x == endx && y == endy) {
                color = Ogre::ColourValue::Blue;
                height = 35.0f;
            } else if (pathSet.count({x, y})) {
                color = Ogre::ColourValue(1.0f, 1.0f, 0.0f); // Yellow
                height = 15.0f;
            } else continue;

            drawHexPrismTo(pathObject, verts, color, height);
        }
    }

    pathObject->end();
}

void HexMapVisualizer::drawHexPrismTo(Ogre::ManualObject* obj,
                                      const std::vector<Ogre::Vector3>& vertices,
                                      const Ogre::ColourValue& color,
                                      float height)
{
    size_t baseIdx = obj->getCurrentVertexCount();

    // Top face
    Ogre::Vector3 topNorm = Ogre::Vector3::UNIT_Z;
    for (int i = 0; i < 6; ++i) {
        obj->position(vertices[i]); obj->colour(color); obj->normal(topNorm);
    }
    for (int i = 0; i < 4; ++i) {
        obj->triangle(baseIdx, baseIdx + i + 1, baseIdx + i + 2);
    }
    obj->triangle(baseIdx, baseIdx + 5, baseIdx + 1);

    // Bottom face
    size_t botStart = baseIdx + 6;
    Ogre::Vector3 botNorm = -Ogre::Vector3::UNIT_Z;
    std::vector<Ogre::Vector3> botVerts;
    for (auto& v : vertices) botVerts.push_back(v - Ogre::Vector3(0,0,height));
    for (int i = 0; i < 6; ++i) {
        obj->position(botVerts[i]); obj->colour(color * 0.6f); obj->normal(botNorm);
    }
    for (int i = 0; i < 4; ++i) {
        obj->triangle(botStart, botStart + i + 2, botStart + i + 1);
    }
    obj->triangle(botStart, botStart + 1, botStart + 5);

    // Sides
    size_t sideStart = botStart + 6;
    for (int i = 0; i < 6; ++i) {
        int next = (i+1)%6;
        Ogre::Vector3 edge = vertices[next] - vertices[i];
        Ogre::Vector3 norm = edge.crossProduct(Ogre::Vector3::UNIT_Z).normalisedCopy();

        obj->position(vertices[i]);     obj->colour(color * 0.7f); obj->normal(norm);
        obj->position(vertices[next]);   obj->colour(color * 0.7f); obj->normal(norm);
        obj->position(botVerts[next]);  obj->colour(color * 0.7f); obj->normal(norm);
        obj->position(botVerts[i]);     obj->colour(color * 0.7f); obj->normal(norm);

        obj->triangle(sideStart + i*4, sideStart + i*4 + 1, sideStart + i*4 + 2);
        obj->triangle(sideStart + i*4, sideStart + i*4 + 2, sideStart + i*4 + 3);
    }
}