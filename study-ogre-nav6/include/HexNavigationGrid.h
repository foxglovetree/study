// main.cpp - Complete Ogre A* Hex Grid Visualization System
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

// === Include OgreBites for modern initialization ===
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>

struct PairHash
{
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &p) const
    {
        auto h1 = std::hash<T>{}(p.first);
        auto h2 = std::hash<U>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

// === NavNode structure ===
struct NavNode
{
    int x, y;
    float g, h;
    float f() const { return g + h; }
    bool operator>(const NavNode &other) const { return f() > other.f(); }
};


class HexNavigationGrid
{
private:


// === Fixed hexagon neighbor offsets (flat-top) ===
int dx_even[6] = {+1, 0, -1, -1, -1, 0};
int dy_even[6] = {0, -1, -1, 0, +1, +1};
int dx_odd[6] = {+1, +1, 0, -1, 0, +1};
int dy_odd[6] = {0, -1, -1, 0, +1, +1};

public:
std::vector<std::vector<int>> costGrid;
int width, height;

public:
    static const int OBSTACLE = 0;
    static const int DEFAULT_COST = 1;

    HexNavigationGrid(int w, int h) : width(w), height(h)
    {
        costGrid.resize(height, std::vector<int>(width, DEFAULT_COST));
    }

    void setCost(int x, int y, int cost)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            costGrid[y][x] = cost;
        }
    }

    int getCost(int x, int y) const
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return OBSTACLE;
        return costGrid[y][x];
    }

    bool isWalkable(int x, int y) const
    {
        return getCost(x, y) > 0;
    }

    std::pair<int, int> getNeighbor(int x, int y, int direction) const
    {
        if (direction < 0 || direction >= 6)
            return {x, y};
        if (y % 2 == 0)
        {
            return {x + dx_even[direction], y + dy_even[direction]};
        }
        else
        {
            return {x + dx_odd[direction], y + dy_odd[direction]};
        }
    }

    float heuristic(int x1, int y1, int x2, int y2) const
    {
        int q1 = x1 - (y1 - (y1 & 1)) / 2;
        int r1 = y1;
        int s1 = -q1 - r1;

        int q2 = x2 - (y2 - (y2 & 1)) / 2;
        int r2 = y2;
        int s2 = -q2 - r2;

        int dq = abs(q1 - q2);
        int dr = abs(r1 - r2);
        int ds = abs(s1 - s2);

        return static_cast<float>(std::max({dq, dr, ds}) * DEFAULT_COST);
    }

    std::vector<Ogre::Vector2> findPath(int startX, int startY, int endX, int endY)
    {
        using Pos = std::pair<int, int>;

        if (!isWalkable(startX, startY) || !isWalkable(endX, endY))
        {
            return {};
        }

        std::priority_queue<NavNode, std::vector<NavNode>, std::greater<NavNode>> openList;
        std::unordered_map<Pos, Pos, PairHash> cameFrom;
        std::unordered_map<Pos, float, PairHash> gScore;
        std::unordered_set<Pos, PairHash> closed;

        NavNode start = {startX, startY, 0, heuristic(startX, startY, endX, endY)};
        openList.push(start);
        gScore[{startX, startY}] = 0;

        while (!openList.empty())
        {
            NavNode current = openList.top();
            openList.pop();
            Pos currPos = {current.x, current.y};

            if (closed.find(currPos) != closed.end())
                continue;
            closed.insert(currPos);

            if (current.x == endX && current.y == endY)
            {
                return reconstructPath(cameFrom, currPos);
            }

            for (int i = 0; i < 6; i++)
            {
                auto [nx, ny] = getNeighbor(current.x, current.y, i);
                if (!isWalkable(nx, ny))
                    continue;

                Pos neighbor = {nx, ny};
                int moveCost = getCost(nx, ny);
                float tentativeG = gScore[currPos] + moveCost;

                auto it = gScore.find(neighbor);
                if (it == gScore.end() || tentativeG < it->second)
                {
                    cameFrom[neighbor] = currPos;
                    gScore[neighbor] = tentativeG;
                    float h = heuristic(nx, ny, endX, endY);

                    NavNode node = {nx, ny, tentativeG, h};
                    openList.push(node);
                }
            }
        }

        return {};
    }

private:
    std::vector<Ogre::Vector2> reconstructPath(
        const std::unordered_map<std::pair<int, int>, std::pair<int, int>, PairHash> &cameFrom,
        const std::pair<int, int> &current) const
    {

        std::vector<Ogre::Vector2> path;
        std::pair<int, int> node = current;

        while (cameFrom.find(node) != cameFrom.end())
        {
            path.push_back(Ogre::Vector2(static_cast<float>(node.first), static_cast<float>(node.second)));
            node = cameFrom.at(node);
        }
        path.push_back(Ogre::Vector2(static_cast<float>(node.first), static_cast<float>(node.second)));

        std::reverse(path.begin(), path.end());
        return path;
    }

public:
    

    float calculatePathCost(const std::vector<Ogre::Vector2> &path) const
    {
        float totalCost = 0;
        for (size_t i = 1; i < path.size(); i++)
        {
            int x = static_cast<int>(path[i].x);
            int y = static_cast<int>(path[i].y);
            totalCost += getCost(x, y);
        }
        return totalCost;
    }

    // === Data interface for Ogre rendering ===
    const std::vector<std::vector<int>> &getCostGrid() const { return costGrid; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Get hexagon vertices (for Ogre rendering)
    std::vector<Ogre::Vector3> getHexagonVertices(int x, int y, float size) const
    {
        std::vector<Ogre::Vector3> vertices(6);
        float centerX = x * size * 1.5f;
        float centerY = y * size * std::sqrt(3.0f);
        if (y % 2 == 1)
            centerX += size * 0.75f;

        for (int i = 0; i < 6; i++)
        {
            float angle_rad = (60.0f * i + 30.0f) * 3.1415926f / 180.0f;
            float dx = size * std::cos(angle_rad);
            float dy = size * std::sin(angle_rad);
            vertices[i] = Ogre::Vector3(centerX + dx, centerY + dy, 0.0f);
        }

        return vertices;
    }
};