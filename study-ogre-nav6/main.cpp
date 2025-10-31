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

// 2D导航网格类 - 支持六边形网格的6方向移动
class HexNavigationGrid {
private:
    std::vector<std::vector<bool>> grid;  // true=可通行, false=障碍物
    int width, height;
    
    // 六边形网格的6方向移动偏移量（奇偶行交错）
    // 对于平顶六边形，相邻关系需要根据行的奇偶性调整
    int dx[6] = {-1, 0, 1, -1, 0, 1};  // x方向偏移
    int dy[6] = {0, -1, 0, 1, 1, 1};   // y方向偏移 (偶数行)
    int dy_odd[6] = {-1, -1, -1, 0, 0, 0};  // y方向偏移 (奇数行)
    
public:
    HexNavigationGrid(int w, int h) : width(w), height(h) {
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
    
    // 获取相邻六边形坐标
    std::pair<int, int> getNeighbor(int x, int y, int direction) const {
        if (y % 2 == 0) {  // 偶数行
            return std::make_pair(x + dx[direction], y + dy[direction]);
        } else {  // 奇数行
            return std::make_pair(x + dx[direction], y + dy_odd[direction]);
        }
    }
    
    // 计算启发函数（六边形网格距离）
    float heuristic(int x1, int y1, int x2, int y2) const {
        // 转换为立方坐标系计算距离
        int x1_c = x1 - (y1 - (y1 & 1)) / 2;
        int z1 = y1;
        int y1_c = -x1_c - z1;
        
        int x2_c = x2 - (y2 - (y2 & 1)) / 2;
        int z2 = y2;
        int y2_c = -x2_c - z2;
        
        // 六边形网格距离 = (|dx| + |dy| + |dz|) / 2
        int dx = abs(x1_c - x2_c);
        int dy = abs(y1_c - y2_c);
        int dz = abs(z1 - z2);
        
        return (dx + dy + dz) / 2.0f;
    }
    
    // A*算法寻找最短路径 - 六边形网格
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
        
        while (!openList.empty()) {
            NavNode current = openList.top();
            openList.pop();
            
            if (current.x == endX && current.y == endY) {
                // 重建路径
                return reconstructPath(cameFrom, current);
            }
            
            // 检查6个相邻六边形
            for (int i = 0; i < 6; i++) {
                auto [nx, ny] = getNeighbor(current.x, current.y, i);
                
                if (!isWalkable(nx, ny)) continue;
                
                float tentativeG = gScore[current] + 1.0f;  // 六边形移动成本相同
                
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
    // 获取网格数据用于可视化
    const std::vector<std::vector<bool>>& getGrid() const {
        return grid;
    }
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // 获取六边形顶点坐标
    std::vector<Ogre::Vector3> getHexagonVertices(int x, int y, float size) const {
        std::vector<Ogre::Vector3> vertices(6);
        
        // 计算六边形中心位置
        float centerX = x * size * 1.5f;
        float centerY = y * size * sqrt(3);
        
        // 如果是奇数行，需要偏移x坐标
        if (y % 2 == 1) {
            centerX += size * 0.75f;
        }
        
        // 计算6个顶点（平顶六边形）
        for (int i = 0; i < 6; i++) {
            float angle_deg = 60 * i + 30;  // +30度使顶点朝上
            float angle_rad = angle_deg * 3.1415926f / 180.0f;
            float dx = size * cos(angle_rad);
            float dy = size * sin(angle_rad);
            
            vertices[i] = Ogre::Vector3(centerX + dx, centerY + dy, 0);
        }
        
        return vertices;
    }
    
    // 打印六边形网格（文本形式）
    void printHexGrid(int startx = -1, int starty = -1, int endx = -1, int endy = -1) const {
        std::cout << "Hexagonal grid visualization:" << std::endl;
        
        for (int y = 0; y < height; y++) {
            // 奇数行缩进
            if (y % 2 == 1) std::cout << " ";
            
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
                
                // 在每行末尾添加空格以保持六边形视觉效果
                if (x < width - 1) std::cout << " ";
            }
            std::cout << std::endl;
        }
    }
};

// 六边形网格可视化类
class HexNavigationVisualizer {
private:
    Ogre::SceneManager* sceneMgr;
    Ogre::ManualObject* pathObject;
    Ogre::ManualObject* hexGridObject;
    Ogre::ManualObject* directionObject;
    Ogre::SceneNode* pathNode;
    Ogre::SceneNode* hexGridNode;
    Ogre::SceneNode* directionNode;
    
    float hexSize;  // 六边形大小
    Ogre::Vector3 gridOffset;  // 网格偏移量
    
public:
    HexNavigationVisualizer(Ogre::SceneManager* mgr, float size = 1.0f) 
        : sceneMgr(mgr), hexSize(size) {
        
        // 创建路径可视化对象
        pathObject = sceneMgr->createManualObject("PathObject");
        pathObject->setDynamic(true);
        pathNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        pathNode->attachObject(pathObject);
        
        // 创建六边形网格可视化对象
        hexGridObject = sceneMgr->createManualObject("HexGridObject");
        hexGridObject->setDynamic(true);
        hexGridNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        hexGridNode->attachObject(hexGridObject);
        
        // 创建方向可视化对象
        directionObject = sceneMgr->createManualObject("DirectionObject");
        directionObject->setDynamic(true);
        directionNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        directionNode->attachObject(directionObject);
        
        gridOffset = Ogre::Vector3(0, 0, 0);
    }
    
    // 更新六边形网格可视化
    void updateHexGridVisualization(const HexNavigationGrid& navGrid) {
        hexGridObject->clear();
        int width = navGrid.getWidth();
        int height = navGrid.getHeight();
        
        hexGridObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                auto vertices = navGrid.getHexagonVertices(x, y, hexSize);
                
                if (!navGrid.isWalkable(x, y)) {
                    // 绘制障碍物六边形（红色）
                    drawHexagon(vertices, Ogre::ColourValue::Red);
                } else {
                    // 绘制可通行六边形（白色边框）
                    drawHexagon(vertices, Ogre::ColourValue::White, false);
                }
            }
        }
        hexGridObject->end();
    }
    
    // 更新路径可视化
    void updatePathVisualization(const std::vector<Ogre::Vector2>& path, const HexNavigationGrid& navGrid) {
        pathObject->clear();
        if (path.empty()) return;
        
        pathObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
        
        for (size_t i = 0; i < path.size(); i++) {
            auto pos = getHexCenter(path[i].x, path[i].y, navGrid);
            pos.z = 0.02f;  // 稍微高一点避免被六边形覆盖
            pathObject->position(pos);
            
            // 高亮路径上的六边形
            auto vertices = navGrid.getHexagonVertices((int)path[i].x, (int)path[i].y, hexSize);
            if (i == 0) {  // 起点
                drawHexagon(vertices, Ogre::ColourValue::Green, true);
            } else if (i == path.size() - 1) {  // 终点
                drawHexagon(vertices, Ogre::ColourValue::Blue, true);
            } else {  // 路径中间
                drawHexagon(vertices, Ogre::ColourValue::Red, true);
            }
        }
        pathObject->end();
    }
    
    // 更新方向可视化
    void updateDirectionVisualization(int centerX, int centerY, const HexNavigationGrid& navGrid) {
        directionObject->clear();
        
        directionObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
        
        auto centerPos = getHexCenter(centerX, centerY, navGrid);
        centerPos.z = 0.03f;
        
        // 绘制6个方向的连接线
        for (int i = 0; i < 6; i++) {
            auto [nx, ny] = navGrid.getNeighbor(centerX, centerY, i);
            
            if (navGrid.isWalkable(nx, ny)) {
                auto neighborPos = getHexCenter(nx, ny, navGrid);
                neighborPos.z = 0.03f;
                
                // 绘制方向线
                directionObject->position(centerPos);
                directionObject->position(neighborPos);
                
                // 绘制箭头
                Ogre::Vector3 dir = (neighborPos - centerPos).normalisedCopy();
                Ogre::Vector3 perp(-dir.z, 0, dir.x);
                
                Ogre::Vector3 arrow1 = neighborPos - dir * 0.2f + perp * 0.1f;
                Ogre::Vector3 arrow2 = neighborPos - dir * 0.2f - perp * 0.1f;
                
                directionObject->position(neighborPos);
                directionObject->position(arrow1);
                directionObject->position(neighborPos);
                directionObject->position(arrow2);
            }
        }
        
        directionObject->end();
    }
    
    // 清除所有可视化
    void clearAllVisualization() {
        pathObject->clear();
        hexGridObject->clear();
        directionObject->clear();
    }

private:
    // 获取六边形中心坐标
    Ogre::Vector3 getHexCenter(int x, int y, const HexNavigationGrid& navGrid) const {
        float centerX = x * hexSize * 1.5f;
        float centerY = y * hexSize * sqrt(3);
        
        // 如果是奇数行，需要偏移x坐标
        if (y % 2 == 1) {
            centerX += hexSize * 0.75f;
        }
        
        return Ogre::Vector3(centerX + gridOffset.x, centerY + gridOffset.y, 0);
    }
    
    // 绘制六边形
    void drawHexagon(const std::vector<Ogre::Vector3>& vertices, 
                    const Ogre::ColourValue& color, bool filled = false) {
        
        if (filled) {
            // 绘制填充的六边形
            for (int i = 1; i < 5; i++) {
                hexGridObject->position(vertices[0]); hexGridObject->colour(color * 0.7f);
                hexGridObject->position(vertices[i]); hexGridObject->colour(color);
                hexGridObject->position(vertices[i+1]); hexGridObject->colour(color);
            }
            // 闭合三角形扇形
            hexGridObject->position(vertices[0]); hexGridObject->colour(color * 0.7f);
            hexGridObject->position(vertices[5]); hexGridObject->colour(color);
            hexGridObject->position(vertices[1]); hexGridObject->colour(color);
        } else {
            // 绘制六边形边框
            for (int i = 0; i < 6; i++) {
                int next = (i + 1) % 6;
                hexGridObject->position(vertices[i]); hexGridObject->colour(color);
                hexGridObject->position(vertices[next]); hexGridObject->colour(color);
            }
        }
    }
};

// 示例使用类
class HexNavigationExample {
private:
    HexNavigationGrid* navGrid;
    HexNavigationVisualizer* visualizer;
    
public:
    HexNavigationExample() {
        navGrid = new HexNavigationGrid(12, 10);
        visualizer = nullptr;
        
        // 设置一些障碍物
        setupExampleObstacles();
    }
    
    ~HexNavigationExample() {
        delete navGrid;
    }
    
    void setupExampleObstacles() {
        // 创建一个简单的障碍物布局
        for (int i = 3; i < 8; i++) {
            navGrid->setObstacle(i, 4, true);  // 水平墙
        }
        
        for (int i = 2; i < 6; i++) {
            navGrid->setObstacle(6, i, true);  // 垂直墙
        }
        
        // 创建一个通道
        navGrid->setObstacle(6, 4, false);
    }
    
    // 执行导航计算
    std::vector<Ogre::Vector2> navigate(int startx, int starty, int endx, int endy) {
        auto path = navGrid->findPath(startx, starty, endx, endy);
        
        // 打印调试信息
        std::cout << "Path found with " << path.size() << " hexes" << std::endl;
        if (!path.empty()) {
            std::cout << "Path: ";
            for (const auto& node : path) {
                std::cout << "(" << (int)node.x << "," << (int)node.y << ") ";
            }
            std::cout << std::endl;
        }
        
        return path;
    }
    
    // 打印六边形网格
    void printHexGrid(int startx = -1, int starty = -1, int endx = -1, int endy = -1) const {
        navGrid->printHexGrid(startx, starty, endx, endy);
    }
    
    // 设置可视化器
    void setVisualizer(HexNavigationVisualizer* viz) {
        visualizer = viz;
    }
    
    // 获取导航网格
    HexNavigationGrid* getNavGrid() { return navGrid; }
};

// 主函数示例
int main() {
    try {
        // 创建六边形导航系统
        HexNavigationExample hexNavExample;
        
        std::cout << "Hexagonal Grid Navigation System" << std::endl;
        std::cout << "=================================" << std::endl;
        
        // 打印初始六边形网格
        std::cout << "Initial hexagonal grid (S=start, E=end, #=obstacle, .=walkable):" << std::endl;
        hexNavExample.printHexGrid(2, 2, 9, 7);  // 标记起点(2,2)和终点(9,7)
        
        // 执行导航
        std::cout << "\nFinding path from (2,2) to (9,7):" << std::endl;
        auto path = hexNavExample.navigate(2, 2, 9, 7);
        
        if (path.empty()) {
            std::cout << "No path found!" << std::endl;
        } else {
            std::cout << "Successfully found path with " << path.size() << " hexes!" << std::endl;
        }
        
        // 在实际OGRE应用中，这里会调用可视化更新
        std::cout << "\nIn a real OGRE application, the following would be visualized:" << std::endl;
        std::cout << "- Hexagonal grid with alternating row offset" << std::endl;
        std::cout << "- Red hexes for obstacles" << std::endl;
        std::cout << "- Green hex for start point" << std::endl;
        std::cout << "- Blue hex for end point" << std::endl;
        std::cout << "- Yellow hexes for path nodes" << std::endl;
        std::cout << "- Direction arrows showing 6 possible movement directions" << std::endl;
        
        // 测试另一个路径
        std::cout << "\nTesting path (1, 1) -> (10, 8):" << std::endl;
        hexNavExample.printHexGrid(1, 1, 10, 8);
        auto path2 = hexNavExample.navigate(1, 1, 10, 8);
        
        // 演示六边形邻居关系
        std::cout << "\nDemonstrating hexagonal neighbor relationships:" << std::endl;
        auto navGrid = hexNavExample.getNavGrid();
        for (int dir = 0; dir < 6; dir++) {
            auto [nx, ny] = navGrid->getNeighbor(5, 5, dir);
            std::cout << "Direction " << dir << " from (5,5): (" << nx << "," << ny << ")" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}



