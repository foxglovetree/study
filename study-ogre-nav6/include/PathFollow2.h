#pragma once

#include <vector>
#include <Ogre.h>
#include "Polyline2.h"

using namespace Ogre;

class PathFollow2
{
    std::vector<Ogre::Vector2> path;
    float speed = 1.0f;
    int target = 0;
    Vector2 position;

public:
    bool move(float timeEscape, Vector2 &position)
    {
        bool rt = true;
        while (true)
        {
            if (target >= path.size())
            {
                rt = false;
                break;
            }

            Vector2 targetPoint = path[target];
            Vector2 direction = targetPoint - this->position;
            float distance = direction.length();
            if (distance < 0.1f)
            {
                continue;
            }
            direction.normalise();
            float move = speed * timeEscape;
            if (move > distance)
            {
                move = distance;
            }
            this->position += direction * move;
        }
        position = this->position;
        return rt;
    }
};