#pragma once
#include "fg/Ground.h"
#include "fg/util/Polygon2.h"

class SimpleGround : public Ground
{
protected:
    Polygon2 polygon;

public:
    bool isPointInside(Vector2 &p) override
    {
        return isPointInside(p.x, p.y);
    }
    bool isPointInside(float x, float y) override
    {
        return polygon.isPointInPolygon(x, y);
    }
};