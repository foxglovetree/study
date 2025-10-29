
#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <OgreRTShaderSystem.h>
#include <iostream>
using namespace Ogre;
using namespace OgreBites;
using namespace RTShader;
class BasicTutorial : public ApplicationContext, public InputListener
{

public:
	BasicTutorial();
	virtual ~BasicTutorial() {}
	void setup();
	bool keyPressed(const KeyboardEvent &e);
};

BasicTutorial::BasicTutorial() : ApplicationContext("StudyOgre1")
{
}

void BasicTutorial::setup()
{
	ApplicationContext::setup();
	addInputListener(this);
	Root *root = getRoot();
	SceneManager *sm = root->createSceneManager();
	ShaderGenerator *sg = ShaderGenerator::getSingletonPtr();
	sg->addSceneManager(sm);
	// light
	sm->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	Light *light = sm->createLight();
	SceneNode *lnode = sm->getRootSceneNode()->createChildSceneNode();
	lnode->attachObject(light);
	lnode->setPosition(20, 80, 50);
	//
	SceneNode *camNode = sm->getRootSceneNode()->createChildSceneNode();
	Camera *cam = sm->createCamera("MyCam");
	cam->setNearClipDistance(5);
	cam->setAutoAspectRatio(true);
	camNode->attachObject(cam);
	camNode->setPosition(0, 0, 140);
	getRenderWindow()->addViewport(cam);

	// entity
	Entity *e1 = sm->createEntity("ogrehead.mesh");
	SceneNode *node1 = sm->getRootSceneNode()->createChildSceneNode();
	node1->attachObject(e1);	
	camNode->setPosition(0, 47, 222);
	//
	Entity *e2 = sm->createEntity("ogrehead.mesh");
	SceneNode *node2 = sm->getRootSceneNode()->createChildSceneNode(Vector3(84, 48, 0));
	node2->attachObject(e2);	
	//
	Entity* ogreEntity3 = sm->createEntity("ogrehead.mesh");
    SceneNode* ogreNode3 = sm->getRootSceneNode()->createChildSceneNode();
    ogreNode3->setPosition(0, 104, 0);
    ogreNode3->setScale(2, 1.2, 1);
    ogreNode3->attachObject(ogreEntity3);
	//
	Entity* ogreEntity4 = sm->createEntity("ogrehead.mesh");
    SceneNode* ogreNode4 = sm->getRootSceneNode()->createChildSceneNode();
    ogreNode4->setPosition(-84, 48, 0);
    ogreNode4->roll(Degree(-90));
    ogreNode4->attachObject(ogreEntity4);

}
bool BasicTutorial::keyPressed(const KeyboardEvent &e)
{
	if (e.keysym.sym == SDLK_ESCAPE)
	{
		getRoot()->queueEndRendering();
	}
	return true;
}

int main()
{
	try
	{
		BasicTutorial app;
		app.initApp();
		app.getRoot()->startRendering();
		app.closeApp();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error:" << e.what() << "\n";
		return 1;
	}
	return 0;
}
