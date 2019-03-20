#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "../OpenGL/Drawable.h"

typedef struct line {
    glm::vec3 start;
    glm::vec3 end;
} Line;

class DrawableLine : public Drawable {
private:
    std::vector<Line> lineSegments;
public:
    DrawableLine();
    ~DrawableLine();

    void addLineSegment(const Line& l) {
        lineSegments.push_back(l);
    }

    void Create() override;
    GLenum DrawMode() override;
};
