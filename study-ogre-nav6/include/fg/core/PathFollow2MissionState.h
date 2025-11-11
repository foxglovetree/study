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
    float heightOffset = 0.0f;

public:
    PathFollow2MissionState(PathFollow2 *path, AnimationStateSet *aniSet, std::vector<std::string> &aniNames, float heightOffset = 0.0f)
    {
        this->path = path;
        this->aniSet = aniSet;
        this->heightOffset = heightOffset;
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

            Vector2 currentPos2D;
            Vector2 direction2D;
            if (pathFollow->move(evt.timeSinceLastFrame, currentPos2D, direction2D))
            {

                //
                Vector3 prevPos = pNode->getPosition();
                Vector3 currentPos = Ground::Transfer::to3D(currentPos2D, heightOffset); //

                pNode->translate(currentPos - prevPos); // new position
                // animation
                AnimationStateIterator it = this->aniSet->getAnimationStateIterator();
                while (it.hasMoreElements())
                {
                    AnimationState *as = it.getNext();
                    as->addTime(evt.timeSinceLastFrame);
                }

                Quaternion orientation = Ground::getRotationTo(direction2D);
                pNode->setOrientation(orientation);

                // pNode->lookAt();

                // pNode->setOrientation(Quaternion(Degree(90), Vector3::UNIT_Y));
                //   update direction
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