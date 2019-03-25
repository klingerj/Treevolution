#pragma once

#include <string>
#include <vector>
#include <stack>
#include <glm/glm.hpp>

#include "../Scene/Mesh.h"

class TreeNode {
public:
    char name;
    float param;
    glm::vec3 axis;
    std::vector<TreeNode*> children;

    TreeNode();
    TreeNode(TreeNode* t);
    TreeNode(char c, float f);
    TreeNode(char c, float f, glm::vec3 &a);
    ~TreeNode() {}

    char GetName() const {
        return name;
    }
};

class TreeStructure {
public:
    TreeStructure(std::string gram, float minAngle, float maxAngle, float minLen, float maxLen);
    TreeStructure(TreeStructure* t, TreeNode* root);
    ~TreeStructure() {}

    // getters
    TreeNode* GetRoot() { return mRoot; }
    int GetCount() { return nodeList.size(); }
    TreeNode* GetNodeAtCount(int count) { return nodeList[count]; }
    std::string GetGram() { return mGrammar; }
    float GetMinAngle() { return mMinAngle; }
    float GetMaxAngle() { return mMaxAngle; }
    float GetMinLen() { return mMinLen; }
    float GetMaxLen() { return mMaxLen; }

    void CreateNodeList(TreeNode* root);

    void processNode(TreeNode* currNode, Mesh &baseMesh);

    // Get geometry from running the turtle
    Mesh GetTreeMesh(Mesh &branch);

    TreeNode* AddChildToNode(TreeNode* parent, char c);

    // Genetic algoirthm functions
    std::vector<TreeStructure> Crossover(TreeStructure* parent2);
    void Grow(TreeNode* toGrow);
    void Cut(TreeNode* toCut);
    void Alter();

    // TODO: Free all heap-allocated tree nodes

private: 
    void ConstructTree(TreeNode *root, std::string substring);

protected:
    std::string mGrammar;
    float mMinAngle;
    float mMaxAngle;
    float mMinLen;
    float mMaxLen;

    // Tree data
    TreeNode* mRoot;
    std::vector<TreeNode*> nodeList;
    std::stack<TreeNode*> nodeStack;

    std::vector<Triangle> tris;

    class Turtle
    {
    public:
        Turtle();
        Turtle(const Turtle& t);
        Turtle& operator=(const Turtle& t);

        void moveForward(float distance);
        void applyRot(glm::vec3 axis, float angle);

        glm::vec3 pos;
        glm::vec4 up;
        glm::vec4 forward;
        glm::vec4 left;
    };

    std::stack<Turtle> turtStack;
    Turtle currTurtle;
};
