
#pragma once

#include <Ogre.h>
#include <OgreColourValue.h>
using namespace Ogre;
class DrawerUtil
{
public:
    static void drawHexagonTo(Ogre::ManualObject *obj,
                              const std::vector<Ogre::Vector3> &vertices,
                              const Ogre::ColourValue &color1)
    {
        drawHexagonTo(obj, vertices, color1, color1);
    }

    static void drawHexagonTo(Ogre::ManualObject *obj,
                              const std::vector<Ogre::Vector3> &vertices,
                              const Ogre::ColourValue &color1, ColourValue color2)
    {
        const float nomX = 0;
        const float nomY = 1;
        const float nomZ = 0;
        // Compute center
        Ogre::Vector3 center(0,0, 0);
        for (auto &v : vertices)
            center += v;
        center *= (1.0f / 6.0f);

        size_t baseIndex = obj->getCurrentVertexCount();

        // Center
        obj->position(center.x, center.y, center.z);
        obj->normal(nomX, nomY, nomZ);
        obj->colour(color1);

        // Corners
        for (int i = 0; i < 6; ++i)
        {
            obj->position(vertices[i].x, vertices[i].y, vertices[i].z);
            obj->normal(nomX, nomY, nomZ);
            obj->colour(color2);
        }

        // Triangles
        for (int i = 0; i < 6; ++i)
        {
            int p1 = baseIndex + i + 1;
            int p2 = baseIndex + (i + 1) % 6 + 1;
            obj->triangle(baseIndex, p1, p2);
        }
    } //
};