#pragma once
#include "util/CellMark.h"
class Movable
{
public:
    virtual bool setTargetCell(CellKey &cKey2) = 0;
};