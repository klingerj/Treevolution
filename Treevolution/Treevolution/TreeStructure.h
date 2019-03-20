#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

class TreeStructure {
public:
    TreeStructure(std::string gram, float minAngle, float maxAngle, float minLen, float maxLen);
    ~TreeStructure() {}

    // Get geometry from running the turtle
    void process();

protected:
    std::string mGrammar;
    float mMinAngle;
    float mMaxAngle;
    float mMinLen;
    float mMaxLen;

    class Turtle
    {
    public:
        Turtle();
        Turtle(const Turtle& t);
        Turtle& operator=(const Turtle& t);

        void moveForward(float distance);
        void applyUpRot(float degrees);
        void applyLeftRot(float degrees);
        void applyForwardRot(float degrees);

        glm::vec3 pos;
        glm::vec4 up;
        glm::vec4 forward;
        glm::vec4 left;
    };
};