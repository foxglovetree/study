
#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include <OgreRTShaderSystem.h>
#include <iostream>
using namespace Ogre;
using namespace OgreBites;
using namespace RTShader;
class BasicTutorial2 : public ApplicationContext, public InputListener
{

public:
	BasicTutorial2();
	virtual ~BasicTutorial2() {}
	void setup();
	bool keyPressed(const KeyboardEvent &e);
};

BasicTutorial2::BasicTutorial2() : ApplicationContext("StudyOgre2")
{
}

void BasicTutorial2::setup()
{
	ApplicationContext::setup();
	addInputListener(this);
	Root *root = getRoot();
	SceneManager *sm = root->createSceneManager();
	RTShader::ShaderGenerator * sg = RTShader::ShaderGenerator::getSingletonPtr();
	sg->addSceneManager(sm);

	// light
	sm->setAmbientLight(ColourValue(0, 0, 0));
	sm->setShadowTechnique(ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
	Light *light = sm->createLight("SpotLight");
	light->setDiffuseColour(0, 0, 1.0);
	light->setSpecularColour(0, 0, 1.0);
	light->setType(Light::LT_SPOTLIGHT);
	light->setSpotlightRange(Degree(35), Degree(50));
	SceneNode *lnode = sm->getRootSceneNode()->createChildSceneNode();
	lnode->attachObject(light);
	lnode->setPosition(200, 200, 0);
	lnode->setDirection(-1, -1, 0);

	// Camera
	SceneNode *camNode = sm->getRootSceneNode()->createChildSceneNode();
	Camera *cam = sm->createCamera("MyCam");
	camNode->setPosition(200, 300, 400);
	camNode->lookAt(Vector3(0, 0, 0), Node::TransformSpace::TS_WORLD);
	cam->setNearClipDistance(5);
	camNode->attachObject(cam);
	// Viewport
	Viewport *vp = getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(ColourValue(0, 0, 0));
	cam->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

	// entity
	Entity *e1 = sm->createEntity("ninja.mesh");
	e1->setCastShadows(true);
	SceneNode *node1 = sm->getRootSceneNode()->createChildSceneNode();
	node1->attachObject(e1);
	// ground
	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane("ground", RGN_DEFAULT, plane, 1500, 1500, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);
	Entity *e2 = sm->createEntity("ground");
	e2->setCastShadows(false);
	e2->setMaterialName("Rockwall");
	SceneNode *node2 = sm->getRootSceneNode()->createChildSceneNode();
	node2->attachObject(e2);
	/*
	*/
}
bool BasicTutorial2::keyPressed(const KeyboardEvent &e)
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
		BasicTutorial2 app;
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
