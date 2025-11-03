#pragma once
#include <iostream>
#include "HexNavigationGrid.h"

class HexGridPrinter
{
public:
    // === Print original cost grid ===
    static void printCostGrid(HexNavigationGrid &grid);
    // === Print path result grid ===
    static void printPathGrid(HexNavigationGrid &grid, int startx = -1, int starty = -1, int endx = -1, int endy = -1,
                              const std::vector<Ogre::Vector2> &path = {});
};