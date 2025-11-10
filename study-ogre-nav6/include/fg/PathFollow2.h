#pragma once

#include <vector>
#include <Ogre.h>
#include "util/Polyline2.h"

using namespace Ogre;

class PathFollow2
{
    std::vector<Ogre::Vector2> path;
    float speed = 30.0f;
    int next = 0;
    Vector2 position;

public:
    PathFollow2(Vector2 position, std::vector<Ogre::Vector2> path)
    {
        this->position = position;
        this->path = path;
    }

    bool move(float timeEscape, Vector2 &currentPos, Vector2 &direction)
    {
        bool rt = false;
        while (next < path.size())
        {
            Vector2 nextPos = path[next];//next position
            direction = nextPos - position;//direction

            float distance = direction.length();
            if (distance < 0.01f)
            {
                next++;
                continue;
            }
            //found the right position
            direction.normalise();
            float move = speed * timeEscape;
            if (move > distance)
            {
                move = distance;
            }

            position += direction * move;
            currentPos = this->position;
            rt = true;
            break;
        }
        return rt;
    }
};