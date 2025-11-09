#pragma once
#include "fg/Ground.h"
#include "fg/util/Polygon2.h"

class SimpleGround : public Ground
{
protected:
    Polygon2 polygon;

public:
    bool isPointInside(float x, float z) override
    {
        return polygon.isPointInPolygon(x, z);
    }
};