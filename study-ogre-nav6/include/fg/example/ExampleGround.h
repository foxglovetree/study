#pragma once

#include "fg/core/SimpleGround.h"

class ExampleGround : public SimpleGround
{
    CostMap *costMap;

public:
    ExampleGround(CostMap *costMap)
    {
        this->costMap = costMap;
        
        int width = costMap->getWidth();
        int height = costMap->getHeight();
        float rX = width * CostMap::hexSize * 2;
        float rZ = height * CostMap::hexSize * 2;
        
        polygon.add(0, 0);
        polygon.add(0, rZ);
        polygon.add(rX, rZ);
        polygon.add(rX, 0);
        
    }
    CostMap *getCostMap()
    {
        return costMap;
    }
};