#pragma once
#include <vector>
#include "CostMap.h"
struct CellIndex
{
    int x;
    int y;
};

struct CellState
{
public:
    bool selected = false;
};

class CellManager
{
private:
    static constexpr  CellState DEFAULT_STATE{};
    
    std::vector<std::vector<CellState>> cellStates;
    int width;
    int height;
    CostMap costMap;

public:
    CellManager(int width, int height) : width(width), height(height), costMap(width, height)
    {
        cellStates.resize(height, std::vector<CellState>(width, DEFAULT_STATE));
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

    void setSelected(int x, int y, bool selected)
    {
        cellStates[x][y].selected = selected;
    }

    bool getSelected(int x, int y)
    {
        return cellStates[x][y].selected;
    }
};