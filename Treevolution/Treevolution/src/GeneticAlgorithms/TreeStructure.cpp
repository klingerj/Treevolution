#include "TreeStructure.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define Deg2Rad 0.017453292519943295769236907684886

TreeNode::TreeNode() : TreeNode(0, -1.0f, nullptr)
{

}

TreeNode::TreeNode(TreeNode* t) : name(t->name), param(t->param),
    axis(t->axis), parent(t->parent), children(t->children)
{

}

TreeNode::TreeNode(char c, float f, TreeNode* parent) : name(c), param(f),
    parent(parent)
{
    axis = glm::vec3(0.f);
}

TreeNode::TreeNode(char c, float f, glm::vec3 &a, TreeNode* parent) : 
    name(c), param(f), axis(a), parent(parent)
{

}

TreeStructure::TreeStructure(int id, std::string gram, float minAngle, float maxAngle, 
    float minLen, float maxLen) : mId(id), mGrammar(gram), mMinAngle(minAngle), mMaxAngle(maxAngle),
    mMinLen(minLen), mMaxLen(maxLen), mRoot(nullptr)
{
    mGenerator.seed(mId);
    mAxisDist = std::uniform_real_distribution<float>(-1.0, 1.0);
    mAngleDist = std::uniform_real_distribution<float>(mMinAngle, mMaxAngle);
    mLenDist = std::uniform_real_distribution<float>(mMinLen, mMaxLen);
    
    mRoot = new TreeNode();
    nodeList.push_back(mRoot);

    ConstructTree(mRoot, gram);

    currTurtle  = Turtle();
}

TreeStructure::TreeStructure(TreeStructure* t, TreeNode* root) : mId(t->GetId()), mGrammar(t->GetGram()), 
    mMinAngle(t->GetMinAngle()), mMaxAngle(t->GetMaxAngle()), mMinLen(t->GetMinLen()), mMaxLen(t->GetMaxLen()), mRoot(root)
{
    mGenerator.seed(mId);
    mAxisDist = std::uniform_real_distribution<float>(-1.0, 1.0);
    mAngleDist = std::uniform_real_distribution<float>(mMinAngle, mMaxAngle);
    mLenDist = std::uniform_real_distribution<float>(mMinLen, mMaxLen);
    
    nodeList.clear();
    CreateNodeList(mRoot);
}

void TreeStructure::ConstructTree(TreeNode *root, std::string substring)
{
    if (substring.length() != 0)
    {
        char sym = substring.at(0);

        if (sym == 'F' || sym == '-' || sym == '+')
        {
            TreeNode* next = AddChildToNode(root, sym);
            ConstructTree(next, substring.substr(1, substring.length() - 1));
        }
        else if (sym == '[')
        {
            nodeStack.push(root);
            ConstructTree(root, substring.substr(1, substring.length() - 1));
        }
        else if (sym == ']')
        {
            TreeNode* returnNode = nodeStack.top();
            nodeStack.pop();
            ConstructTree(returnNode, substring.substr(1, substring.length() - 1));
        }
    }
}

TreeNode* TreeStructure::AddChildToNode(TreeNode* parent, char c) 
{
    TreeNode* newNode = new TreeNode();

    float p = 0.f;
    if (c == 'F')
    {
        // generate random float between mMinLen and mMaxLen 
        p = (float(std::rand()) / RAND_MAX) * (mMaxLen - mMinLen) + mMinLen;
        //p = 1.0f;
        p = mLenDist(mGenerator);
        (*newNode) = TreeNode(c, p, parent);
    }
    else if (c == '-')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = (float(rand()) / RAND_MAX) * 2 - 1;
        float y = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float z = (float(std::rand()) / RAND_MAX) * 2 - 1;
        x = mAxisDist(mGenerator);
        y = mAxisDist(mGenerator);
        z = mAxisDist(mGenerator);
        glm::vec3 a(x, y, z);
        a = glm::normalize(a);

        //a = glm::vec3(0, 0, 1);

        p = -1.f * ((float(std::rand()) / RAND_MAX) * (mMaxAngle - mMinAngle) + mMinAngle);
        //p = -45.0f;
        p = -1.f * mAngleDist(mGenerator);
        (*newNode) = TreeNode(c, p, a, parent);
    }
    else if (c == '+')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float y = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float z = (float(std::rand()) / RAND_MAX) * 2 - 1;
        x = mAxisDist(mGenerator);
        y = mAxisDist(mGenerator);
        z = mAxisDist(mGenerator);
        glm::vec3 a(x, y, z);
        a = glm::normalize(a);

        //a = glm::vec3(0, 0, 1);

        p = (float(std::rand()) / RAND_MAX) * (mMaxAngle - mMinAngle) + mMinAngle;
        //p = 45.0f;
        p = mAngleDist(mGenerator);
        (*newNode) = TreeNode(c, p, a, parent);
    }

    parent->children.push_back(newNode);
    nodeList.push_back(newNode);

    return newNode;
}

void TreeStructure::CreateNodeList(TreeNode* root)
{
    nodeList.push_back(root);
    for (TreeNode* c : root->children)
    {
        CreateNodeList(c);
    }
}

int GetIndexInParentList(TreeNode* node)
{
    TreeNode* par = node->parent;
    int idx = -1;
    for (int i = 0; i < par->children.size(); i++)
    {
        if (par->children[i] == node) { idx = i; }
    }
    return idx;
}

void TreeStructure::Crossover(TreeStructure* parent2)
{
    // get a random node from both this and parent2
    int rand1 = std::rand() % this->nodeList.size();
    int rand2 = std::rand() % parent2->GetCount();
    TreeNode* subtree1 = this->GetNodeAtCount(rand1);
    TreeNode* subtree2 = parent2->GetNodeAtCount(rand2);
    int idx1 = GetIndexInParentList(subtree1);
    int idx2 = GetIndexInParentList(subtree2);

    // swap the subtrees
    subtree1->parent->children[idx1] = subtree2;
    subtree2->parent->children[idx2] = subtree1;
    TreeNode* temp = subtree1->parent;
    subtree1->parent = subtree2->parent;
    subtree2->parent = temp;
}

void TreeStructure::Grow()
{
    // TODO
}

void TreeStructure::Cut()
{
    // get a random node
    int rand = std::rand() % this->nodeList.size();
    TreeNode* gene = this->GetNodeAtCount(rand);

    if (gene != mRoot)
    {
        int idx = GetIndexInParentList(gene);
        gene->parent->children.erase(gene->parent->children.begin() + idx);
    }
}

void TreeStructure::Alter()
{
    // get a random node
    int rand = std::rand() % this->nodeList.size();
    TreeNode* gene = this->GetNodeAtCount(rand);

    // recompute random parameters
    if (gene->name == 'F')
    {
        // generate random float between mMinLen and mMaxLen 
        gene->param = mLenDist(mGenerator); //(float(std::rand()) / RAND_MAX) * (mMaxLen - mMinLen) + mMinLen;
    }
    else if (gene->name == '+')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = mAxisDist(mGenerator); //(float(std::rand()) / RAND_MAX) * 2 - 1;
        float y = mAxisDist(mGenerator); //(float(std::rand()) / RAND_MAX) * 2 - 1;
        float z = mAxisDist(mGenerator); //(float(std::rand()) / RAND_MAX) * 2 - 1;
        glm::vec3 a(x, y, z);
        gene->axis = glm::normalize(a);

        gene->param = mAngleDist(mGenerator); //(float(std::rand()) / RAND_MAX) * (mMaxAngle - mMinAngle) + mMinAngle;
    }
    else if (gene->name == '-')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = mAxisDist(mGenerator); //(float(std::rand()) / RAND_MAX) * 2 - 1;
        float y = mAxisDist(mGenerator); //(float(std::rand()) / RAND_MAX) * 2 - 1;
        float z = mAxisDist(mGenerator); //(float(std::rand()) / RAND_MAX) * 2 - 1;
        glm::vec3 a(x, y, z);
        gene->axis = glm::normalize(a);

        gene->param = mAngleDist(mGenerator); //-1.f * ((float(std::rand()) / RAND_MAX) * (mMaxAngle - mMinAngle) + mMinAngle);
    }
}

void TreeStructure::Mutate()
{
    std::uniform_real_distribution<float> chooseDist(0.0, 3.0);
    float action = chooseDist(mGenerator);

    if (action <= 1)
    {
        Grow();
    }
    else if (action <= 2)
    {
        Cut();
    }
    else if (action <= 3)
    {
        Alter();
    }
}

void TreeStructure::processNode(TreeNode* currNode, Mesh &baseMesh)
{
    // perform the desired action for this node
    if (currNode->name == 'F')
    {
        // store current position then find the next position
        glm::vec3 startPos = currTurtle.pos;
        currTurtle.moveForward(currNode->param);
        glm::vec3 endPos = currTurtle.pos;

        // find the scale and translation of the branch geometry
        const float dist = glm::length(startPos - endPos);
        const float radius = 0.3f;
        const glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(radius, dist, radius));
        
        const glm::vec3 newStart = glm::vec3(startPos.x, startPos.y, startPos.z);
        const glm::vec3 newEnd = glm::vec3(endPos.x, endPos.y, endPos.z);
        const glm::mat4 trans = glm::translate(glm::mat4(1.0), (newEnd - newStart) * 0.5f + newStart);

        // find the rotation of the branch geometry
        const glm::mat4 rot = glm::mat4(currTurtle.left, currTurtle.up,
            currTurtle.forward, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        // create a transormation matrix
        const glm::mat4 transform = trans * rot * scale;

        // add all the triangles of baseMesh modified by this matrix to tris
        std::vector<Triangle> origTris = baseMesh.GetTriangles();
        for (Triangle t : origTris)
        {
            std::vector<glm::vec3> oldPts = t.GetPoints();

            Triangle newT;
            newT.AppendVertex(transform * glm::vec4(oldPts[0], 1.0));
            newT.AppendVertex(transform * glm::vec4(oldPts[1], 1.0));
            newT.AppendVertex(transform * glm::vec4(oldPts[2], 1.0));
            newT.ComputePlaneNormal();

            tris.push_back(newT);
        }
    }
    else if (currNode->name == '-' || currNode->name == '+')
    {
        currTurtle.applyRot(currNode->axis, currNode->param);
    }
    
    for (TreeNode* child : currNode->children)
    {
        // begin new branch
        turtStack.push(currTurtle);

        // process child's subtree
        processNode(child, baseMesh);

        // end branch
        currTurtle = turtStack.top();
        turtStack.pop();
    }
}

Mesh TreeStructure::GetTreeMesh(Mesh &baseMesh)
{
    tris.clear();
    
    // clear turtle stack
    while (!turtStack.empty()) { turtStack.pop(); }

    currTurtle = Turtle();
    
    Mesh output;

    processNode(mRoot, baseMesh);

    // set tris to traingles of this mesh
    output.SetTriangles(tris);
    output.SetMinMaxPosFromTris();

    return output;
}

TreeStructure::Turtle::Turtle() :
    pos(0, 0, 0),
    up(0, 1, 0, 0),
    forward(0, 0, 1, 0),
    left(1, 0, 0, 0)
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
    pos = pos + length * glm::vec3(up);
}

void TreeStructure::Turtle::applyRot(glm::vec3 axis, float angle)
{
    glm::mat4 mat = glm::rotate(glm::mat4(1.0), float(Deg2Rad*angle), axis);
    glm::mat4 world2local = glm::mat4(left, up, forward, glm::vec4(0.0));
    up = world2local * mat * glm::vec4(0, 1, 0, 0);
    left = world2local * mat * glm::vec4(1, 0, 0, 0);
    forward = world2local * mat * glm::vec4(0, 0, 1, 0);
}
