#include "DrawablePoints.h"

DrawablePoints::DrawablePoints() : Drawable(), points() {}

DrawablePoints::~DrawablePoints() {}

void DrawablePoints::Create() {
    // Indices
    genBufIdx();

    // Create an indices vector
    std::vector<unsigned int> indices = std::vector<unsigned int>();
    for (int i = 0; i < points.size(); ++i) {
        indices.emplace_back(i);
    }

    count = (int)indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Positions
    std::vector<glm::vec3> lineData = std::vector<glm::vec3>();
    lineData.reserve(points.size());
    for (int i = 0; i < points.size(); ++i) {
        lineData.emplace_back(points[i]);
    }

    genBufPos();
    glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * lineData.size(), lineData.data(), GL_STATIC_DRAW);
}

GLenum DrawablePoints::DrawMode() {
    return GL_POINTS;
}
