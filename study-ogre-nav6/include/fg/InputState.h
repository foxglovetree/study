#pragma once

struct InputState
{
    bool left = false;
    bool right = false;
    bool front = false;
    bool back = false;
    bool up = false;
    bool down = false;

    bool isMoving()
    {
        return left || right || front || back;
    }
};