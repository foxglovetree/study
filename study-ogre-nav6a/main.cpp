// main.cpp - 六边形网格 A* 导航系统（带路径可视化）

#include <Ogre.h>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <iostream>
#include <utility>  // for std::pair
#include <algorithm>  // for std::max
#include <functional> // for std::hash
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

// === NavNode 结构体（用于A*搜索）===
struct NavNode {
    int x, y;
    float g, h;
    float f() const { return g + h; }
    bool operator>(const NavNode& other) const { return f() > other.f(); }
};

// === HexNavigationGrid 类 ===
class HexNavigationGrid {
private:
    std::vector<std::vector<bool>> grid;
    int width, height;

    // === 修正后的六边形邻居偏移（flat-top）===
    // 方向顺序：E, NE, NW, W, SW, SE
    int dx_even[6] = {+1,  0, -1, -1, -1,  0};
    int dy_even[6] = { 0, -1, -1,  0, +1, +1};

    int dx_odd[6]  = {+1, +1,  0, -1,  0, +1};
    int dy_odd[6]  = { 0, -1, -1,  0, +1, +1};

public:
    HexNavigationGrid(int w, int h) : width(w), height(h) {
        grid.resize(height, std::vector<bool>(width, true));
    }

    void setObstacle(int x, int y, bool isObstacle = true) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y][x] = !isObstacle;
        }
    }

    bool isWalkable(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        return grid[y][x];
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

        int dq = std::abs(q1 - q2);
        int dr = std::abs(r1 - r2);
        int ds = std::abs(s1 - s2);
        return static_cast<float>(std::max({dq, dr, ds}));
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
                float tentativeG = gScore[currPos] + 1.0f;

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

        return {}; // 无路径
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
        path.push_back(Ogre::Vector2(static_cast<float>(node.first), static_cast<float>(node.second))); // 起点

        std::reverse(path.begin(), path.end());
        return path;
    }

public:
    const std::vector<std::vector<bool>>& getGrid() const { return grid; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // === 增强版 printHexGrid：支持显示路径 ===
    void printHexGrid(int startx = -1, int starty = -1, int endx = -1, int endy = -1,
                      const std::vector<Ogre::Vector2>& path = {}) const {
        std::cout << "Hexagonal grid visualization:\n";

        // 将路径点转换为集合，便于快速查找
        std::unordered_set<std::pair<int, int>, PairHash> pathSet;
        for (const auto& p : path) {
            pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
        }

        for (int y = 0; y < height; y++) {
            if (y % 2 == 1) std::cout << " ";  // 锯齿对齐
            for (int x = 0; x < width; x++) {
                char c = '.';

                if (x == startx && y == starty) {
                    c = 'S';
                } else if (x == endx && y == endy) {
                    c = 'E';
                } else if (!grid[y][x]) {
                    c = '#';
                } else if (pathSet.find({x, y}) != pathSet.end()) {
                    // 是路径点，且不是起点或终点
                    if (!(x == startx && y == starty) && !(x == endx && y == endy)) {
                        c = '*';
                    }
                }

                std::cout << c << " ";
            }
            std::cout << "\n";
        }
    }

    std::vector<Ogre::Vector3> getHexagonVertices(int x, int y, float size) const {
        std::vector<Ogre::Vector3> vertices(6);
        float centerX = x * size * 1.5f;
        float centerY = y * size * std::sqrt(3.0f);

        if (y % 2 == 1) {
            centerX += size * 0.75f;
        }

        for (int i = 0; i < 6; i++) {
            float angle_rad = (60.0f * i + 30.0f) * 3.1415926f / 180.0f;
            float dx = size * std::cos(angle_rad);
            float dy = size * std::sin(angle_rad);
            vertices[i] = Ogre::Vector3(centerX + dx, centerY + dy, 0.0f);
        }

        return vertices;
    }
};

// === 示例主类（未变）===
class HexNavigationExample {
private:
    std::unique_ptr<HexNavigationGrid> navGrid;

public:
    HexNavigationExample() : navGrid(std::make_unique<HexNavigationGrid>(12, 10)) {
        setupExampleObstacles();
    }

    void setupExampleObstacles() {
        for (int i = 3; i < 8; i++) navGrid->setObstacle(i, 4, true);  // 水平墙
        for (int i = 2; i < 6; i++) navGrid->setObstacle(6, i, true);   // 垂直墙
        navGrid->setObstacle(6, 4, false); // 交叉口留通
    }

    std::vector<Ogre::Vector2> navigate(int sx, int sy, int ex, int ey) {
        auto path = navGrid->findPath(sx, sy, ex, ey);
        std::cout << "Path found with " << path.size() << " hexes\n";
        if (!path.empty()) {
            std::cout << "Path: ";
            for (const auto& p : path) {
                std::cout << "(" << (int)p.x << "," << (int)p.y << ") ";
            }
            std::cout << "\n";
        }
        return path;
    }

    void printHexGrid(int sx, int sy, int ex, int ey, const std::vector<Ogre::Vector2>& path = {}) const {
        navGrid->printHexGrid(sx, sy, ex, ey, path);
    }

    HexNavigationGrid* getNavGrid() { return navGrid.get(); }
};

// === 主函数（现在会显示带 * 的路径）===
int main() {
    try {
        HexNavigationExample hexNav;

        std::cout << "Hexagonal Grid Navigation System\n";
        std::cout << "=================================\n";

        // 测试1: (2,2) -> (9,7)
        std::cout << "Finding path from (2,2) to (9,7):\n";
        auto path1 = hexNav.navigate(2, 2, 9, 7);

        std::cout << "\nVisualization of path (S=start, E=end, *=path, #=obstacle):\n";
        hexNav.printHexGrid(2, 2, 9, 7, path1);  // 传入路径，显示 *

        // 测试2: (1,1) -> (10,8)
        std::cout << "\n\nTesting path (1,1) -> (10,8):\n";
        auto path2 = hexNav.navigate(1, 1, 10, 8);

        std::cout << "\nVisualization:\n";
        hexNav.printHexGrid(1, 1, 10, 8, path2);

        // 邻居测试
        std::cout << "\nNeighbor demo from (5,5):\n";
        auto* grid = hexNav.getNavGrid();
        for (int dir = 0; dir < 6; dir++) {
            auto [nx, ny] = grid->getNeighbor(5, 5, dir);
            std::cout << "Dir " << dir << ": (" << nx << "," << ny << ")\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}