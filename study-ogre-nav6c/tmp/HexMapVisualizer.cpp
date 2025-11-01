// HexMapVisualizer.cpp - Hexagonal Map 2D Visualization for Ogre 1.x

#include "Ogre.h"
#include "OgreManualObject.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"
#include "OgreCamera.h"
#include "OgreViewport.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreMaterialManager.h"
#include "OgreTechnique.h"
#include "OgrePass.h"
#include <vector>
#include <unordered_set>
#include <functional>

// Import HexNavigationGrid class from main.cpp
class HexNavigationGrid
{
public:
    static const int OBSTACLE = 0;
    static const int DEFAULT_COST = 1;

    virtual ~HexNavigationGrid() = default;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual int getCost(int x, int y) const = 0;
    virtual std::vector<Ogre::Vector3> getHexagonVertices(int x, int y, float size) const = 0;
    virtual const std::vector<std::vector<int>> &getCostGrid() const = 0;
};

// Custom hash function
struct PairHash
{
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &p) const
    {
        auto h1 = std::hash<T>{}(p.first);
        auto h2 = std::hash<U>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

// Hexagonal Map Visualizer
class HexMapVisualizer
{
private:
    Ogre::SceneManager *sceneMgr;
    Ogre::RenderWindow *window;
    Ogre::Camera *camera;
    Ogre::SceneNode *cameraNode; // Use SceneNode to control camera position
    Ogre::ManualObject *hexGridObject;
    Ogre::ManualObject *pathObject;
    Ogre::SceneNode *gridNode;
    Ogre::SceneNode *pathNode;
    float hexSize;
    float gridOffsetX, gridOffsetY;

public:
    HexMapVisualizer(Ogre::SceneManager *mgr, Ogre::RenderWindow *win, float size = 30.0f)
        : sceneMgr(mgr), window(win), hexSize(size), gridOffsetX(200), gridOffsetY(100)
    {

        // Create camera
        camera = sceneMgr->createCamera("HexMapCamera");
        camera->setNearClipDistance(0.1f);
        camera->setFarClipDistance(1000.0f);

        // Create camera node and set position and direction
        cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        cameraNode->setPosition(0, 0, 500);
        cameraNode->attachObject(camera);
        cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_PARENT);

        // Create viewport
        Ogre::Viewport *vp = window->addViewport(camera);
        vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

        // Create hexagonal grid object
        hexGridObject = sceneMgr->createManualObject("HexGridObject");
        hexGridObject->setDynamic(true);
        gridNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        gridNode->attachObject(hexGridObject);

        // Create path object
        pathObject = sceneMgr->createManualObject("PathObject");
        pathObject->setDynamic(true);
        pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        pathNode->attachObject(pathObject);
    }

    // Clear all visualization objects
    void clear()
    {
        hexGridObject->clear();
        pathObject->clear();
    }

    // Draw hexagonal grid
    void drawHexGrid(const HexNavigationGrid &navGrid)
    {
        clear();

        int width = navGrid.getWidth();
        int height = navGrid.getHeight();

        hexGridObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int cost = navGrid.getCost(x, y);
                Ogre::ColourValue color = getCostColor(cost);

                auto vertices = navGrid.getHexagonVertices(x, y, hexSize);

                // Draw hexagon (triangle fan)
                if (cost == HexNavigationGrid::OBSTACLE)
                {
                    // Obstacles in red
                    drawHexagon(hexGridObject, vertices, Ogre::ColourValue::Red);
                }
                else
                {
                    // Normal terrain with corresponding cost color
                    drawHexagon(hexGridObject,vertices, color);
                }
            }
        }

        hexGridObject->end();
    }

    // Draw path
    void drawPath(const HexNavigationGrid &navGrid,
                  const std::vector<Ogre::Vector2> &path,
                  int startx, int starty, int endx, int endy)
    {
        if (path.empty())
            return;

        // Create path points set for quick lookup
        std::unordered_set<std::pair<int, int>, PairHash> pathSet;
        for (const auto &p : path)
        {
            pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
        }

        pathObject->clear();
        pathObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        int width = navGrid.getWidth();
        int height = navGrid.getHeight();

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                auto vertices = navGrid.getHexagonVertices(x, y, hexSize);

                if (x == startx && y == starty)
                {
                    // Start point in green
                    drawHexagon(pathObject, vertices, Ogre::ColourValue::Green);
                }
                else if (x == endx && y == endy)
                {
                    // End point in blue
                    drawHexagon(pathObject, vertices, Ogre::ColourValue::Blue);
                }
                else if (pathSet.find({x, y}) != pathSet.end())
                {
                    // Path in yellow
                    drawHexagon(pathObject, vertices, Ogre::ColourValue(1.0f, 1.0f, 0.0f)); // Yellow
                }
            }
        }

        pathObject->end();
    }

private:
    // Get color based on cost
    Ogre::ColourValue getCostColor(int cost) const
    {
        switch (cost)
        {
        case HexNavigationGrid::OBSTACLE:
            return Ogre::ColourValue::Red;
        case HexNavigationGrid::DEFAULT_COST:
            return Ogre::ColourValue(0.5f, 0.5f, 0.5f); // Gray
        case 2:
            return Ogre::ColourValue(0.8f, 0.6f, 0.2f); // Sand color
        case 3:
            return Ogre::ColourValue(0.2f, 0.4f, 0.8f); // Water color
        default:
            return Ogre::ColourValue(0.7f, 0.7f, 0.7f); // Default gray
        }
    }
    void drawHexagon(Ogre::ManualObject *obj,
                       const std::vector<Ogre::Vector3> &vertices,
                       const Ogre::ColourValue &color)
    {
        // Compute center
        Ogre::Vector3 center(0, 0, 0);
        for (auto &v : vertices)
            center += v;
        center *= (1.0f / 6.0f);

        // Add center
        obj->position(center);
        obj->colour(color * 0.7f);

        // Add 6 corners
        for (int i = 0; i < 6; ++i)
        {
            obj->position(vertices[i]);
            obj->colour(color);
        }

        // Create 6 triangles (fan)
        for (int i = 0; i < 6; ++i)
        {
            int next = (i + 1) % 6;
            obj->triangle(0, i + 1, next + 1);
        }
    }
    // Draw a single hexagon
    void drawHexagonx(const std::vector<Ogre::Vector3> &vertices,
                     const Ogre::ColourValue &color)
    {
        // Use triangle fan to draw hexagon
        for (int i = 1; i < 5; i++)
        {
            hexGridObject->position(vertices[0]);
            hexGridObject->colour(color * 0.8f); // Center slightly darker

            hexGridObject->position(vertices[i]);
            hexGridObject->colour(color);

            hexGridObject->position(vertices[i + 1]);
            hexGridObject->colour(color);
        }

        // Close fan
        hexGridObject->position(vertices[0]);
        hexGridObject->colour(color * 0.8f);
        hexGridObject->position(vertices[5]);
        hexGridObject->colour(color);
        hexGridObject->position(vertices[1]);
        hexGridObject->colour(color);
    }

public:
    // Get camera for external settings
    Ogre::Camera *getCamera() { return camera; }

    // Update scene
    void update()
    {
        // Add animation or update logic here
    }
};

// Simple Ogre application class
class HexMapApp : public Ogre::FrameListener
{
private:
    Ogre::Root *root;
    Ogre::RenderWindow *window;
    Ogre::SceneManager *sceneMgr;
    HexMapVisualizer *visualizer;
    bool quit;

public:
    HexMapApp() : root(nullptr), window(nullptr), sceneMgr(nullptr), visualizer(nullptr), quit(false) {}

    bool init()
    {
        root = new Ogre::Root("", "", "Ogre.log");

        // Try to restore config
        if (!root->restoreConfig())
        {
            // If no config file, use default renderer
            Ogre::RenderSystem *renderSystem = root->getRenderSystemByName("Direct3D9 Rendering Subsystem");
            if (!renderSystem)
            {
                renderSystem = root->getRenderSystemByName("OpenGL Rendering Subsystem");
            }

            root->setRenderSystem(renderSystem);
            root->initialise(false, "Hexagonal Grid Visualizer"); // Don't show config dialog
        }
        else
        {
            window = root->initialise(true, "Hexagonal Grid Visualizer");
        }

        sceneMgr = root->createSceneManager(); // Use Ogre 1.x scene manager type

        // Create visualizer
        visualizer = new HexMapVisualizer(sceneMgr, window);

        // Register frame listener
        root->addFrameListener(this);

        return true;
    }

    // Implement FrameListener interface
    bool frameRenderingQueued(const Ogre::FrameEvent &evt) override
    {
        // Check exit condition (e.g. press ESC)
        if (window->isClosed())
        {
            quit = true;
            return false;
        }

        // Update logic
        if (visualizer)
        {
            visualizer->update();
        }

        return true;
    }

    void run()
    {
        if (root)
        {
            root->startRendering();
        }
    }

    ~HexMapApp()
    {
        if (visualizer)
        {
            delete visualizer;
        }
        if (root)
        {
            delete root;
        }
    }
};