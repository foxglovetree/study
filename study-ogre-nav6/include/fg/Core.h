#pragma once
#include <OgreInput.h>
#include <OgreApplicationContext.h>

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
};