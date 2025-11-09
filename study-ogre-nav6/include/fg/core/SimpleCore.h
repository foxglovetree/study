#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>

#include "fg/util/HexGridPrinter.h"
#include "fg/CostMapControl.h"
#include "fg/Module.h"
#include <unordered_map>
using namespace OgreBites;
using namespace Ogre;
class SimpleCore : public Core
{
private:
    Ogre::Camera *camera;
    Ogre::SceneNode *cameraNode;
    Ogre::Viewport *vp;
    ApplicationContext *appCtx;
    Ogre::SceneManager *sceMgr;
    Ogre::Root *root;
    std::unordered_map<std::string, std::any> userObjs;
    MaterialManager *matMgr;

public:
    SimpleCore()
    {

        appCtx = new ApplicationContext("HexagonalGridVisualizer");

        appCtx->initApp();
        this->matMgr = MaterialManager::getSingletonPtr();
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

        // Create world state and controls.
    }

    ApplicationContext *getAppContext() { return this->appCtx; }
    SceneManager *getSceneManager() { return this->sceMgr; }
    Viewport *getViewport() { return this->vp; }
    Camera *getCamera() { return this->camera; }
    Root *getRoot() { return this->root; };
    RenderWindow *getWindow()
    {
        return this->appCtx->getRenderWindow();
    }
    MaterialManager *getMaterialManager() override
    {
        return this -> matMgr;
    }

    void addInputListener(InputListener *listener) override
    {
        this->appCtx->addInputListener(listener);
    }
    void addFrameListener(FrameListener *listener) override
    {

        this->root->addFrameListener(listener);
    }

    void setUserObject(const std::string key, std::any value) override
    {
        this->userObjs[key] = value;
    }

    bool getUserObject(const std::string key, std::any &value) override
    {
        std::unordered_map<std::string, std::any>::iterator it = userObjs.find(key);
        if (it != userObjs.end())
        {
            value = it->second;
            return true;
        }
        return false;
    }
};