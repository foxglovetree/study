#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    
    double cross(const Point& other) const {
        return x * other.y - y * other.x;
    }
    
    double dot(const Point& other) const {
        return x * other.x + y * other.y;
    }
    
    double length() const {
        return sqrt(x * x + y * y);
    }
};

// 射线法判断点是否在多边形内部
bool isPointInPolygon(const Point& p, const std::vector<Point>& polygon) {
    int n = polygon.size();
    bool inside = false;
    
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        if (((polygon[i].y > p.y) != (polygon[j].y > p.y)) &&
            (p.x < (polygon[j].x - polygon[i].x) * (p.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

// 计算点到多边形边界的距离
double distanceToPolygon(const Point& p, const std::vector<Point>& polygon) {
    double minDist = 999999.0;
    
    for (size_t i = 0; i < polygon.size(); i++) {
        int j = (i + 1) % polygon.size();
        
        Point edge = polygon[j] - polygon[i];
        Point pointToStart = p - polygon[i];
        
        double edgeLength = edge.length();
        double edgeDot = edge.dot(pointToStart);
        
        if (edgeLength < 0.0001) continue; // 避免除零
        
        double t = std::max(0.0, std::min(1.0, edgeDot / (edgeLength * edgeLength)));
        
        Point projection = Point(polygon[i].x + t * edge.x, polygon[i].y + t * edge.y);
        double dist = (p - projection).length();
        
        minDist = std::min(minDist, dist);
    }
    
    return minDist;
}

class TerrainGenerator {
private:
    std::vector<Point> contour;  // 轮廓点
    int width, height;
    double scale;
    double maxDistance;  // 轮廓外最大影响距离
    
public:
    TerrainGenerator(const std::vector<Point>& _contour, int _width, int _height, double _scale = 1.0)
        : contour(_contour), width(_width), height(_height), scale(_scale) {
        
        // 计算轮廓的边界框，确定最大影响距离
        double minX = 999999, minY = 999999, maxX = -999999, maxY = -999999;
        for (const auto& p : contour) {
            minX = std::min(minX, p.x);
            minY = std::min(minY, p.y);
            maxX = std::max(maxX, p.x);
            maxY = std::max(maxY, p.y);
        }
        
        maxDistance = std::max(maxX - minX, maxY - minY) * 0.5;  // 设定为轮廓尺寸的一半
    }
    
    // 生成地形高度数据
    std::vector<std::vector<double>> generateTerrain() {
        std::vector<std::vector<double>> terrain(height, std::vector<double>(width, 0.0));
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Point currentPos(x * scale, y * scale);
                
                if (isPointInPolygon(currentPos, contour)) {
                    // 在轮廓内部，可以生成陆地地形
                    terrain[y][x] = generateInlandHeight(currentPos);
                } else {
                    // 在轮廓外部，根据距离轮廓的远近生成地形
                    double dist = distanceToPolygon(currentPos, contour);
                    terrain[y][x] = generateOutlandHeight(dist);
                }
            }
        }
        
        return terrain;
    }
    
private:
    // 轮廓内部的地形高度生成（可以是平原、丘陵、山脉等）
    double generateInlandHeight(const Point& pos) {
        // 这里可以加入噪声算法生成更自然的地形
        // 简单示例：随机高度 + 距离中心的调整
        double centerX = 0, centerY = 0;
        for (const auto& p : contour) {
            centerX += p.x;
            centerY += p.y;
        }
        centerX /= contour.size();
        centerY /= contour.size();
        
        double distToCenter = sqrt((pos.x - centerX) * (pos.x - centerX) + 
                                  (pos.y - centerY) * (pos.y - centerY));
        
        // 基础高度 + 中心高四周低的地形
        double baseHeight = 50.0 + (rand() % 20 - 10) * 0.1;  // 随机基础高度
        double centerHill = std::max(0.0, 30.0 * (1.0 - distToCenter / maxDistance * 0.5));
        
        return baseHeight + centerHill;
    }
    
    // 轮廓外部的地形高度生成（如浅海、深海）
    double generateOutlandHeight(const double& dist) {
        if (dist < 10.0) {
            // 靠近轮廓的浅海区域
            return -10.0 + std::max(0.0, -dist * 0.5);
        } else if (dist < 30.0) {
            // 较远的深海区域
            return -30.0;
        } else {
            // 远离轮廓的区域（可设置为更低的海床）
            return -50.0;
        }
    }
    
public:
    // 输出地形数据到文件（便于查看和导入其他工具）
    void saveTerrainToFile(const std::vector<std::vector<double>>& terrain, const std::string& filename) {
        std::ofstream file(filename);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                file << terrain[y][x];
                if (x < width - 1) file << " ";
            }
            file << "\n";
        }
        file.close();
        std::cout << "Terrain saved to " << filename << std::endl;
    }
    
    // 输出轮廓到文件（便于可视化）
    void saveContourToFile(const std::string& filename) {
        std::ofstream file(filename);
        file << "x,y\n";
        for (const auto& p : contour) {
            file << p.x << "," << p.y << "\n";
        }
        file.close();
        std::cout << "Contour saved to " << filename << std::endl;
    }
};

int main() {
    // 定义一个简单的多边形轮廓（陆地边界）
    std::vector<Point> contour = {
        Point(10, 10),
        Point(40, 5),
        Point(70, 20),
        Point(60, 50),
        Point(30, 60),
        Point(5, 40)
    };
    
    // 创建地形生成器
    TerrainGenerator generator(contour, 80, 70, 1.0);  // 80x70网格，单位尺寸1.0
    
    // 生成地形
    auto terrain = generator.generateTerrain();
    
    // 输出结果
    std::cout << "Generated terrain with " << terrain.size() << " rows and " 
              << terrain[0].size() << " columns" << std::endl;
    
    // 保存到文件
    generator.saveTerrainToFile(terrain, "terrain_heightmap.txt");
    generator.saveContourToFile("contour_points.txt");
    
    // 简单输出中心区域的高度值
    std::cout << "\nCenter region heights (20,20) to (30,30):" << std::endl;
    for (int y = 20; y <= 30; y++) {
        for (int x = 20; x <= 30; x++) {
            std::cout << (int)terrain[y][x] << "\t";
        }
        std::cout << std::endl;
    }
    
    return 0;
}



