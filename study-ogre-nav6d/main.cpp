// main.cpp - Complete Ogre A* Hex Grid Visualization System

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

// === Include OgreBites for modern initialization ===
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>
// === Custom hash function ===
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

// === NavNode structure ===
struct NavNode
{
    int x, y;
    float g, h;
    float f() const { return g + h; }
    bool operator>(const NavNode &other) const { return f() > other.f(); }
};

//
struct InputState
{
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;

} inputState;
std::string materialNameInUse = "VertexColour";
std::string materialNameToCreate = "VertexColour2";

// === HexNavigationGrid class (with cost weights) ===
class HexNavigationGrid
{
private:
    std::vector<std::vector<int>> costGrid;
    int width, height;

    // === Fixed hexagon neighbor offsets (flat-top) ===
    int dx_even[6] = {+1, 0, -1, -1, -1, 0};
    int dy_even[6] = {0, -1, -1, 0, +1, +1};
    int dx_odd[6] = {+1, +1, 0, -1, 0, +1};
    int dy_odd[6] = {0, -1, -1, 0, +1, +1};

public:
    static const int OBSTACLE = 0;
    static const int DEFAULT_COST = 1;

    HexNavigationGrid(int w, int h) : width(w), height(h)
    {
        costGrid.resize(height, std::vector<int>(width, DEFAULT_COST));
    }

    void setCost(int x, int y, int cost)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            costGrid[y][x] = cost;
        }
    }

    int getCost(int x, int y) const
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return OBSTACLE;
        return costGrid[y][x];
    }

    bool isWalkable(int x, int y) const
    {
        return getCost(x, y) > 0;
    }

    std::pair<int, int> getNeighbor(int x, int y, int direction) const
    {
        if (direction < 0 || direction >= 6)
            return {x, y};
        if (y % 2 == 0)
        {
            return {x + dx_even[direction], y + dy_even[direction]};
        }
        else
        {
            return {x + dx_odd[direction], y + dy_odd[direction]};
        }
    }

    float heuristic(int x1, int y1, int x2, int y2) const
    {
        int q1 = x1 - (y1 - (y1 & 1)) / 2;
        int r1 = y1;
        int s1 = -q1 - r1;

        int q2 = x2 - (y2 - (y2 & 1)) / 2;
        int r2 = y2;
        int s2 = -q2 - r2;

        int dq = abs(q1 - q2);
        int dr = abs(r1 - r2);
        int ds = abs(s1 - s2);

        return static_cast<float>(std::max({dq, dr, ds}) * DEFAULT_COST);
    }

    std::vector<Ogre::Vector2> findPath(int startX, int startY, int endX, int endY)
    {
        using Pos = std::pair<int, int>;

        if (!isWalkable(startX, startY) || !isWalkable(endX, endY))
        {
            return {};
        }

        std::priority_queue<NavNode, std::vector<NavNode>, std::greater<NavNode>> openList;
        std::unordered_map<Pos, Pos, PairHash> cameFrom;
        std::unordered_map<Pos, float, PairHash> gScore;
        std::unordered_set<Pos, PairHash> closed;

        NavNode start = {startX, startY, 0, heuristic(startX, startY, endX, endY)};
        openList.push(start);
        gScore[{startX, startY}] = 0;

        while (!openList.empty())
        {
            NavNode current = openList.top();
            openList.pop();
            Pos currPos = {current.x, current.y};

            if (closed.find(currPos) != closed.end())
                continue;
            closed.insert(currPos);

            if (current.x == endX && current.y == endY)
            {
                return reconstructPath(cameFrom, currPos);
            }

            for (int i = 0; i < 6; i++)
            {
                auto [nx, ny] = getNeighbor(current.x, current.y, i);
                if (!isWalkable(nx, ny))
                    continue;

                Pos neighbor = {nx, ny};
                int moveCost = getCost(nx, ny);
                float tentativeG = gScore[currPos] + moveCost;

                auto it = gScore.find(neighbor);
                if (it == gScore.end() || tentativeG < it->second)
                {
                    cameFrom[neighbor] = currPos;
                    gScore[neighbor] = tentativeG;
                    float h = heuristic(nx, ny, endX, endY);

                    NavNode node = {nx, ny, tentativeG, h};
                    openList.push(node);
                }
            }
        }

        return {};
    }

private:
    std::vector<Ogre::Vector2> reconstructPath(
        const std::unordered_map<std::pair<int, int>, std::pair<int, int>, PairHash> &cameFrom,
        const std::pair<int, int> &current) const
    {

        std::vector<Ogre::Vector2> path;
        std::pair<int, int> node = current;

        while (cameFrom.find(node) != cameFrom.end())
        {
            path.push_back(Ogre::Vector2(static_cast<float>(node.first), static_cast<float>(node.second)));
            node = cameFrom.at(node);
        }
        path.push_back(Ogre::Vector2(static_cast<float>(node.first), static_cast<float>(node.second)));

        std::reverse(path.begin(), path.end());
        return path;
    }

public:
    // === Print original cost grid ===
    void printCostGrid() const
    {
        std::cout << "Original Cost Grid (0=obstacle, 1=normal, 2=costly, 3=very costly):\n";
        for (int y = 0; y < height; y++)
        {
            if (y % 2 == 1)
                std::cout << " ";
            for (int x = 0; x < width; x++)
            {
                int cost = costGrid[y][x];
                if (cost == OBSTACLE)
                {
                    std::cout << "# ";
                }
                else
                {
                    std::cout << cost << " ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    // === Print path result grid ===
    void printPathGrid(int startx = -1, int starty = -1, int endx = -1, int endy = -1,
                       const std::vector<Ogre::Vector2> &path = {}) const
    {
        std::cout << "Path Result (S=start, E=end, *=path, number=cost):\n";

        std::unordered_set<std::pair<int, int>, PairHash> pathSet;
        for (const auto &p : path)
        {
            pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
        }

        for (int y = 0; y < height; y++)
        {
            if (y % 2 == 1)
                std::cout << " ";
            for (int x = 0; x < width; x++)
            {
                char c = '.';
                int cost = costGrid[y][x];

                if (x == startx && y == starty)
                {
                    c = 'S';
                }
                else if (x == endx && y == endy)
                {
                    c = 'E';
                }
                else if (cost == OBSTACLE)
                {
                    c = '#';
                }
                else if (pathSet.find({x, y}) != pathSet.end() &&
                         !(x == startx && y == starty) && !(x == endx && y == endy))
                {
                    c = '*'; // Path
                }
                else
                {
                    if (cost == DEFAULT_COST)
                    {
                        c = '.'; // Default cost shown as dot
                    }
                    else
                    {
                        c = '0' + cost; // Show specific cost number
                    }
                }

                std::cout << c << " ";
            }
            std::cout << "\n";
        }
    }

    float calculatePathCost(const std::vector<Ogre::Vector2> &path) const
    {
        float totalCost = 0;
        for (size_t i = 1; i < path.size(); i++)
        {
            int x = static_cast<int>(path[i].x);
            int y = static_cast<int>(path[i].y);
            totalCost += getCost(x, y);
        }
        return totalCost;
    }

    // === Data interface for Ogre rendering ===
    const std::vector<std::vector<int>> &getCostGrid() const { return costGrid; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Get hexagon vertices (for Ogre rendering)
    std::vector<Ogre::Vector3> getHexagonVertices(int x, int y, float size) const
    {
        std::vector<Ogre::Vector3> vertices(6);
        float centerX = x * size * 1.5f;
        float centerY = y * size * std::sqrt(3.0f);
        if (y % 2 == 1)
            centerX += size * 0.75f;

        for (int i = 0; i < 6; i++)
        {
            float angle_rad = (60.0f * i + 30.0f) * 3.1415926f / 180.0f;
            float dx = size * std::cos(angle_rad);
            float dy = size * std::sin(angle_rad);
            vertices[i] = Ogre::Vector3(centerX + dx, centerY + dy, 0.0f);
        }

        return vertices;
    }
};

// === Hexagonal Map Visualizer class ===
class HexMapVisualizer
{
private:
    Ogre::SceneManager *sceneMgr;
    Ogre::RenderWindow *window;
    Ogre::Camera *camera;
    Ogre::SceneNode *cameraNode;
    Ogre::ManualObject *hexGridObject;
    Ogre::ManualObject *pathObject;
    Ogre::SceneNode *gridNode;
    Ogre::SceneNode *pathNode;
    float hexSize;

    // Current state
    const HexNavigationGrid *currentGrid;
    std::vector<Ogre::Vector2> currentPath;
    int startx, starty, endx, endy;
    bool gridDirty;
    bool pathDirty;

public:
    HexMapVisualizer(Ogre::SceneManager *mgr, Ogre::RenderWindow *win, float size = 30.0f)
        : sceneMgr(mgr), window(win), hexSize(size),
          currentGrid(nullptr), startx(-1), starty(-1), endx(-1), endy(-1),
          gridDirty(false), pathDirty(false)
    {

        // Create camera
        camera = sceneMgr->createCamera("HexMapCamera");
        camera->setNearClipDistance(0.1f);
        camera->setFarClipDistance(1000.0f);
        camera->setAutoAspectRatio(true);

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
        gridNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        gridNode->attachObject(hexGridObject);
        //

        // Create path object
        pathObject = sceneMgr->createManualObject("PathObject");
        pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        pathNode->attachObject(pathObject);
        createVertexColourMaterial();
    }
    // 在你的 HexMapVisualizer 构造函数或初始化函数中调用
    Ogre::MaterialPtr createVertexColourMaterial()
    {
        using namespace Ogre;

        // 创建材质，名称和资源组
        MaterialPtr mat = MaterialManager::getSingleton().create(materialNameToCreate, "General");

        // 禁用阴影接收
        mat->setReceiveShadows(false);

        // 获取默认技术（Ogre 2.x 默认会自动创建一个）
        Technique *tech = mat->getTechnique(0);

        // 配置 Pass
        Pass *pass = tech->getPass(0);
        pass->setLightingEnabled(false);  // 关闭光照
        pass->setDepthWriteEnabled(true); // 写入深度缓冲（推荐开启）
        pass->setCullingMode(CULL_NONE);  // 双面渲染（适合六边形从上下看）

        // 核心：启用顶点颜色混合
        // 使用当前片段颜色（无纹理）与顶点颜色进行混合
       // auto tu = pass->createTextureUnitState();
        //tu->setColourOperation(LayerBlendOperation::LBO_REPLACE);

        return mat;
    }

    Ogre::Camera *getCamera()
    {
        return this->camera;
    }

    void setGrid(const HexNavigationGrid &grid)
    {
        currentGrid = &grid;
        gridDirty = true;
    }

    void setPath(const std::vector<Ogre::Vector2> &path, int sx, int sy, int ex, int ey)
    {
        currentPath = path;
        startx = sx;
        starty = sy;
        endx = ex;
        endy = ey;
        pathDirty = true;
    }

    void update()
    {

        if (!currentGrid)
            return;

        if (gridDirty)
        {
            drawHexGrid();
            gridDirty = false;
        }

        if (pathDirty)
        {
            drawPath();
            pathDirty = false;
        }
    }

private:
    void drawHexGrid()
    {
        std::cout << "Drawing hex grid... width=" << currentGrid->getWidth()
                  << ", height=" << currentGrid->getHeight() << std::endl;
        if (!currentGrid)
            return;

        hexGridObject->clear();

        int width = currentGrid->getWidth();
        int height = currentGrid->getHeight();

        // Begin the manual object
        hexGridObject->begin(materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int cost = currentGrid->getCost(x, y);
                Ogre::ColourValue color = getCostColor(cost);

                auto vertices = currentGrid->getHexagonVertices(x, y, hexSize);

                // Draw hexagon (triangle fan)
                if (cost == HexNavigationGrid::OBSTACLE)
                {
                    // Obstacles in red
                    drawHexagonTo(hexGridObject, vertices, color);
                }
                else
                {
                    // Normal terrain with corresponding cost color
                    drawHexagonTo(hexGridObject, vertices, color);
                }
            }
        }

        // End the manual object
        hexGridObject->end();
        std::cout << "End of drawing hex grid" << std::endl;
    }

    void drawPath()
    {
        std::cout << "Drawing path ..." << std::endl;
        if (currentPath.empty())
        {
            pathObject->clear();
            return;
        }

        pathObject->clear();

        // Create path points set for quick lookup
        std::unordered_set<std::pair<int, int>, PairHash> pathSet;
        for (const auto &p : currentPath)
        {
            pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
        }

        // Begin the manual object
        pathObject->begin(materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        int width = currentGrid->getWidth();
        int height = currentGrid->getHeight();

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                auto vertices = currentGrid->getHexagonVertices(x, y, hexSize);

                if (x == startx && y == starty)
                {
                    // Start point in green
                    drawHexagonTo(pathObject, vertices, Ogre::ColourValue::Green);
                }
                else if (x == endx && y == endy)
                {
                    // End point in blue
                    drawHexagonTo(pathObject, vertices, Ogre::ColourValue::Blue);
                }
                else if (pathSet.find({x, y}) != pathSet.end())
                {
                    // Path in yellow
                    drawHexagonTo(pathObject, vertices, Ogre::ColourValue(1.0f, 1.0f, 0.0f)); // Yellow
                }
            }
        }

        // End the manual object
        pathObject->end();
        std::cout << "End of drawing path." << std::endl;
    }

    // Get color based on cost
    Ogre::ColourValue getCostColor(int cost) const
    {
        switch (cost)
        {
        case HexNavigationGrid::OBSTACLE:
            return Ogre::ColourValue::Red;
        case HexNavigationGrid::DEFAULT_COST:
        return Ogre::ColourValue(0.8f, 0.6f, 0.2f); // light Sand color
        case 2:
        return Ogre::ColourValue(0.6f, 0.4f, 0.1f); // Dark Sand color
        case 3:
            return Ogre::ColourValue(0.2f, 0.4f, 0.8f); // Water color
        default:
            return Ogre::ColourValue(0.7f, 0.7f, 0.7f); // light gray
        }
    }

    // Draw a single hexagon to a specific object
    void drawHexagonTo(Ogre::ManualObject *obj,
                       const std::vector<Ogre::Vector3> &vertices,
                       const Ogre::ColourValue &color)
    {
        // Compute center
        Ogre::Vector3 center(0, 0, 0);
        for (auto &v : vertices)
            center += v;
        center *= (1.0f / 6.0f);

        size_t baseIndex = obj->getCurrentVertexCount();

        // Center
        obj->position(center);
        obj->colour(color * 0.7f);

        // Corners
        for (int i = 0; i < 6; ++i)
        {
            obj->position(vertices[i]);
            obj->colour(color);
        }

        // Triangles
        for (int i = 0; i < 6; ++i)
        {
            int next = (i + 1) % 6;
            obj->triangle(baseIndex, baseIndex + i + 1, baseIndex + next + 1);
        }
    }
};

// === Frame Listener class for main loop ===
class HexApp : public Ogre::FrameListener
{
private:
    HexMapVisualizer *visualizer;
    bool quit;

public:
    HexApp(HexMapVisualizer *viz) : visualizer(viz), quit(false) {}

    bool frameStarted(const Ogre::FrameEvent &evt) override
    {
        //std::cout << "Frame started!\n";

        // Move camera
        Ogre::Camera *camera = visualizer->getCamera();
        Ogre::SceneNode *node = camera->getParentSceneNode();
        // 获取当前朝向（四元数）
        Ogre::Quaternion orientation = node->getOrientation();

        // 计算右向量（X轴）
        Ogre::Vector3 right = orientation * Ogre::Vector3::UNIT_X;
        Ogre::Vector3 up = orientation * Ogre::Vector3::UNIT_Y;
        float speed = 100.0f;

        if (inputState.up)
        {
            node->translate(up * speed * evt.timeSinceLastFrame);
        }
        if (inputState.down)
        {
            node->translate(-up * speed * evt.timeSinceLastFrame);
        }
        if (inputState.left)
        {
            node->translate(-right * speed * evt.timeSinceLastFrame);
        }
        if (inputState.right)
        {
            node->translate(right * speed * evt.timeSinceLastFrame);
        }
        // Update visualization
        if (visualizer)
        {
            visualizer->update();
        }
        Ogre::Root &root = Ogre::Root::getSingleton();
        Ogre::RenderWindow *window = root.getAutoCreatedWindow();
        // Check if window is closed
        if (window != nullptr && window->isClosed())
        {
            quit = true;
            std::cout << "quit = true!\n";
            return false;
        }

        return true; // Continue rendering
    }
};

// === Input handler for closing application ===
class KeyHandler : public OgreBites::InputListener
{
public:
    bool keyPressed(const OgreBites::KeyboardEvent &evt) override
    {
        if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
        {
            Ogre::Root::getSingleton().queueEndRendering();
        }
        if (evt.keysym.sym == OgreBites::SDLK_LEFT)
        {
            inputState.left = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
        {
            inputState.right = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_UP)
        {
            inputState.up = true;
        }
        if (evt.keysym.sym == OgreBites::SDLK_DOWN)
        {
            inputState.down = true;
        }
        return true;
    }
    bool keyReleased(const OgreBites::KeyboardEvent &evt) override
    {

        if (evt.keysym.sym == OgreBites::SDLK_LEFT)
        {
            inputState.left = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_RIGHT)
        {
            inputState.right = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_UP)
        {
            inputState.up = false;
        }
        if (evt.keysym.sym == OgreBites::SDLK_DOWN)
        {
            inputState.down = false;
        }
        return true;
    }
};

// === Main function ===
int main()
{
    std::cout << "OGRE Version: "
              << OGRE_VERSION_MAJOR << "."
              << OGRE_VERSION_MINOR << "."
              << OGRE_VERSION_PATCH << std::endl;
    try
    {
        std::cout << "Weighted Hexagonal Grid Navigation System\n";
        std::cout << "=========================================\n\n";

        // Initialize Ogre application context
        auto appCtx = std::make_unique<OgreBites::ApplicationContext>("HexagonalGridVisualizer");
        appCtx->initApp();

        Ogre::Root *root = appCtx->getRoot();
        Ogre::SceneManager *sceneMgr = root->createSceneManager();

        // Register our scene with the RTSS (Required for proper lighting/shaders)
        Ogre::RTShader::ShaderGenerator *shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(sceneMgr);
        
        // Ogre::RTShader::RenderState* renderState = shadergen->getRenderState(Ogre::RTShader::RS_DEFAULT);
        // std::string techName = "VertexColourTech";
        // Ogre::Pass *pass=nullptr;

        // Create visualizer
        HexMapVisualizer visualizer(sceneMgr, appCtx->getRenderWindow());

        // Create navigation grid and set up example terrain
        HexNavigationGrid navGrid(12, 10);

        // Sand: cost 2
        for (int i = 3; i < 8; i++)
        {
            navGrid.setCost(i, 4, 2);
        }

        // Water: cost 3
        for (int i = 2; i < 6; i++)
        {
            navGrid.setCost(6, i, 3);
        }

        // Low cost path (like road)
        for (int i = 2; i < 10; i++)
        {
            navGrid.setCost(i, 7, 1);
        }

        // Obstacles
        navGrid.setCost(4, 3, HexNavigationGrid::OBSTACLE);
        navGrid.setCost(7, 5, HexNavigationGrid::OBSTACLE);

        // Find path
        std::cout << "Finding path from (1,1) to (10,8):\n";
        navGrid.printCostGrid();
        auto path = navGrid.findPath(1, 1, 10, 8);
        std::cout << "Path found with " << path.size() << " hexes\n";
        if (!path.empty())
        {
            float totalCost = navGrid.calculatePathCost(path);
            std::cout << "Total path cost: " << totalCost << "\n";
            std::cout << "Path: ";
            for (const auto &p : path)
            {
                std::cout << "(" << (int)p.x << "," << (int)p.y << ") ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        navGrid.printPathGrid(1, 1, 10, 8, path);

        // Set data to visualizer (don't draw directly!)
        visualizer.setGrid(navGrid);
        visualizer.setPath(path, 1, 1, 10, 8);

        // Create frame listener for main loop
        HexApp frameListener(&visualizer);
        root->addFrameListener(&frameListener);

        // Add input listener
        KeyHandler keyHandler;
        appCtx->addInputListener(&keyHandler);

        std::cout << "Starting Ogre visualization... Press ESC to exit.\n";

        // Start rendering loop - this will call frameStarted automatically
        root->startRendering();

        std::cout << "Closing application.\n";
        appCtx->closeApp();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
