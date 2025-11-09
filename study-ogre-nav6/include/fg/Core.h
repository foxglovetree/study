#pragma once
#include <OgreInput.h>
#include <OgreApplicationContext.h>
#include <any>

using namespace Ogre;
using namespace OgreBites;

class Core
{
public:
    virtual ApplicationContext *getAppContext() = 0;
    virtual SceneManager *getSceneManager() = 0;
    virtual Viewport *getViewport() = 0;
    virtual Camera *getCamera() = 0;
    virtual Root *getRoot() = 0;
    virtual RenderWindow *getWindow() = 0;
    virtual void addInputListener(InputListener *listener) = 0;
    virtual void addFrameListener(Ogre::FrameListener *listener) = 0;
    virtual MaterialManager *getMaterialManager() = 0;

    virtual void setUserObject(const std::string key, std::any value) = 0;
    virtual bool getUserObject(const std::string key, std::any &value) = 0;

    template <typename T>
    void setUserObject(const std::string key, T *obj)
    {
        this->setUserObject(key, std::any(obj));
    }

    template <typename T>
    T *getUserObject(const std::string key)
    {

        return getUserObject<T>(key, false);
    }

    template <typename T>
    T *getUserObject(const std::string key, bool required)
    {
        std::any value;
        T *rt = nullptr;
        if (this->getUserObject(key, value))
        {
            T **pp = any_cast<T *>(&value);
            if (pp != nullptr)
            {
                rt = *pp;
            }
        }

        if (rt == nullptr && required)
        {
            throw std::runtime_error("No such user object with key=" + key + " or type of the value mis-match.");
        }
        return rt;
    }
};