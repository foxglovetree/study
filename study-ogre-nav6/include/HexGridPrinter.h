#pragma once
#include <iostream>
#include "GridManager.h"

class HexGridPrinter
{
public:
    // === Print original cost grid ===
    static void printCostGrid(GridManager &grid);
    // === Print path result grid ===
    static void printPathGrid(GridManager &grid, int startx = -1, int starty = -1, int endx = -1, int endy = -1,
                              const std::vector<Ogre::Vector2> &path = {});
};