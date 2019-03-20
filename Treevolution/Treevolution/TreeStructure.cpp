#include "TreeStructure.h"

#include <stack>
#include <glm/gtc/matrix_transform.hpp>

TreeStructure::TreeStructure(std::string gram, float minAngle, float maxAngle, 
    float minLen, float maxLen) : mGrammar(gram), mMinAngle(minAngle), mMaxAngle(maxAngle),
    mMinLen(minLen), mMaxLen(maxLen)
{}

void TreeStructure::process()
{

}

TreeStructure::Turtle::Turtle() :
    pos(0, 0, 0),
    up(0, 1, 0, 1),
    forward(0, 0, 1, 1),
    left(1, 0, 0, 1)
{
}

TreeStructure::Turtle::Turtle(const TreeStructure::Turtle& t)
{
    pos = t.pos;
    up = t.up;
    forward = t.forward;
    left = t.left;
}

TreeStructure::Turtle& TreeStructure::Turtle::operator=(const TreeStructure::Turtle& t)
{
    if (&t == this) return *this;

    pos = t.pos;
    up = t.up;
    forward = t.forward;
    left = t.left;
    return *this;
}

void TreeStructure::Turtle::moveForward(float length)
{
    pos = pos + length * glm::vec3(forward);
}

void TreeStructure::Turtle::applyUpRot(float degrees)
{
    glm::mat4 mat = glm::rotate(glm::mat4(1.0), float(Deg2Rad*degrees), glm::vec3(0, 0, 1)); // Z axis
    glm::mat4 world2local = glm::mat4(forward, left, up, glm::vec4(0.0));
    up = world2local * mat * glm::vec4(0, 0, 1, 1);
    left = world2local * mat * glm::vec4(0, 1, 0, 1);
    forward = world2local * mat * glm::vec4(1, 0, 0, 1);
}

void TreeStructure::Turtle::applyLeftRot(float degrees)
{
    glm::mat4 mat = glm::rotate(glm::mat4(1.0), float(Deg2Rad*degrees), glm::vec3(0, 1, 0)); // Y axis
    glm::mat4 world2local = glm::mat4(forward, left, up, glm::vec4(0.0));
    up = world2local * mat * glm::vec4(0, 0, 1, 1);
    left = world2local * mat * glm::vec4(0, 1, 0, 1);
    forward = world2local * mat * glm::vec4(1, 0, 0, 1);
}

void TreeStructure::Turtle::applyForwardRot(float degrees)
{
    glm::mat4 mat = glm::rotate(glm::mat4(1.0), float(Deg2Rad*degrees), glm::vec3(1, 0, 0)); // X axis
    glm::mat4 world2local = glm::mat4(forward, left, up, glm::vec4(0.0));
    up = world2local * mat * glm::vec4(0, 0, 1, 1);
    left = world2local * mat * glm::vec4(0, 1, 0, 1);
    forward = world2local * mat * glm::vec4(1, 0, 0, 1);
}