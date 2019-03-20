#pragma once

#include "glm/glm.hpp"

#include <vector>

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
public:
    Mesh() : filename("") {
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

    // Setters
    void SetPositions(std::vector<glm::vec3>& p) { positions = p; }
    void SetNormals(std::vector<glm::vec3>& n) { normals = n; }
    void SetIndices(std::vector<unsigned int>& i) { indices = i; }

    // Raytracing functions
    Intersection Intersect(const Ray& r) const; // Intersect a single ray with this mesh
    bool Contains(const glm::vec3& p) const; // Check if a point intersects this mesh an odd number of times

    // Inherited Function(s)
    void Create() override;
    GLenum DrawMode() override;
};
