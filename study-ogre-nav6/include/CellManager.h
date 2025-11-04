#pragma once
#include <vector>
#include "CostMap.h"

class CellManager
{
private:
    
    int width;
    int height;
    CostMap costMap;

public:
    CellManager(int width, int height) : width(width), height(height), costMap(width, height)
    {        
        for (int i = 3; i < 8; i++)
        {
            costMap.setCost(i, 4, 2);
        }

        // Water: cost 3
        for (int i = 2; i < 6; i++)
        {
            costMap.setCost(6, i, 3);
        }

        // Low cost path (like road)
        for (int i = 2; i < 10; i++)
        {
            costMap.setCost(i, 7, 1);
        }

        // Obstacles
        costMap.setCost(4, 3, CostMap::OBSTACLE);
        costMap.setCost(7, 5, CostMap::OBSTACLE);
    }


    CostMap &getCostMap()
    {
        return this->costMap;
    }

    int getWidth()
    {
        return width;
    }
    int getHeight()
    {
        return height;
    }

};