#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include "MaterialFactory.h"
#include "HexGridPrinter.h"
#include "InputState.h"
#include "CameraStateControl.h"
#include "MainInputListener.h"
#include "WorldStateControl.h"

using namespace OgreBites;
using namespace Ogre;
class HexGridApp : public ApplicationContext
{
private:
    

    CameraStateControl *frameListener;
    MainInputListener *keyHandler;
Ogre::SceneManager *sceneMgr;
    Ogre::RenderWindow *window;
    Ogre::Camera *camera;
    Ogre::SceneNode *cameraNode;

    Ogre::ManualObject *hexGridObject;

    Ogre::SceneNode *gridNode;
    Ogre::SceneNode *pathNode;

    Ogre::Viewport *vp;
    CostMap* costMap;
    WorldStateControl* wsc;
public:
    HexGridApp() : ApplicationContext("HexagonalGridVisualizer")
    {
    }
    void initApp()
    {
        ApplicationContextBase::initApp();
        // log level
        LogManager *lm = LogManager::getSingletonPtr();
        Log *log = lm->getDefaultLog();
        log->setDebugOutputEnabled(false);
        log->setLogDetail(Ogre::LL_LOW);
        //
        InputListener *ls = getImGuiInputListener();

        RenderWindow *window = this->getRenderWindow();

        Ogre::Root *root = getRoot();
        Ogre::SceneManager *sceneMgr = root->createSceneManager();

        // Register our scene with the RTSS (Required for proper lighting/shaders)
        Ogre::RTShader::ShaderGenerator *shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(sceneMgr);

        // Ogre::RTShader::RenderState* renderState = shadergen->getRenderState(Ogre::RTShader::RS_DEFAULT);
        // std::string techName = "VertexColourTech";
        // Ogre::Pass *pass=nullptr;

        // Create visualizer

        // Create navigation grid and set up example terrain

        // Sand: cost 2
        CostMap *costMap = new CostMap(12, 10);
        
        // 假设你已经有 sceneMgr 和 camera
        Ogre::Light *light = sceneMgr->createLight("MyPointLight");
        light->setType(Ogre::Light::LT_POINT);
        light->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));  // 白色漫反射
        light->setSpecularColour(Ogre::ColourValue(1.0, 1.0, 1.0)); // 白色镜面光

        Ogre::SceneNode *lightNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->setPosition(0, 500, 0);
        lightNode->attachObject(light);
        // Create camera
        camera = sceneMgr->createCamera("HexMapCamera");
        camera->setNearClipDistance(0.1f);
        camera->setFarClipDistance(1000.0f);
        camera->setAutoAspectRatio(true);

        // Create camera node and set position and direction
        cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        cameraNode->setPosition(0, 500, 500); //
        cameraNode->attachObject(camera);
        cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_PARENT);

        // Create viewport
        vp = window->addViewport(camera);
        vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

        // Create materials before buding mesh.   
        MaterialFactory::createMaterials();

        // Create world state and controls.
        wsc = new WorldStateControl(root, costMap, sceneMgr, camera);


        // Add input listener
        keyHandler = new MainInputListener(wsc, window, vp, camera);
        addInputListener(keyHandler);
        
    }

    void findPath()
    {
    }
    void startRendering()
    {
        mRoot->startRendering();
    }
};