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
    class Transfer
    {
    public:
        static const bool VERTEX_ORDER_REVERSE = false; // is look from back

        static Vector3 to3D(Vector2 &vec2, float height = 0.0f)
        {
            if (VERTEX_ORDER_REVERSE)
            {
                return Vector3(vec2.y, height, -vec2.x);
            }
            else
            {
                return Vector3(vec2.x, height, -vec2.y);
            }
        }

        static Vector2 to2D(const Vector3 &vec3, float &height)
        {
            height = vec3.y;
            return to2D(vec3);
        }
        static Vector2 to2D(const Vector3 &vec3)
        {
            if (VERTEX_ORDER_REVERSE)
            {

                return Vector2(-vec3.z, vec3.x);
            }
            else
            {

                return Vector2(vec3.x, -vec3.z);
            }
        }

        static void to3D(Vector2 &vec2, Vector3 &vec3, float height = 0.0f)
        {
            vec3 = Transfer::to3D(vec2, height);
        }

        static std::vector<Ogre::Vector3> to3D(std::vector<Ogre::Vector2> &vec2Vec, float height = 0.0f)
        {
            std::vector<Ogre::Vector3> vertices(vec2Vec.size());

            for (int i = 0; i < vec2Vec.size(); i++)
            {
                to3D(vec2Vec[VERTEX_ORDER_REVERSE ? (6 - i - 1) : i], vertices[i], height);
            }

            return vertices;
        }
    };

public:
    static inline const Vector3 DEFAULT_FORWARD = Ogre::Vector3::UNIT_Z;

    static Quaternion getRotationTo(Vector2 &direction)
    {
        Vector3 d3 = Transfer::to3D(direction);
        return DEFAULT_FORWARD.getRotationTo(d3);
    }

    static std::vector<Ogre::Vector3> calculateVertices3D(int x, int y, float rad, float scale = 1.0f)
    {
        std::vector<Ogre::Vector2> vec2Vec = calculateVertices(x, y, rad, scale);
        return Transfer::to3D(vec2Vec);
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

            vertices[i] = Ogre::Vector2(center.x + dx, center.y + dy);
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