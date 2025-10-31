// main.cpp - 带代价权重的六边形网格 A* 导航系统（双图输出版）

#include <Ogre.h>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <iostream>
#include <utility>
#include <algorithm>
#include <functional>
#include <initializer_list>

// === 自定义哈希函数 ===
struct PairHash {
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& p) const {
        auto h1 = std::hash<T>{}(p.first);
        auto h2 = std::hash<U>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

// === NavNode 结构体 ===
struct NavNode {
    int x, y;
    float g, h;
    float f() const { return g + h; }
    bool operator>(const NavNode& other) const { return f() > other.f(); }
};

// === HexNavigationGrid 类（支持代价权重）===
class HexNavigationGrid {
private:
    std::vector<std::vector<int>> costGrid;
    int width, height;

    // === 修复后的六边形邻居偏移（flat-top）===
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
    // === 打印原始代价图 ===
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

    // === 打印路径结果图 ===
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
                    c = '*';  // 路径
                } else {
                    // 显示原始代价（非路径点）
                    if (cost == DEFAULT_COST) {
                        c = '.';  // 默认代价显示为点
                    } else {
                        c = '0' + cost;  // 显示具体代价数字
                    }
                }

                std::cout << c << " ";
            }
            std::cout << "\n";
        }
    }

    float calculatePathCost(const std::vector<Ogre::Vector2>& path) const {
        float totalCost = 0;
        for (size_t i = 1; i < path.size(); i++) {  // 从第二个节点开始累加代价
            int x = static_cast<int>(path[i].x);
            int y = static_cast<int>(path[i].y);
            totalCost += getCost(x, y);
        }
        return totalCost;
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

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

// === 示例主类 ===
class HexNavigationExample {
private:
    std::unique_ptr<HexNavigationGrid> navGrid;

public:
    HexNavigationExample() : navGrid(std::make_unique<HexNavigationGrid>(12, 10)) {
        setupExampleTerrain();
    }

    void setupExampleTerrain() {
        // 沙地：代价 2
        for (int i = 3; i < 8; i++) {
            navGrid->setCost(i, 4, 2);
        }

        // 水域：代价 3
        for (int i = 2; i < 6; i++) {
            navGrid->setCost(6, i, 3);
        }

        // 低代价路径（如道路）
        for (int i = 2; i < 10; i++) {
            navGrid->setCost(i, 7, 1);
        }

        // 障碍物
        navGrid->setCost(4, 3, HexNavigationGrid::OBSTACLE);
        navGrid->setCost(7, 5, HexNavigationGrid::OBSTACLE);
    }

    // 导航并打印双图
    std::vector<Ogre::Vector2> navigate(int sx, int sy, int ex, int ey) {
        std::cout << "Finding path from (" << sx << "," << sy << ") to (" << ex << "," << ey << "):\n";
        
        // 先打印原始代价图
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

        // 再打印路径结果图
        std::cout << "\n";
        navGrid->printPathGrid(sx, sy, ex, ey, path);

        std::cout << "\n" << std::string(50, '-') << "\n\n";  // 分隔线
        return path;
    }

    HexNavigationGrid* getNavGrid() { return navGrid.get(); }
};

// === 主函数 ===
int main() {
    try {
        HexNavigationExample hexNav;

        std::cout << "Weighted Hexagonal Grid Navigation System\n";
        std::cout << "=========================================\n\n";

        // 测试1: (1,1) -> (10,8)
        hexNav.navigate(1, 1, 10, 8);

        // 测试2: (2,2) -> (9,7)
        hexNav.navigate(2, 2, 9, 7);

        // 测试3: (0,0) -> (11,9)
        hexNav.navigate(0, 0, 11, 9);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}