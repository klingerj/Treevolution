#pragma once

#include "glm/glm.hpp"

#include <vector>
#include <algorithm>

#include "../OpenGL/Drawable.h"
#include "Raytracing/Raytracing.h"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 nor;
    Vertex() : pos(glm::vec3(0.f)), nor(glm::vec3(0.f)) {}
};

class Triangle {
private:
    glm::vec3 planeNormal;
    std::vector<glm::vec3> points;
public:
    Triangle() {
        points = std::vector<glm::vec3>();
        points.reserve(3);
    }
    void AppendVertex(const glm::vec3& p) {
        points.emplace_back(p);
    }
    std::vector<glm::vec3> GetPoints() {
        return points;
    }
    glm::vec3 GetNormal() {
        return planeNormal;
    }
    Intersection Intersect(const Ray& r) const;
    inline void ComputePlaneNormal() { planeNormal = glm::normalize(glm::cross(points[1] - points[0], points[2] - points[1])); }
};

class Mesh : public Drawable {
protected:
    std::string filename;
private:
    // Lists of vertices and indices
    std::vector<Triangle> triangles;
    std::vector<Vertex> vertices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    // Extra info
    glm::vec3 minPoint;
    glm::vec3 maxPoint;
public:
    Mesh() : filename(""), minPoint(FLT_MAX), maxPoint(-FLT_MAX) {
        triangles = std::vector<Triangle>();
        vertices = std::vector<Vertex>();
        positions = std::vector<glm::vec3>();
        normals = std::vector<glm::vec3>();
        indices = std::vector<unsigned int>();
    }
    void LoadFromFile(const char* filepath);
    //void ExportToFile() const;
    void SetName(const char* name) { filename = std::string(name, 0, 100); }

    void clearData() {
        triangles.clear();
        vertices.clear();
        positions.clear();
        normals.clear();
        indices.clear();
    }

    // Getters
    const std::vector<Triangle>&     GetTriangles() const { return triangles; }
    const std::vector<Vertex>&       GetVertices()  const { return vertices; }
    const std::vector<glm::vec3>&    GetPositions() const { return positions; }
    const std::vector<glm::vec3>&    GetNormals()   const { return normals; }
    const std::vector<unsigned int>& GetIndices()   const { return indices; }
    const glm::vec3& GetMinPos() const { return minPoint; }
    const glm::vec3& GetMaxPos() const { return maxPoint; }

    // Setters
    void SetTriangles(std::vector<Triangle>& t);
    void SetPositions(std::vector<glm::vec3>& p) { positions = p; }
    void SetNormals(std::vector<glm::vec3>& n) { normals = n; }
    void SetIndices(std::vector<unsigned int>& i) { indices = i; }
    void SetMinMaxPosFromTris() {
        for (int i = 0; i < triangles.size(); ++i) {
            for (int j = 0; j < 3; ++j) {
                minPoint.x = std::min(minPoint.x, triangles[i].GetPoints()[j].x);
                minPoint.y = std::min(minPoint.y, triangles[i].GetPoints()[j].y);
                minPoint.z = std::min(minPoint.z, triangles[i].GetPoints()[j].z);
                maxPoint.x = std::max(maxPoint.x, triangles[i].GetPoints()[j].x);
                maxPoint.y = std::max(maxPoint.y, triangles[i].GetPoints()[j].y);
                maxPoint.z = std::max(maxPoint.z, triangles[i].GetPoints()[j].z);
            }
        }
    }

    // Raytracing functions
    Intersection Intersect(const Ray& r) const; // Intersect a single ray with this mesh
    bool Contains(const glm::vec3& p) const; // Check if a point intersects this mesh an odd number of times

    // Inherited Function(s)
    void Create() override;
    GLenum DrawMode() override;
};
