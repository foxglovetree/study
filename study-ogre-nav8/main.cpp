#include <Ogre.h>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <iostream>

// 2D导航网格节点
struct NavNode {
    int x, y;
    float g, h;  // g:实际代价, h:启发函数
    float f() const { return g + h; }
    bool operator>(const NavNode& other) const { return f() > other.f(); }
    bool operator==(const NavNode& other) const { return x == other.x && y == other.y; }
    
    // 用于unordered_map的哈希函数
    struct Hash {
        size_t operator()(const NavNode& node) const {
            return std::hash<int>()(node.x) ^ std::hash<int>()(node.y);
        }
    };
};

// 2D导航网格类
class NavigationGrid2D {
private:
    std::vector<std::vector<bool>> grid;  // true=可通行, false=障碍物
    int width, height;
    
public:
    NavigationGrid2D(int w, int h) : width(w), height(h) {
        grid.resize(height, std::vector<bool>(width, true));  // 默认全部可通行
    }
    
    // 设置障碍物
    void setObstacle(int x, int y, bool isObstacle = true) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = !isObstacle;
        }
    }
    
    // 检查位置是否可通行
    bool isWalkable(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        return grid[y][x];
    }
    
    // 计算启发函数（曼哈顿距离）
    float heuristic(int x1, int y1, int x2, int y2) const {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }
    
    // A*算法寻找最短路径
    std::vector<Ogre::Vector2> findPath(int startX, int startY, int endX, int endY) {
        if (!isWalkable(startX, startY) || !isWalkable(endX, endY)) {
            return {};  // 起点或终点不可通行
        }
        
        std::priority_queue<NavNode, std::vector<NavNode>, std::greater<NavNode>> openList;
        std::unordered_map<NavNode, NavNode, NavNode::Hash> cameFrom;
        std::unordered_map<NavNode, float, NavNode::Hash> gScore;
        
        NavNode start = {startX, startY, 0, heuristic(startX, startY, endX, endY)};
        openList.push(start);
        gScore[start] = 0;
        
        // 8方向移动（包括对角线）
        int dx[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
        int dy[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
        float costs[8] = {1.414f, 1.0f, 1.414f, 1.0f, 1.0f, 1.414f, 1.0f, 1.414f};  // 对角线成本
        
        while (!openList.empty()) {
            NavNode current = openList.top();
            openList.pop();
            
            if (current.x == endX && current.y == endY) {
                // 重建路径
                return reconstructPath(cameFrom, current);
            }
            
            // 检查8个相邻节点
            for (int i = 0; i < 8; i++) {
                int nx = current.x + dx[i];
                int ny = current.y + dy[i];
                
                if (!isWalkable(nx, ny)) continue;
                
                float tentativeG = gScore[current] + costs[i];
                
                if (gScore.find({nx, ny}) == gScore.end() || tentativeG < gScore[{nx, ny}]) {
                    cameFrom[{nx, ny}] = current;
                    gScore[{nx, ny}] = tentativeG;
                    float h = heuristic(nx, ny, endX, endY);
                    
                    NavNode neighbor = {nx, ny, tentativeG, h};
                    openList.push(neighbor);
                }
            }
        }
        
        return {}; // 无路径
    }
    
private:
    std::vector<Ogre::Vector2> reconstructPath(
        const std::unordered_map<NavNode, NavNode, NavNode::Hash>& cameFrom,
        const NavNode& current) const {
        
        std::vector<Ogre::Vector2> path;
        NavNode node = current;
        
        while (cameFrom.find(node) != cameFrom.end()) {
            path.push_back(Ogre::Vector2(node.x, node.y));
            node = cameFrom.at(node);
        }
        path.push_back(Ogre::Vector2(node.x, node.y));  // 添加起点
        
        std::reverse(path.begin(), path.end());
        return path;
    }
    
public:
    // 可视化网格（用于调试）
    void printGrid(int startx = -1, int starty = -1, int endx = -1, int endy = -1) const {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (x == startx && y == starty) {
                    std::cout << "S ";  // 起点
                } else if (x == endx && y == endy) {
                    std::cout << "E ";  // 终点
                } else if (!grid[y][x]) {
                    std::cout << "# ";  // 障碍物
                } else {
                    std::cout << ". ";  // 可通行
                }
            }
            std::cout << std::endl;
        }
    }
};

// OGRE 2D导航可视化类
class Navigation2DVisualizer {
private:
    Ogre::SceneManager* sceneMgr;
    Ogre::ManualObject* pathObject;
    Ogre::SceneNode* pathNode;
    std::vector<Ogre::Vector2> currentPath;
    
public:
    Navigation2DVisualizer(Ogre::SceneManager* mgr) : sceneMgr(mgr) {
        // 创建路径可视化对象
        pathObject = sceneMgr->createManualObject("PathObject");
        pathObject->setDynamic(true);
        sceneMgr->getRootSceneNode()->attachObject(pathObject);
        
        pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        pathObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    }
    
    // 更新路径可视化
    void updatePathVisualization(const std::vector<Ogre::Vector2>& path) {
        currentPath = path;
        
        pathObject->clear();
        if (path.empty()) return;
        
        pathObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
        
        for (size_t i = 0; i < path.size(); i++) {
            // 将2D坐标转换为3D坐标（Z=0）
            Ogre::Vector3 pos(path[i].x, path[i].y, 0);
            pathObject->position(pos);
            
            // 添加小点标记路径节点
            if (i > 0) {
                pathObject->position(pos + Ogre::Vector3(0.1f, 0.1f, 0));
                pathObject->position(pos);
            }
        }
        pathObject->end();
    }
    
    // 清除路径可视化
    void clearPathVisualization() {
        pathObject->clear();
    }
};

// 示例使用类
class NavigationExample {
private:
    NavigationGrid2D* navGrid;
    Navigation2DVisualizer* visualizer;
    
public:
    NavigationExample() {
        navGrid = new NavigationGrid2D(20, 15);
        visualizer = nullptr;  // OGRE场景管理器需要在外部传入
        
        // 设置一些障碍物
        setupExampleObstacles();
    }
    
    ~NavigationExample() {
        delete navGrid;
    }
    
    void setupExampleObstacles() {
        // 创建一个简单的障碍物布局
        for (int i = 5; i < 15; i++) {
            navGrid->setObstacle(i, 7, true);  // 水平墙
        }
        
        for (int i = 3; i < 10; i++) {
            navGrid->setObstacle(10, i, true);  // 垂直墙
        }
        
        // 创建一个通道
        navGrid->setObstacle(10, 7, true);
    }
    
    // 执行导航计算
    std::vector<Ogre::Vector2> navigate(int startx, int starty, int endx, int endy) {
        auto path = navGrid->findPath(startx, starty, endx, endy);
        
        // 打印调试信息
        std::cout << "Path found with " << path.size() << " nodes" << std::endl;
        if (!path.empty()) {
            std::cout << "Path: ";
            for (const auto& node : path) {
                std::cout << "(" << (int)node.x << "," << (int)node.y << ") ";
            }
            std::cout << std::endl;
        }
        
        return path;
    }
    
    // 打印网格状态
    void printGrid(int startx = -1, int starty = -1, int endx = -1, int endy = -1) const {
        navGrid->printGrid(startx, starty, endx, endy);
    }
    
    // 设置可视化器
    void setVisualizer(Navigation2DVisualizer* viz) {
        visualizer = viz;
    }
};

// 主函数示例
int main() {
    try {
        // 创建导航系统
        NavigationExample navExample;
        
        // 打印初始网格
        std::cout << "Initial grid (S=start, E=end, #=obstacle, .=walkable):" << std::endl;
        navExample.printGrid(2, 2, 18, 12);  // 标记起点(2,2)和终点(18,12)
        
        // 执行导航
        auto path = navExample.navigate(2, 2, 18, 12);
        
        if (path.empty()) {
            std::cout << "No path found!" << std::endl;
        } else {
            std::cout << "Successfully found path!" << std::endl;
            // 在实际应用中，这里会更新OGRE的可视化
        }
        
        // 测试另一个路径
        std::cout << "\nTesting another path (5, 5) -> (15, 10):" << std::endl;
        navExample.printGrid(5, 5, 15, 10);
        auto path2 = navExample.navigate(5, 5, 15, 10);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}



