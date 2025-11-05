
#pragma once
#include <vector>
#include <Ogre.h>

class CameraUtil
{
public:
    static bool getViewportOnPlane(Ogre::Plane &plane, Ogre::Camera *cam, std::vector<Ogre::Vector3> &points)
    {

        // (0,0), (1,0), (1,1), (0,1)
        float uvs[4][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
        bool rt = true;
        for (int i = 0; i < 4; ++i)
        {
            Ogre::Ray ray = cam->getCameraToViewportRay(uvs[i][0], uvs[i][1]);

            std::pair<bool, Ogre::Real> result = ray.intersects(plane);
            if (result.first && result.second > 0)
            {
                Ogre::Vector3 hit = ray.getPoint(result.second);
                points.push_back(hit);
            }
            else
            {
                rt = false;
                break;
            }
        }
        return rt;
    }
}