#include "DrawableLine.h"

DrawableLine::DrawableLine() : Drawable(), lineSegments() {}

DrawableLine::~DrawableLine() {}

void DrawableLine::Create() {
    // Indices
    genBufIdx();

    // Create an indices vector
    std::vector<unsigned int> indices = std::vector<unsigned int>();
    for (int i = 0; i < lineSegments.size(); ++i) {
        indices.emplace_back(i);
        indices.emplace_back(i + 1);
    }

    count = (int)indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Positions
    std::vector<glm::vec3> lineData = std::vector<glm::vec3>();
    lineData.reserve(lineSegments.size() * 2);
    for (int i = 0; i < lineSegments.size(); ++i) {
        lineData.emplace_back(lineSegments[i].start);
        lineData.emplace_back(lineSegments[i].end);
    }
    genBufPos();
    glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * lineData.size(), lineData.data(), GL_STATIC_DRAW);
}

GLenum DrawableLine::DrawMode() {
    return GL_LINES;
}
