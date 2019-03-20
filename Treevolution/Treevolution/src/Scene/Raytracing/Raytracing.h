#pragma once

#pragma once

#include "glm/glm.hpp"
#include "../../Globals.h"

// This file contains necessary classes for raytracing. This is used for point-in-mesh queries.

class Ray {
private:
    glm::vec3 origin;
    glm::vec3 direction;
public:
    Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(d) {}
    Ray GetTransformedCopy(const glm::mat4& t) const {
        return Ray(glm::vec3(t * glm::vec4(origin, 1.0f)),
            glm::vec3(t * glm::vec4(direction, 0.0f)));
    }
    const glm::vec3& GetDirection() const { return direction; }
    const glm::vec3& GetOrigin() const { return origin; }
    void SetOrigin(const glm::vec3& o) { origin = o; }
};

class Intersection {
private:
    glm::vec3 point;
    glm::vec3 normal;
    float t;
public:
    Intersection() : Intersection(glm::vec3(0.f), glm::vec3(0.f), -1.0f) {}
    Intersection(const glm::vec3& p, const glm::vec3& n, float t) : point(p), normal(n), t(t) {}
    Ray SpawnRayAtPoint(const Ray& r) const {
        const glm::vec3 originOffset = normal * EPSILON * ((dot(normal, r.GetDirection()) > 0.0f) ? 1.0f : -1.0f);
        return Ray(point + originOffset, r.GetDirection());
    }
    bool IsValid() const { return t > 0.0f; }
    float GetT() const { return t; }
    const glm::vec3& GetPoint() const { return point; }
};
