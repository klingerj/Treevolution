#pragma once

#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../Globals.h"
#include <iostream>

class Camera {
private:
    glm::vec3 eye; // world space eye position
    glm::vec3 ref; // world space ref position
    glm::vec3 look; // forward vector
    glm::vec3 right; // right vector
    glm::vec3 up; // up vector
    float fovy; // field of view in radians
    float nearPlane; // near clip plane
    float farPlane; // far clip plane
    float aspect; // aspect ratio
public:
    Camera(const glm::vec3 e, const glm::vec3& r, const float fov, const float a, const float n, const float f) :
        eye(e), ref(r), fovy(fov), aspect(a), nearPlane(n), farPlane(f), look(0, 0, -1), right(0, 0, 1), up(0, 1, 0) {
        UpdateAttributes();
    }
    ~Camera() {}
    float GetAspect() const { return aspect; }
    float GetFovy() const { return fovy; }
    glm::vec3 GetEye() const { return eye; }
    const glm::mat4 GetViewProj() const { return glm::perspective(fovy, aspect, nearPlane, farPlane) *
                                                 glm::lookAt(eye, ref, WORLD_UP_VECTOR); }
    const glm::mat4 GetView() const { return glm::lookAt(eye, ref, WORLD_UP_VECTOR); }
    const glm::mat4 GetProj() const { return glm::perspective(fovy, aspect, nearPlane, farPlane); }
    const float GetFarPlane() const { return farPlane; }
    void UpdateAttributes() { ref = eye + look; }
    void TranslateAlongLook(const float amt) {
        eye += amt * look;
        UpdateAttributes();
    }
    void TranslateAlongUp(const float amt) {
        eye += amt * up;
        UpdateAttributes();
    }
    void TranslateAlongRight(const float amt) {
        eye += amt * right;
        UpdateAttributes();
    }
    void RotateAboutLook(const float amt) {
        glm::mat3 rot = glm::mat3(glm::rotate(glm::mat4(1.0f), amt, look));
        right = rot * right;
        glm::normalize(right);
        up = rot * up;
        glm::normalize(up);
        UpdateAttributes();
    }
    void RotateAboutUp(const float amt) {
        glm::mat3 rot = glm::mat3(glm::rotate(glm::mat4(1.0f), amt, up));
        right = rot * right;
        glm::normalize(right);
        look = rot * look;
        glm::normalize(look);
        UpdateAttributes();
    }
    void RotateAboutRight(const float amt) {
        glm::mat3 rot = glm::mat3(glm::rotate(glm::mat4(1.0f), amt, right)); // TODO: this no work
        look = rot * look;
        glm::normalize(look);
        up = rot * up;
        glm::normalize(up);
        UpdateAttributes();
    }
};
