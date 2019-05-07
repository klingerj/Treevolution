#pragma once

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <random>
#include <functional>
#include <glm/glm.hpp>

#include "../Scene/Mesh.h"

class TreeNode {
public:
    char name;
    float param;
    float param2;
    glm::vec3 axis;
    TreeNode* parent;
    std::vector<TreeNode*> children;

    TreeNode();
    TreeNode(TreeNode* t);
    TreeNode(char c, float f, TreeNode* parent);
    TreeNode(char c, float f, glm::vec3 &a, TreeNode* parent);
    TreeNode(char c, float f, float f2, glm::vec3 &a, TreeNode* parent);
    ~TreeNode();

    char GetName() const {
        return name;
    }
};

class TreeStructure {
public:
    int fitnessScore;
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
    void ClearNodeList() { nodeList.clear(); }

    void CreateNodeList(TreeNode* root);
    void CreateCopyNodeList(TreeNode* root, TreeNode* toCopy);

    void processNode(TreeNode * currNode, Mesh & baseMesh, Mesh & leafMesh);

    // Get geometry from running the turtle
    Mesh GetTreeMesh(Mesh & baseMesh, Mesh & leafMesh);

    TreeNode* AddChildToNode(TreeNode* parent, char c);

    // Genetic algoirthm functions
    void Crossover(TreeStructure* parent2);
    void Grow(const std::map<std::string, std::vector<std::string>> &rules);
    void Cut();
    void Alter();
    int Mutate(const std::map<std::string, std::vector<std::string>> &rules);

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
