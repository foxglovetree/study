#pragma once
#include "State.h"
#include "util/Polygon2.h"
#include <Ogre.h>
#include <OgreVector2.h>
#include <OgreVector3.h>
using namespace Ogre;

class Ground
{
public:
    static std::vector<Ogre::Vector3> to3D(std::vector<Ogre::Vector2> &vec2Vec)
    {
        std::vector<Ogre::Vector3> vertices(vec2Vec.size());

        for (int i = 0; i < vec2Vec.size(); i++)
        {
            to3D(vec2Vec[i], vertices[i]);
        }

        return vertices;
    }
    static Vector2 to2D(const Vector3 &vec3)
    {
        return Vector2(-vec3.z, vec3.x);
    }

    static Vector3 to3D(Vector2 &vec2)
    {
        return Vector3(vec2.y, 0, -vec2.x);
    }

    static void to3D(Vector2 &vec2, Vector3 &vec3)
    {
        vec3 = to3D(vec2);
    }

    static std::vector<Ogre::Vector3> calculateVertices3D(int x, int y, float rad, float scale = 1.0f)
    {
        std::vector<Ogre::Vector2> vec2Vec = calculateVertices(x, y, rad, scale);
        return to3D(vec2Vec);
    }

    // Get hexagon vertices
    // anti-clockwise
    static std::vector<Ogre::Vector2> calculateVertices(float rad, float scale = 1.0f)
    {
        return calculateVertices(0, 0, rad, scale);
    }

    static std::vector<Ogre::Vector2> calculateVertices(int x, int y, float rad, float scale = 1.0f)
    {
        std::vector<Ogre::Vector2> vertices(6);

        Ogre::Vector2 center = calculateCenter(x, y, rad);

        float RAD = scale * 2 * rad / std::sqrt(3.0f);

        for (int i = 0; i < 6; i++)
        {
            float angle_rad = (60.0f * i + 30.0f) * Ogre::Math::PI / 180.0f;
            float dx = RAD * std::cos(angle_rad);
            float dy = RAD * std::sin(angle_rad);

            vertices[6 - i - 1] = Ogre::Vector2(center.x + dx, center.y + dy);
        }

        return vertices;
    }

    static Ogre::Vector2 calculateCenter(int x, int y, float rad = CostMap::hexSize)
    {
        float centerX = x * 2 * rad + (y % 2 == 0 ? 0 : rad);
        float centerY = y * rad * std::sqrt(3.0f);
        return Ogre::Vector2(centerX, centerY);
    }

public:
    virtual bool isPointInside(float x, float z) = 0;
    virtual bool isPointInside(Vector2 &p) = 0;

};