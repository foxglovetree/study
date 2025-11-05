
#pragma once
#include <vector>
#include <Ogre.h>
#include "StateControl.h"
#include "Sprite.h"

using namespace Ogre;

class SpriteStateControl : SimpleStateControl
{

public:
    SpriteStateControl(Ogre::SceneManager *sceneMgr) : SimpleStateControl(sceneMgr)
    {
    }

    void buildMeshInternal(ManualObject *obj) override
    {
    }
};
