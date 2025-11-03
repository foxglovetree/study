#pragma once
#include <iostream>
#include "CostMap.h"

class HexGridPrinter
{
public:
    // === Print original cost grid ===
    static void printCostGrid(CostMap &grid);
    // === Print path result grid ===
    static void printPathGrid(CostMap &grid, int startx = -1, int starty = -1, int endx = -1, int endy = -1,
                              const std::vector<Ogre::Vector2> &path = {});
};