#pragma once
#include "State.h"
#include "util/Polygon2.h"

class Ground
{

public:
    virtual bool isPointInside(float x, float z) = 0;
};