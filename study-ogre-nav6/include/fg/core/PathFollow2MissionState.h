#pragma
#include <Ogre.h>
#include <OgreFrameListener.h>
#include <OgreAnimationState.h>
#include <vector>
#include "fg/core/MissionState.h"
#include "fg/PathFollow2.h"

/**
 * Move a node to a destination.
 */
class PathFollow2MissionState : public MissionState, public FrameListener
{
    PathFollow2 *path;

    AnimationStateSet *aniSet;

public:
    PathFollow2MissionState(State *parent, PathFollow2 *path, AnimationStateSet *aniSet, std::vector<std::string> &aniNames) : MissionState(parent)
    {
        this->path = path;
        this->aniSet = aniSet;
        for (std::string name : aniNames)
        {
            AnimationState *as = this->aniSet->getAnimationState(name);
            as->setEnabled(true);
            as->setLoop(true);
            as->setWeight(1.0f);
        }
        //
        this->setFrameListener(this);
    }

    PathFollow2 *getPath()
    {
        return this->path;
    }

    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        SceneNode *pNode = this->findSceneNode(); // the parent node to operate

        if (pNode) // if the parent has no scene node attached,ignore the update operation.
        {
            PathFollow2 *pathFollow = this->getPath();

            Vector2 currentPos;
            Vector2 direction;
            if (pathFollow->move(evt.timeSinceLastFrame, currentPos, direction))
            {

                //
                Vector3 prevPos = pNode->getPosition();

                pNode->translate(currentPos.x - prevPos.x, 0, currentPos.y - prevPos.z); // new position
                // animation
                AnimationStateIterator it = this->aniSet->getAnimationStateIterator();
                while (it.hasMoreElements())
                {
                    AnimationState *as = it.getNext();
                    as->addTime(evt.timeSinceLastFrame);
                }

                float angle = atan2(-direction.y, direction.x) + Ogre::Math::HALF_PI; // 因为 next.y 对应 Z

                Quaternion orientation = Quaternion(Radian(angle), Vector3::UNIT_Y);
                pNode->setOrientation(orientation);
                // pNode->setOrientation(Quaternion(Degree(90), Vector3::UNIT_Y));
                //  update direction
                //
            }
            else
            {
                this->setDone(true);
            }
        }
        else
        {
            // todo add warning here if no scene node found.
        }
        return true;
    }
};