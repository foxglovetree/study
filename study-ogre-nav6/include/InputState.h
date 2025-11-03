#pragma once

struct InputState
{
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;

    bool isMoving()
    {
        return left || right || up || down;
    }
};