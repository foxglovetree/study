
#pragma once
#include <vector>
#include <Ogre.h>
#include "CostMap.h"

class CellUtil
{
public:
    static void translatePathToCellCenter(std::vector<Vector2> &pathByKey, std::vector<Vector2> &pathByPosition)
    {
        for (int i = 0; i < pathByKey.size(); i++)
        {
            auto p = pathByKey[i];
            auto center = CostMap::calculateCenterForXZ(static_cast<int>(p.x), static_cast<int>(p.y), CostMap::hexSize);
            pathByPosition[i] = Vector2(center.x, center.z);
        }
    }

    static bool findCellByPoint(CostMap *costMap, Vector2 point, CellKey &cKey)
    {
        return findCellByPoint(costMap, point.x, point.y, cKey.first, cKey.second);
    }

    static bool findCellByPoint(CostMap *costMap, float px, float py, int &cx, int &cy)
    {

        int width = costMap->getWidth();
        int height = costMap->getHeight();
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (isPointInCell(px, py, x, y))
                {
                    cx = x;
                    cy = y;
                    return true;
                }
            }
        }
        return false;
    }
    static bool isPointInCell(float px, float py, int cx, int cy)
    {
        auto corners = CostMap::calculateVerticesForXZ(cx, cy, CostMap::hexSize);

        // 叉积判断是否在所有边的左侧
        for (int i = 0; i < 6; ++i)
        {
            // 逆时针
            auto p1 = corners[i];
            auto p2 = corners[(i + 1) % 6];

            // 向量 edge = p2 - p1
            // 向量 point = (mx, my) - p1
            float cross = (px - p1.x) * (p2.y - p1.y) - (py - p1.y) * (p2.x - p1.x);
            if (cross < 0)
                return false;
        }
        return true;
    }
};