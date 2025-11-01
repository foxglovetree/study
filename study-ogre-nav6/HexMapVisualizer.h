#pragma once

#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreLight.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreVector2.h>
#include <OgreColourValue.h>
#include <vector>
#include <unordered_set>
#define M_PI 3.1415926
struct PairHash {
    size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

// 简化版六边形导航网格
class HexNavigationGrid {
public:
    static const int OBSTACLE = -1;
    static const int DEFAULT_COST = 1;

private:
    int width, height;
    std::vector<std::vector<int>> grid;

public:
    HexNavigationGrid(int w, int h) : width(w), height(h), grid(h, std::vector<int>(w, DEFAULT_COST)) {
        // 添加一些障碍物和地形
        for (int y = 5; y < 8; ++y)
            for (int x = 5; x < 10; ++x)
                grid[y][x] = OBSTACLE;

        for (int y = 12; y < 15; ++y)
            for (int x = 3; x < 7; ++x)
                grid[y][x] = 3; // Water

        for (int y = 2; y < 4; ++y)
            for (int x = 8; x < 12; ++x)
                grid[y][x] = 2; // Sand
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getCost(int x, int y) const {
        if (x < 0 || y < 0 || x >= width || y >= height) return OBSTACLE;
        return grid[y][x];
    }

    // 获取六边形顶点（中心在 (cx,cy)）
    std::vector<Ogre::Vector3> getHexagonVertices(int x, int y, float size) const {
        float cx = x * 1.5f * size;
        float cy = y * size * sqrt(3.0f) + (x % 2 ? size * sqrt(3.0f)/2 : 0);

        std::vector<Ogre::Vector3> vertices;
        for (int i = 0; i < 6; ++i) {
            float angle = M_PI / 3.0f * i;
            vertices.emplace_back(cx + size * cos(angle),
                                  cy + size * sin(angle),
                                  0.0f);
        }
        return vertices;
    }
};

class HexMapVisualizer {
private:
    Ogre::SceneManager* sceneMgr;
    Ogre::RenderWindow* window;
    Ogre::Camera* camera;
    Ogre::SceneNode* cameraNode;
    Ogre::ManualObject* hexGridObject;
    Ogre::ManualObject* pathObject;
    Ogre::SceneNode* gridNode;
    Ogre::SceneNode* pathNode;
    float hexSize;

    const HexNavigationGrid* currentGrid;
    std::vector<Ogre::Vector2> currentPath;
    int startx, starty, endx, endy;
    bool gridDirty, pathDirty;

    Ogre::Light* mainLight;
    Ogre::SceneNode* lightNode;

public:
    HexMapVisualizer(Ogre::SceneManager* mgr, Ogre::RenderWindow* win, float size = 30.0f);
    ~HexMapVisualizer();

    Ogre::Camera* getCamera() { return camera; }
    void setGrid(const HexNavigationGrid& grid);
    void setPath(const std::vector<Ogre::Vector2>& path, int sx, int sy, int ex, int ey);
    void update();

private:
    void createLights();
    void drawHexGrid();
    void drawPath();
    Ogre::ColourValue getCostColor(int cost) const;
    float getHeightForCell(int x, int y) const;
    void drawHexPrismTo(Ogre::ManualObject* obj,
                        const std::vector<Ogre::Vector3>& vertices,
                        const Ogre::ColourValue& color,
                        float height);
};