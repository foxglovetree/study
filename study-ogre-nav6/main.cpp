#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreInput.h>
#include "HexMapVisualizer.h"
using namespace Ogre;
using namespace OgreBites;

class App : public OgreBites::ApplicationContext, public OgreBites::InputListener {
public:
    HexMapVisualizer* visualizer;

    App() : ApplicationContext("HexMapDemo") {}

    void setup() override {
        ApplicationContext::setup();
        addInputListener(this);

        Ogre::Root* root = getRoot();
        Ogre::SceneManager* sm = root->createSceneManager();
        RTShader::ShaderGenerator* sg = RTShader::ShaderGenerator::getSingletonPtr();
        sg->addSceneManager(sm);

        HexNavigationGrid grid(15, 15);

        visualizer = new HexMapVisualizer(sm, getRenderWindow());
        visualizer->setGrid(grid);

        // 示例路径
        std::vector<Ogre::Vector2> path = {
            {0,0}, {1,0}, {2,0}, {3,1}, {4,2}, {5,2},
            {6,3}, {7,4}, {8,5}, {9,6}, {10,7}, {11,8}
        };
        visualizer->setPath(path, 0,0, 11,8);

        visualizer->update();
    }

    bool keyPressed(const OgreBites::KeyboardEvent& evt) override {
        if (evt.keysym.sym == SDLK_ESCAPE) getRoot()->queueEndRendering();
        return true;
    }
};

int main() {
    App app;
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();
    return 0;
}