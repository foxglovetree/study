#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include "fg/core/MaterialFactory.h"
#include "fg/util/HexGridPrinter.h"
#include "CameraStateControl.h"
#include "WorldStateControl.h"
#include "fg/CostMapControl.h"
#include "fg/Module.h"

using namespace OgreBites;
using namespace Ogre;
class SimpleCore : public Core
{
private:
    CameraStateControl *frameListener;
    Ogre::Camera *camera;
    Ogre::SceneNode *cameraNode;

    Ogre::ManualObject *hexGridObject;

    Ogre::SceneNode *gridNode;
    Ogre::SceneNode *pathNode;

    Ogre::Viewport *vp;
    CostMap *costMap;
    ApplicationContext *appCtx;
    Ogre::SceneManager *sceMgr;
    Ogre::Root * root;

public:
    SimpleCore()
    {

        appCtx = new ApplicationContext("HexagonalGridVisualizer");

        appCtx->initApp();
        this->root = appCtx->getRoot();

        // log level
        LogManager *lm = LogManager::getSingletonPtr();
        Log *log = lm->getDefaultLog();
        log->setDebugOutputEnabled(false);
        log->setLogDetail(Ogre::LL_LOW);
        //
        InputListener *ls = appCtx->getImGuiInputListener();

        RenderWindow *window = appCtx->getRenderWindow();

        sceMgr = appCtx->getRoot()->createSceneManager();

        // Register our scene with the RTSS (Required for proper lighting/shaders)
        Ogre::RTShader::ShaderGenerator *shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(sceMgr);

        // Ogre::RTShader::RenderState* renderState = shadergen->getRenderState(Ogre::RTShader::RS_DEFAULT);
        // std::string techName = "VertexColourTech";
        // Ogre::Pass *pass=nullptr;

        // Create visualizer

        // Create navigation grid and set up example terrain

        // Sand: cost 2

        // 假设你已经有 sceneMgr 和 camera
        Ogre::Light *light = sceMgr->createLight("MyPointLight");
        light->setType(Ogre::Light::LT_POINT);
        light->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));  // 白色漫反射
        light->setSpecularColour(Ogre::ColourValue(1.0, 1.0, 1.0)); // 白色镜面光

        Ogre::SceneNode *lightNode = sceMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->setPosition(0, 500, 0);
        lightNode->attachObject(light);
        // Create camera
        camera = sceMgr->createCamera("HexMapCamera");
        camera->setNearClipDistance(0.1f);
        camera->setFarClipDistance(1000.0f);
        camera->setAutoAspectRatio(true);

        // Create camera node and set position and direction
        cameraNode = sceMgr->getRootSceneNode()->createChildSceneNode();
        cameraNode->setPosition(0, 500, 500); //
        cameraNode->attachObject(camera);
        cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_PARENT);

        // Create viewport
        vp = window->addViewport(camera);
        vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

        // Create materials before buding mesh.
        MaterialFactory::createMaterials();

        // Create world state and controls.
    }

    ApplicationContext *getAppContext() { return this->appCtx; }
    SceneManager *getSceneManager() { return this->sceMgr; }
    Viewport *getViewport() { return this->vp; }
    Camera *getCamera() { return this->camera; }
    Root * getRoot(){return this->root;};
    RenderWindow * getWindow(){
        return this->appCtx->getRenderWindow();
    }

    void addInputListener(InputListener *listener) override
    {
        this->appCtx->addInputListener(listener);
    }
    void addFrameListener(FrameListener *listener) override
    {

        this->root->addFrameListener(listener);
    }
};