
#pragma once
#include <vector>
#include <Ogre.h>

class TargetCellListener
{
    
public:
    virtual bool onTargetCell(){
        return false;
    }
};