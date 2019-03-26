#pragma once

#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "../OpenGL/Drawable.h"

class DrawablePoints : public Drawable {
private:
    std::vector<glm::vec3> points;
public:
    DrawablePoints();
    ~DrawablePoints();

    void addPoint(const glm::vec3& p) {
        points.push_back(p);
    }

    void Create() override;
    GLenum DrawMode() override;
};
