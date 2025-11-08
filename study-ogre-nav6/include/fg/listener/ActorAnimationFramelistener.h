
#pragma once
#include <Ogre.h>

class ActorAnimationFramelistener : public Ogre::FrameListener
{
    Ogre::AnimationStateSet *animSet;
    

public:
    ActorAnimationFramelistener(Ogre::Entity *entity)
    {
        animSet = entity->getAllAnimationStates();
    }

    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        Ogre::AnimationState *walkAnim = animSet->getAnimationState("Walk");
        walkAnim->setEnabled(true);
        walkAnim->setLoop(true);   // 循环播放
        walkAnim->setWeight(1.0f); // 混合权重（用于多动画混合）
        return true;
    }
};
