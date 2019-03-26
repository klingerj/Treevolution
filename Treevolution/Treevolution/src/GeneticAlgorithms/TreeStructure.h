#pragma once

#include <string>
#include <vector>
#include <stack>
#include <random>
#include <functional>
#include <glm/glm.hpp>

#include "../Scene/Mesh.h"

class TreeNode {
public:
    char name;
    float param;
    glm::vec3 axis;
    TreeNode* parent;
    std::vector<TreeNode*> children;

    TreeNode();
    TreeNode(TreeNode* t);
    TreeNode(char c, float f, TreeNode* parent);
    TreeNode(char c, float f, glm::vec3 &a, TreeNode* parent);
    ~TreeNode();

    char GetName() const {
        return name;
    }
};

class TreeStructure {
public:
    TreeStructure(int id, std::string gram, float minAngle, float maxAngle, 
        float minLen, float maxLen);
    TreeStructure(TreeStructure* t, TreeNode* root);
    ~TreeStructure();

    // getters and setters
    TreeNode* GetRoot() { return mRoot; }
    size_t GetCount() { return nodeList.size(); }
    TreeNode* GetNodeAtCount(int count) { return nodeList[count]; }
    int GetId() { return mId; }
    std::string GetGram() { return mGrammar; }
    float GetMinAngle() { return mMinAngle; }
    float GetMaxAngle() { return mMaxAngle; }
    float GetMinLen() { return mMinLen; }
    float GetMaxLen() { return mMaxLen; }
    void SetRoot(TreeNode* r) { mRoot = r; }

    void CreateNodeList(TreeNode* root);

    void processNode(TreeNode* currNode, Mesh &baseMesh);

    // Get geometry from running the turtle
    Mesh GetTreeMesh(Mesh &branch);

    TreeNode* AddChildToNode(TreeNode* parent, char c);

    // Genetic algoirthm functions
    void Crossover(TreeStructure* parent2);
    void Grow();
    void Cut();
    void Alter();
    void Mutate();

    // TODO: Free all heap-allocated tree nodes

private: 
    void ConstructTree(TreeNode *root, std::string substring);

protected:
    int mId;
    std::string mGrammar;
    float mMinAngle;
    float mMaxAngle;
    float mMinLen;
    float mMaxLen;

    // RNG
    std::default_random_engine mGenerator;
    std::uniform_real_distribution<float> mAxisDist;
    std::uniform_real_distribution<float> mAngleDist;
    std::uniform_real_distribution<float> mLenDist;

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
