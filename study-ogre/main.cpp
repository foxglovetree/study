#include <iostream>
#include <string>
#include <fmt/format.h>
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>

int main()
{
	std::string name = "World";
	fmt::println("Hello {}", name);
	OgreBites::ApplicationContext ctx("StudyOgre");
	fmt::println("Initializing app.");
	try
	{

		ctx.initApp();
		fmt::println("App initialized.");

		Ogre::Root *root = ctx.getRoot();
		Ogre::SceneManager *scnMgr = root->createSceneManager();

		// register our scene with the RTSS
		Ogre::RTShader::ShaderGenerator *shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
		shadergen->addSceneManager(scnMgr);

		// without light we would just get a black screen
		Ogre::Light *light = scnMgr->createLight("MainLight");
		Ogre::SceneNode *lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
		lightNode->setPosition(0, 10, 15);
		lightNode->attachObject(light);

		// also need to tell where we are
		Ogre::SceneNode *camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
		camNode->setPosition(0, 0, 15);
		camNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_PARENT);

		// create the camera
		Ogre::Camera *cam = scnMgr->createCamera("myCam");
		cam->setNearClipDistance(5); // specific to this sample
		cam->setAutoAspectRatio(true);
		camNode->attachObject(cam);

		// and tell it to render into the main window
		ctx.getRenderWindow()->addViewport(cam);

		// finally something to render
		/*
		*/
		Ogre::Entity *ent = scnMgr->createEntity("Sinbad.mesh");
		Ogre::SceneNode *node = scnMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(ent);
		
		ctx.getRoot()->startRendering();
		fmt::println("Closing.");
    	ctx.closeApp();

	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception catched:\n"
				  << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Exception catched:\n"
				  << "Unknown error." << std::endl;
	}

	return 0;
}
