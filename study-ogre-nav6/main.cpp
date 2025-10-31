// main.cpp - Weighted Hexagonal Grid A* Navigation System + Ogre Visualization

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

// === Custom hash function ===
struct PairHash {
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& p) const {
        auto h1 = std::hash<T>{}(p.first);
        auto h2 = std::hash<U>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

// === NavNode structure ===
struct NavNode {
    int x, y;
    float g, h;
    float f() const { return g + h; }
    bool operator>(const NavNode& other) const { return f() > other.f(); }
};

// === HexNavigationGrid class (with cost weights) ===
class HexNavigationGrid {
private:
    std::vector<std::vector<int>> costGrid;
    int width, height;

    // === Fixed hexagon neighbor offsets (flat-top) ===
    int dx_even[6] = {+1,  0, -1, -1, -1,  0};
    int dy_even[6] = { 0, -1, -1,  0, +1, +1};
    int dx_odd[6]  = {+1, +1,  0, -1,  0, +1};
    int dy_odd[6]  = { 0, -1, -1,  0, +1, +1};

public:
    static const int OBSTACLE = 0;
    static const int DEFAULT_COST = 1;

    HexNavigationGrid(int w, int h) : width(w), height(h) {
        costGrid.resize(height, std::vector<int>(width, DEFAULT_COST));
    }

    void setCost(int x, int y, int cost) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            costGrid[y][x] = cost;
        }
    }

    int getCost(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return OBSTACLE;
        return costGrid[y][x];
    }

    bool isWalkable(int x, int y) const {
        return getCost(x, y) > 0;
    }

    std::pair<int, int> getNeighbor(int x, int y, int direction) const {
        if (direction < 0 || direction >= 6) return {x, y};
        if (y % 2 == 0) {
            return {x + dx_even[direction], y + dy_even[direction]};
        } else {
            return {x + dx_odd[direction], y + dy_odd[direction]};
        }
    }

    float heuristic(int x1, int y1, int x2, int y2) const {
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

    std::vector<Ogre::Vector2> findPath(int startX, int startY, int endX, int endY) {
        using Pos = std::pair<int, int>;

        if (!isWalkable(startX, startY) || !isWalkable(endX, endY)) {
            return {};
        }

        std::priority_queue<NavNode, std::vector<NavNode>, std::greater<NavNode>> openList;
        std::unordered_map<Pos, Pos, PairHash> cameFrom;
        std::unordered_map<Pos, float, PairHash> gScore;
        std::unordered_set<Pos, PairHash> closed;

        NavNode start = {startX, startY, 0, heuristic(startX, startY, endX, endY)};
        openList.push(start);
        gScore[{startX, startY}] = 0;

        while (!openList.empty()) {
            NavNode current = openList.top();
            openList.pop();
            Pos currPos = {current.x, current.y};

            if (closed.find(currPos) != closed.end()) continue;
            closed.insert(currPos);

            if (current.x == endX && current.y == endY) {
                return reconstructPath(cameFrom, currPos);
            }

            for (int i = 0; i < 6; i++) {
                auto [nx, ny] = getNeighbor(current.x, current.y, i);
                if (!isWalkable(nx, ny)) continue;

                Pos neighbor = {nx, ny};
                int moveCost = getCost(nx, ny);
                float tentativeG = gScore[currPos] + moveCost;

                auto it = gScore.find(neighbor);
                if (it == gScore.end() || tentativeG < it->second) {
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
        const std::unordered_map<std::pair<int,int>, std::pair<int,int>, PairHash>& cameFrom,
        const std::pair<int,int>& current) const {

        std::vector<Ogre::Vector2> path;
        std::pair<int,int> node = current;

        while (cameFrom.find(node) != cameFrom.end()) {
            path.push_back(Ogre::Vector2(static_cast<float>(node.first), static_cast<float>(node.second)));
            node = cameFrom.at(node);
        }
        path.push_back(Ogre::Vector2(static_cast<float>(node.first), static_cast<float>(node.second)));

        std::reverse(path.begin(), path.end());
        return path;
    }

public:
    // === Print original cost grid ===
    void printCostGrid() const {
        std::cout << "Original Cost Grid (0=obstacle, 1=normal, 2=costly, 3=very costly):\n";
        for (int y = 0; y < height; y++) {
            if (y % 2 == 1) std::cout << " ";
            for (int x = 0; x < width; x++) {
                int cost = costGrid[y][x];
                if (cost == OBSTACLE) {
                    std::cout << "# ";
                } else {
                    std::cout << cost << " ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    // === Print path result grid ===
    void printPathGrid(int startx = -1, int starty = -1, int endx = -1, int endy = -1,
                       const std::vector<Ogre::Vector2>& path = {}) const {
        std::cout << "Path Result (S=start, E=end, *=path, number=cost):\n";

        std::unordered_set<std::pair<int, int>, PairHash> pathSet;
        for (const auto& p : path) {
            pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
        }

        for (int y = 0; y < height; y++) {
            if (y % 2 == 1) std::cout << " ";
            for (int x = 0; x < width; x++) {
                char c = '.';
                int cost = costGrid[y][x];

                if (x == startx && y == starty) {
                    c = 'S';
                } else if (x == endx && y == endy) {
                    c = 'E';
                } else if (cost == OBSTACLE) {
                    c = '#';
                } else if (pathSet.find({x, y}) != pathSet.end() &&
                           !(x == startx && y == starty) && !(x == endx && y == endy)) {
                    c = '*';  // Path
                } else {
                    if (cost == DEFAULT_COST) {
                        c = '.';  // Default cost shown as dot
                    } else {
                        c = '0' + cost;  // Show specific cost number
                    }
                }

                std::cout << c << " ";
            }
            std::cout << "\n";
        }
    }

    float calculatePathCost(const std::vector<Ogre::Vector2>& path) const {
        float totalCost = 0;
        for (size_t i = 1; i < path.size(); i++) {
            int x = static_cast<int>(path[i].x);
            int y = static_cast<int>(path[i].y);
            totalCost += getCost(x, y);
        }
        return totalCost;
    }

    // === Data interface for Ogre rendering ===
    const std::vector<std::vector<int>>& getCostGrid() const { return costGrid; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Get hexagon vertices (for Ogre rendering)
    std::vector<Ogre::Vector3> getHexagonVertices(int x, int y, float size) const {
        std::vector<Ogre::Vector3> vertices(6);
        float centerX = x * size * 1.5f;
        float centerY = y * size * std::sqrt(3.0f);
        if (y % 2 == 1) centerX += size * 0.75f;

        for (int i = 0; i < 6; i++) {
            float angle_rad = (60.0f * i + 30.0f) * 3.1415926f / 180.0f;
            float dx = size * std::cos(angle_rad);
            float dy = size * std::sin(angle_rad);
            vertices[i] = Ogre::Vector3(centerX + dx, centerY + dy, 0.0f);
        }

        return vertices;
    }
};

// === Hexagonal Map Visualizer class ===
class HexMapVisualizer {
private:
    Ogre::SceneManager* sceneMgr;
    Ogre::RenderWindow* window;
    Ogre::Camera* camera;
    Ogre::SceneNode* cameraNode;
    Ogre::ManualObject* hexGridObject;
    Ogre::ManualObject* pathObject;
    Ogre::SceneNode* gridNode;
    Ogre::SceneNode* pathNode;
    float hexSize;

public:
    HexMapVisualizer(Ogre::SceneManager* mgr, Ogre::RenderWindow* win, float size = 30.0f)
        : sceneMgr(mgr), window(win), hexSize(size) {
        
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
        Ogre::Viewport* vp = window->addViewport(camera);
        vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

        // Create hexagonal grid object
        hexGridObject = sceneMgr->createManualObject("HexGridObject");
        gridNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        gridNode->attachObject(hexGridObject);

        // Create path object
        pathObject = sceneMgr->createManualObject("PathObject");
        pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        pathNode->attachObject(pathObject);
    }

    void clear() {
        // Clear and reset objects
        hexGridObject->clear();
        pathObject->clear();
    }

    void drawHexGrid(const HexNavigationGrid& navGrid) {
        // Clear the object first
        hexGridObject->clear();
        
        int width = navGrid.getWidth();
        int height = navGrid.getHeight();
        
        // Begin the manual object
        hexGridObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int cost = navGrid.getCost(x, y);
                Ogre::ColourValue color = getCostColor(cost);
                
                auto vertices = navGrid.getHexagonVertices(x, y, hexSize);
                
                // Draw hexagon (triangle fan)
                if (cost == HexNavigationGrid::OBSTACLE) {
                    // Obstacles in red
                    drawHexagon(vertices, color);
                } else {
                    // Normal terrain with corresponding cost color
                    drawHexagon(vertices, color);
                }
            }
        }
        
        // End the manual object
        hexGridObject->end();
    }

    void drawPath(const HexNavigationGrid& navGrid, 
                  const std::vector<Ogre::Vector2>& path,
                  int startx, int starty, int endx, int endy) {
        if (path.empty()) return;

        // Clear the object first
        pathObject->clear();
        
        // Create path points set for quick lookup
        std::unordered_set<std::pair<int, int>, PairHash> pathSet;
        for (const auto& p : path) {
            pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
        }

        // Begin the manual object
        pathObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        int width = navGrid.getWidth();
        int height = navGrid.getHeight();

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                auto vertices = navGrid.getHexagonVertices(x, y, hexSize);
                
                if (x == startx && y == starty) {
                    // Start point in green
                    drawHexagon(vertices, Ogre::ColourValue::Green);
                } else if (x == endx && y == endy) {
                    // End point in blue
                    drawHexagon(vertices, Ogre::ColourValue::Blue);
                } else if (pathSet.find({x, y}) != pathSet.end()) {
                    // Path in yellow
                    drawHexagon(vertices, Ogre::ColourValue(1.0f, 1.0f, 0.0f));  // Yellow
                }
            }
        }

        // End the manual object
        pathObject->end();
    }

private:
    // Get color based on cost
    Ogre::ColourValue getCostColor(int cost) const {
        switch (cost) {
            case HexNavigationGrid::OBSTACLE: return Ogre::ColourValue::Red;
            case HexNavigationGrid::DEFAULT_COST: return Ogre::ColourValue(0.5f, 0.5f, 0.5f);  // Gray
            case 2: return Ogre::ColourValue(0.8f, 0.6f, 0.2f);  // Sand color
            case 3: return Ogre::ColourValue(0.2f, 0.4f, 0.8f);  // Water color
            default: return Ogre::ColourValue(0.7f, 0.7f, 0.7f);  // Default gray
        }
    }

    // Draw a single hexagon
    void drawHexagon(const std::vector<Ogre::Vector3>& vertices, 
                     const Ogre::ColourValue& color) {
        // Use triangle fan to draw hexagon
        for (int i = 1; i < 5; i++) {
            hexGridObject->position(vertices[0]);
            hexGridObject->colour(color * 0.8f);  // Center slightly darker
            
            hexGridObject->position(vertices[i]);
            hexGridObject->colour(color);
            
            hexGridObject->position(vertices[i+1]);
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
};

// === Input handler for closing application ===
class KeyHandler : public OgreBites::InputListener {
public:
    bool keyPressed(const OgreBites::KeyboardEvent& evt) override {
        if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) {
            Ogre::Root::getSingleton().queueEndRendering();
        }
        return true;
    }
};

// === Example main class ===
class HexNavigationExample {
private:
    std::unique_ptr<HexNavigationGrid> navGrid;
    std::unique_ptr<OgreBites::ApplicationContext> appCtx;
    Ogre::SceneManager* sceneMgr;
    std::unique_ptr<HexMapVisualizer> visualizer;
    KeyHandler keyHandler;

public:
    HexNavigationExample() : navGrid(std::make_unique<HexNavigationGrid>(12, 10)),
                           appCtx(std::make_unique<OgreBites::ApplicationContext>("HexagonalGridVisualizer")),
                           sceneMgr(nullptr) {
        setupExampleTerrain();
    }

    void setupExampleTerrain() {
        // Sand: cost 2
        for (int i = 3; i < 8; i++) {
            navGrid->setCost(i, 4, 2);
        }

        // Water: cost 3
        for (int i = 2; i < 6; i++) {
            navGrid->setCost(6, i, 3);
        }

        // Low cost path (like road)
        for (int i = 2; i < 10; i++) {
            navGrid->setCost(i, 7, 1);
        }

        // Obstacles
        navGrid->setCost(4, 3, HexNavigationGrid::OBSTACLE);
        navGrid->setCost(7, 5, HexNavigationGrid::OBSTACLE);
    }

    bool initOgre() {
        try {
            std::cout << "Initializing app context.\n";
            appCtx->initApp();
            std::cout << "App context initialized.\n";

            Ogre::Root* root = appCtx->getRoot();
            sceneMgr = root->createSceneManager();

            // Register our scene with the RTSS (Required for proper lighting/shaders)
            Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
            shadergen->addSceneManager(sceneMgr);

            // Create visualizer
            visualizer = std::make_unique<HexMapVisualizer>(sceneMgr, appCtx->getRenderWindow());

            // Add input listener
            appCtx->addInputListener(&keyHandler);

            std::cout << "Ogre initialized successfully.\n";
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Ogre initialization failed: " << e.what() << std::endl;
            return false;
        }
    }

    // Navigate and print dual maps
    std::vector<Ogre::Vector2> navigate(int sx, int sy, int ex, int ey) {
        std::cout << "Finding path from (" << sx << "," << sy << ") to (" << ex << "," << ey << "):\n";
        
        navGrid->printCostGrid();

        auto path = navGrid->findPath(sx, sy, ex, ey);
        
        std::cout << "Path found with " << path.size() << " hexes\n";
        if (!path.empty()) {
            float totalCost = navGrid->calculatePathCost(path);
            std::cout << "Total path cost: " << totalCost << "\n";
            std::cout << "Path: ";
            for (const auto& p : path) {
                std::cout << "(" << (int)p.x << "," << (int)p.y << ") ";
            }
            std::cout << "\n";
        }

        std::cout << "\n";
        navGrid->printPathGrid(sx, sy, ex, ey, path);

        // === Use Ogre for visualization ===
        if (visualizer) {
            try {
                visualizer->drawHexGrid(*navGrid);
                visualizer->drawPath(*navGrid, path, sx, sy, ex, ey);
                std::cout << "Ogre visualization updated.\n";
            } catch (const std::exception& e) {
                std::cerr << "Ogre rendering failed: " << e.what() << std::endl;
            }
        }

        std::cout << "\n" << std::string(50, '-') << "\n\n";
        return path;
    }

    void runVisualization() {
        if (appCtx && sceneMgr) {
            std::cout << "Starting Ogre visualization... Press ESC to exit.\n";
            try {
                // Start rendering loop
                appCtx->getRoot()->startRendering();
                std::cout << "Rendering loop ended.\n";
            } catch (const std::exception& e) {
                std::cerr << "Ogre rendering loop error: " << e.what() << std::endl;
            }
        } else {
            std::cout << "Ogre visualization not available.\n";
        }
    }

    HexNavigationGrid* getNavGrid() { return navGrid.get(); }
};

// === Main function ===
int main() {
    try {
        HexNavigationExample hexNav;

        std::cout << "Weighted Hexagonal Grid Navigation System\n";
        std::cout << "=========================================\n\n";

        // Initialize Ogre first
        if (!hexNav.initOgre()) {
            std::cout << "Failed to initialize Ogre, continuing with text output only.\n";
            return 1;
        }

        // Test paths
        hexNav.navigate(1, 1, 10, 8);
        hexNav.navigate(2, 2, 9, 7);

        // Run Ogre visualization
        hexNav.runVisualization();

        // Clean up
        std::cout << "Closing application.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}